#pragma once

#include "board_all.h"
#include "eval.h"

// E D C B
// 6 8 9 A
// 0 2 2 1
// 2 4 1 0
// ==>
// F F F F
// 6 F F F
// 0 0 0 0
// 0 0 0 0

// value stored in specific table, for specific hash

//static constexpr r_t UNDEFINED = numeric_limits<r_t>::quiet_NaN();
//static constexpr r_t UNKNOWN = numeric_limits<r_t>::lowest();

class Special {
private:
    static constexpr r_t UNDEFINED = std::numeric_limits<r_t>::quiet_NaN();
    static constexpr r_t UNKNOWN = std::numeric_limits<r_t>::lowest();
    static constexpr u64 CHUNK_SIZE = E(25);

public:
    static constexpr r_t get_undefined() { return UNDEFINED; }

    static constexpr r_t get_unknown() { return UNKNOWN; }

    static constexpr u64 get_chunk_size() { return CHUNK_SIZE; }

    static constexpr bool is_undefined(const r_t value) { return std::isnan(value); }

    static constexpr bool is_unknown(const r_t value) { return value == UNKNOWN; }
};

class Table {
private:
    const b_t BASE;
    const u8 GOAL;
    const u64 SIZE;
    std::vector<r_t> VALUES;

    static u8 get_goal(const b_t board) {
        u8 G = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 cell = board.get_cell(i);
            if (cell == 0 || cell == 0xFu) { continue; }
            if (G != 0) { return 0; }
            G = cell;
        }
        return G;
    }

    static u64 get_size(const b_t board) {
        return power(get_goal(board) + 1, board.empty_count());
    }

    u64 get_hash(const b_t board) const {
        u64 hash = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = BASE.get_cell(i);
            const u8 cell = board.get_cell(i);
            if (base == 0 && cell > GOAL) { return 0; }
            if (base == GOAL && cell != GOAL) { return 0; }
            if (base == 0xFu && cell <= GOAL) { return 0; }
            if (base == 0) { hash = hash * (GOAL + 1) + cell; }
        }
        return hash;
    }

    u64 get_board(u64 hash) const {
        std::array<u8, 16> cells{};
        u8 index = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = BASE.get_cell(i);
            if (base == 0) {
                cells[index++] = hash % (GOAL + 1);
                hash /= GOAL + 1;
            }
        }
        u64 board = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = BASE.get_cell(i);
            u8 cell;
            if (base == 0) { cell = cells[--index]; }
            if (base == GOAL) { cell = GOAL; }
            if (base == 0xFu) { cell = 0xFu; }
            //set_cell(board, i, cell);
            board |= u64(cell) << (i * 4);
        }
        return board;
    }

    bool is_goal(const b_t board) const {
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = BASE.get_cell(i);
            const u8 cell = board.get_cell(i);
            if (base == 0 && cell > GOAL) { return false; }
            if (base == GOAL && cell != GOAL + 1) { return false; }
            if (base == 0xFu && cell <= GOAL) { return false; }
        }
        return true;
    }

    bool is_goal_state(const b_t state) const {
        for (const Dir dir: DIRS) {
            const b_t afterstate = state.slid(dir);
            if (afterstate == state) { continue; }
            if (is_goal(afterstate)) { return true; }
        }
        return false;
    }

public:
    explicit Table(const u64 base) : BASE(base), GOAL(get_goal(base)), SIZE(get_size(base)) {
        std::cout << "Goal: " << u32(GOAL) << std::endl;
        std::cout << "Space: " << u32(BASE.empty_count()) << std::endl;
        std::cout << "Size: " << SIZE << std::endl;
        std::cout << "GBs: " << r_t(SIZE) * sizeof(r_t) / 1e9 << std::endl;
        auto start = time_now();
        VALUES.resize(get_size(BASE), Special::get_unknown());
        std::cout << "Allocation finished: " << time_since(start) / 1e6 << " s" << std::endl;
    }

    b_t get_base() const {
        return BASE;
    }

    void init_goal_states(const r_t goal_value) {
        auto start = time_now();
        for (u64 hash = 0; hash < SIZE; ++hash) {
            if (hash % (SIZE / 100) == 0) std::cout << "Progress: " << r_t(hash) / r_t(SIZE) << std::endl;
            const u64 board = get_board(hash);
            if (is_goal_state(board)) {
                VALUES[hash] = goal_value;
            }
        }
        std::cout << "Goal state init finished: " << time_since(start) / 1e6 << " s" << std::endl;
    }

    r_t get_state_value(const b_t board) const {
        const u64 hash = get_hash(board);
        if (hash == 0) { return Special::get_undefined(); }
        return VALUES[hash];
        /*for (const auto &b: get_transformations(board)) {
            const u64 hash = get_hash(b);
            if (hash != 0) {
                return VALUES[hash];
            }
        }
        return UNDEFINED;*/
    }

    void set_state_value(const b_t board, const r_t value) {
        const u64 hash = get_hash(board);
        if (hash == 0) { return; }
        VALUES[hash] = value;
    }

    void save_values(std::ofstream &file) const {
        file.write((const char *) &BASE, sizeof(BASE));
        for (u64 i = 0; i < SIZE; i += Special::get_chunk_size()) {
            const u64 chunk_size = std::min(Special::get_chunk_size(), SIZE - i);
            file.write((const char *) &VALUES[i], std::streamsize(chunk_size * sizeof(VALUES[0])));
        }
    }

    void load_values(std::ifstream &file) const {
        u64 base_buffer;
        file.read((char *) &base_buffer, sizeof(base_buffer));
        assert(b_t(base_buffer) == BASE);
        for (u64 i = 0; i < SIZE; i += Special::get_chunk_size()) {
            const u64 chunk_size = std::min(Special::get_chunk_size(), SIZE - i);
            file.read((char *) &VALUES[i], std::streamsize(chunk_size * sizeof(VALUES[0])));
        }
    }

    r_t get_known_ratio() const {
        u64 count = 0;
        for (const r_t value: VALUES) {
            if (!Special::is_unknown(value)) { ++count; }
        }
        return r_t(count) / r_t(SIZE);
    }
};

class Endgame {
private:
    std::vector<Table> tables;

    /*r_t get_state_value(const u64 board) {
        for (const auto &table: tables) {
            const r_t value = table.get_state_value(board);
            if (value != UNDEFINED) {
                return value;
            }
        }
        return UNDEFINED;
    }*/

    r_t eval_state(const b_t board) {
        r_t max_max_eval = Special::get_undefined();
        for (const b_t state: board.get_transformations()) {
            for (auto &table: tables) {
                const r_t value = table.get_state_value(state);
                if (Special::is_undefined(value)) { continue; }
                if (!Special::is_unknown(value)) {
                    if (Special::is_undefined(max_max_eval) || value > max_max_eval) {
                        max_max_eval = value;
                    }
                    continue;
                }
                r_t max_eval = 0;
                for (const Dir dir: DIRS) {
                    const b_t afterstate = state.slid(dir);
                    if (afterstate == state) { continue; }
                    r_t reward = r_t(state.get_reward(dir));
                    //reward = 0;
                    const r_t eval = reward + eval_afterstate(afterstate);
                    max_eval = std::max(max_eval, eval);
                }
                table.set_state_value(state, max_eval);
                if (Special::is_undefined(max_max_eval) || max_eval > max_max_eval) {
                    max_max_eval = max_eval;
                }
            }
        }
        return max_max_eval;
    }

    r_t eval_afterstate(const b_t afterstate) {
        const u64 empty_mask = afterstate.empty_mask();
        const u8 empty_count = b_t::empty_count(empty_mask);
        u64 mask = 1;
        r_t avg_eval = 0;
        for (u8 i = 0; i < empty_count; mask <<= 4) {
            if (empty_mask & mask) {
                for (const auto &[shift, prob]: SHIFTS) {
                    const r_t eval = eval_state(afterstate | (mask << shift));
                    if (Special::is_undefined(eval)) { return Special::get_undefined(); }
                    avg_eval += prob * eval;
                }
                ++i;
            }
        }
        return avg_eval / r_t(empty_count);
    }

public:
    explicit Endgame(const std::vector<u64> &bases) {
        for (const u64 base: bases) {
            tables.emplace_back(base);
            print_board(base);
        }
    }

    void init_goal_states(const r_t goal_value) {
        for (auto &table: tables) {
            table.init_goal_states(goal_value);
        }
    }

    void bruteforce_values() {
        auto start = time_now();
        for (auto &table: tables) {
            eval_afterstate(table.get_base());
        }
        std::cout << "Bruteforce finished: " << time_since(start) / 1e6 << " s" << std::endl;
    }

    r_t get_state_value(const u64 board) {
        return eval_state(board);
    }

    r_t get_afterstate_value(const b_t board) {
        return eval_afterstate(board);
    }

    Eval eval_board(const b_t board) {
        Eval best{};
        for (const Dir dir: DIRS) {
            const b_t afterstate = board.slid(dir);
            if (afterstate == board) { continue; }
            const r_t eval = get_afterstate_value(afterstate);
            if (Special::is_undefined(eval)) { continue; }
            if (eval > best.eval) {
                best = {dir, eval, board.get_reward(dir), afterstate};
            }
        }
        return best;
    }

    void save_values(const std::string &id) {
        const std::string filename = "endgame-" + id + ".bin";
        auto start = time_now();
        std::ofstream file("../endgames_backups/" + filename, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Error opening: " << filename << std::endl;
            return;
        }
        u64 size = tables.size();
        file.write((const char *) &size, sizeof(size));
        for (const auto &table: tables) {
            table.save_values(file);
        }
        file.close();
        std::cout << "Saving " << filename << " finished: " << time_since(start) / 1e6 << " s" << std::endl;
    }

    void load_values(const std::string &id) {
        const std::string filename = "endgame-" + id + ".bin";
        auto start = time_now();
        std::ifstream file("../endgames_backups/" + filename, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Error opening: " << filename << std::endl;
            return;
        }
        u64 size_buffer;
        file.read((char *) &size_buffer, sizeof(size_buffer));
        assert(size_buffer == tables.size());
        for (auto &table: tables) {
            table.load_values(file);
        }
        file.close();
        std::cout << "Loading " << filename << " finished: " << time_since(start) / 1e6 << " s" << std::endl;
    }

    void print_known_ratios() {
        for (const auto &table: tables) {
            std::cout << "Known ratio: " << table.get_known_ratio() << std::endl;
            std::cout << table.get_base() << std::endl;
        }
    }
};

/*class Endgame {
private:
    //const u64 BASE;
    //const u8 GOAL;
    //vector<r_t> evals;
    vector<Table> tables;

    //static constexpr r_t UNKNOWN = -numeric_limits<r_t>::infinity();
    //static constexpr r_t INF = numeric_limits<r_t>::infinity();

    u64 to_hash(const u64 board) const {
        u64 hash = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (board >> (i * 4)) & 0xFu;
            if (base == 0) {
                if (cell > G) { return 0; }
                hash = hash * (G + 1) + cell;
            } else if (base == G) {
                if (cell != G) { return 0; }
            } else {
                if (cell <= G) { return 0; }
            }
        }
        return hash;
    }*/

/*u64 from_hash(u64 hash) const {
    u64 board = 0;
    array<u8, 16> cells{};
    u8 index = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 base = (B >> (i * 4)) & 0xFu;
        if (base == 0) {
            cells[index++] = hash % (G + 1);
            hash /= G + 1;
        }
    }
    u8 large = G + 1;
    for (u8 i = 0; i < 16; ++i) {
        const u8 base = (B >> (i * 4)) & 0xFu;
        u8 cell;
        if (base == 0) {
            cell = cells[--index];
        } else if (base == G) {
            cell = base;
        } else {
            cell = large++;
        }
        board |= u64(cell) << (i * 4);
    }
    return board;
}*/

/*u8 get_largest_small(const u64 board) const {
    u8 largest = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 base = (B >> (i * 4)) & 0xFu;
        const u8 cell = (board >> (i * 4)) & 0xFu;
        if (base == 0) {
            largest = max(largest, cell);
        }
    }
    return largest;
}*/

/*bool is_goal(const u64 board) const {
    for (u8 i = 0; i < 16; ++i) {
        const u8 base = (B >> (i * 4)) & 0xFu;
        const u8 cell = (board >> (i * 4)) & 0xFu;
        if (base == 0) {
            if (cell > G) { return false; }
        } else if (base == G) {
            if (cell != G + 1) { return false; }
        } else {
            if (cell <= G) { return false; }
        }
    }
    return true;
}

bool is_goal_state(const u64 state) const {
    for (const Dir dir: DIRS) {
        const u64 afterstate = moved_board(state, dir);
        if (afterstate == state) { continue; }
        if (is_goal(afterstate)) { return true; }
    }
    return false;
}

r_t eval_goal_state(const u64 state) const {
    //++cnt;
    //return expectimax_limited_states(state, 500, 0.05, tuples_4_stage_2).eval;
    //return 1 + add_weights(state, tuples_4_stage_2) / r_t(1e8);
    //return add_weights(state, tuples_4_stage_2);
    return 1;
}*/

/*r_t eval_state(const u64 state) {
    const u64 hash = to_hash(state);
    if (hash == 0) { return 0; }
    if (evals[hash] == UNKNOWN) { evals[hash] = eval_goal_state(state); }
    if (!isnan(evals[hash])) { return evals[hash]; }
    r_t max_eval = 0;
    for (const Dir dir: DIRS) {
        const u64 afterstate = moved_board(state, dir);
        if (afterstate == state) { continue; }
        r_t reward = r_t(get_reward(state, dir));
        reward = 0;
        const r_t eval = reward + eval_afterstate(afterstate);
        max_eval = max(max_eval, eval);
    }
    evals[hash] = max_eval;
    return max_eval;
}

r_t eval_afterstate(const u64 afterstate) {
    if (to_hash(afterstate) == 0) { return 0; }
    const u64 empty = empty_mask(afterstate);
    const u8 empty_count = popcnt(empty);
    u64 mask = 1;
    r_t sum = 0;
    for (u8 i = 0; i < empty_count; mask <<= 4) {
        if (empty & mask) {
            for (const auto &[shift, prob]: SHIFTS) {
                sum += prob * eval_state(afterstate | (mask << shift));
            }
            ++i;
        }
    }
    return sum / r_t(empty_count);
}*/

/*public:
    explicit Endgame(const u64 base) : B(base), G(get_G(B)) {
        auto start = time_now();
        cout << "Goal: " << u32(G) << endl;
        cout << "Space: " << u32(count_empty(B)) << endl;
        evals.resize(get_size(B), numeric_limits<r_t>::quiet_NaN());
        cout << "Size: " << evals.size() << endl;
        cout << "GBs: " << r_t(evals.size()) * sizeof(r_t) / 1e9 << endl;
    }

    void init_goal_states() {
        auto start = time_now();
        u64 goal_states = 0;
        for (u64 hash = 0; hash < evals.size(); ++hash) {
            const u64 board = from_hash(hash);
            if (is_goal_state(board)) {
                ++goal_states;
                evals[hash] = UNKNOWN;
            }
        }
        cout << "Goal states: " << goal_states << endl;
        cout << "Init finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void bruteforce_states() {
        auto start = time_now();
        const u64 board = from_hash(0);
        const u64 empty = empty_mask(board);
        const u8 empty_count = popcnt(empty);
        u64 mask = 1;
        for (u8 i = 0; i < empty_count; mask <<= 4) {
            if (empty & mask) {
                for (const auto &[shift, prob]: SHIFTS) {
                    eval_state(board | (mask << shift));
                }
                ++i;
            }
        }
        cout << "Bruteforce finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void save_packed_evals(const string &id) {
        const string filename = "evals-" + id + ".bin";
        auto start = time_now();
        ofstream file("../evals_backups/" + filename, ios::binary);
        if (!file.is_open()) {
            cout << "Error opening: " << filename << endl;
            return;
        }
        size_t size = evals.size();
        file.write((const char *) &B, sizeof(B));
        file.write((const char *) &size, sizeof(size));
        file.write((const char *) &evals[0], streamsize(size * sizeof(evals[0])));
        file.close();
        cout << "Saving " << filename << " finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void load_packed_evals(const string &id) {
        const string filename = "evals-" + id + ".bin";
        auto start = time_now();
        ifstream file("../evals_backups/" + filename, ios::binary);
        if (!file.is_open()) {
            cout << "Error opening: " << filename << endl;
            return;
        }
        u64 B_buffer;
        file.read((char *) &B_buffer, sizeof(B_buffer));
        assert(B_buffer == B);
        u64 size_buffer;
        file.read((char *) &size_buffer, sizeof(size_buffer));
        assert(size_buffer == evals.size());
        file.read((char *) &evals[0], streamsize(evals.size() * sizeof(evals[0])));
        file.close();
        cout << "Loading " << filename << " finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void print_evals() {
        r_t max_eval = 0;
        r_t min_eval = 0;
        u64 cnt_nan = 0;
        u64 cnt_inf = 0;
        for (u64 hash = 0; hash < evals.size(); ++hash) {
            if (isnan(evals[hash])) {
                ++cnt_nan;
                continue;
            }
            if (evals[hash] == -numeric_limits<r_t>::infinity()) {
                ++cnt_inf;
                continue;
            }
            if (max_eval == 0 || evals[hash] > max_eval) { max_eval = evals[hash]; }
            if (min_eval == 0 || evals[hash] < min_eval) { min_eval = evals[hash]; }
        }
        cout << "Max eval: " << max_eval << endl;
        cout << "Min eval: " << min_eval << endl;
        cout << "NaN evals: " << cnt_nan << endl;
        cout << "Inf evals: " << cnt_inf << endl;
    }

    r_t get_state_eval(const u64 board) {
        for (const auto &b: get_transformations(board)) {
            if (to_hash(b) != 0) {
                return eval_state(b);
            }
        }
        return 0;
    }

    r_t get_afterstate_eval(const u64 board) {
        for (const auto &b: get_transformations(board)) {
            if (to_hash(b) != 0) {
                return eval_afterstate(b);
            }
        }
        return 0;
    }

    Eval eval_board(const u64 board) {
        Eval best = Eval::None();
        for (const Dir dir: DIRS) {
            u64 afterstate = moved_board(board, dir);
            if (afterstate == board) { continue; }
            r_t eval = eval_afterstate(afterstate);
            if (eval > best.eval) {
                best = {dir, eval, get_reward(board, dir), afterstate};
            }
        }
        return best;
    }

    bool is_goal_state_2(const u64 state) const {
        return is_goal_state(state);
    }

    Dir best_dir(u64 board) {
        Dir best_dir = None;
        r_t best_eval = 0;
        for (const Dir dir: DIRS) {
            u64 afterstate = moved_board(board, dir);
            if (afterstate == board) { continue; }
            r_t eval = eval_afterstate(afterstate);
            if (eval > best_eval) {
                best_dir = dir;
                best_eval = eval;
            }
        }
        return best_dir;
    }

    void play_game() {
        u64 board = B;
        fill_board(board);
        print_board(board);
        while (!is_goal_state(board)) {
            Dir dir = best_dir(board);
            if (dir == None) { break; }
            board = moved_board(board, dir);
            fill_board(board);
            print_board(board);
        }
        cout << "done" << endl;
    }

    vector<u8> pack_dirs() {
        vector<u8> packed(size / 4 + 1, 0);
        for (u64 hash = 0; hash < size; ++hash) {
            u8 dir = best_dir(from_hash(hash));
            if (dir != 0) { --dir; }
            packed[hash / 4] |= dir << ((hash % 4) * 2);
        }
        return packed;
    }

    Dir unpack_dir(const vector<u8> &packed, const u64 hash) {
        u8 byte = packed[hash / 4];
        u8 bits = (byte >> ((hash % 4) * 2)) & 0b11u;
        return Dir(bits + 1);
    }
};*/

/*class Endgame {
private:
    const u64 B;
    const u8 G;
    const u64 size;
    vector<r_t> evals;

    static constexpr r_t UNKNOWN = -numeric_limits<r_t>::infinity();

    static u8 get_G(const u64 B) {
        u8 G = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 cell = (B >> (i * 4)) & 0xFu;
            if (cell == 0 || cell == 0xFu) { continue; }
            if (G != 0) { return 0; }
            G = cell;
        }
        return G;
    }

    static u64 get_size(const u64 B) {
        return power(get_G(B) + 1, count_empty(B));
    }

    u64 to_hash(const u64 board) const {
        u64 hash = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (board >> (i * 4)) & 0xFu;
            if (base == 0) {
                if (cell > G) { return 0; }
                hash = hash * (G + 1) + cell;
            } else if (base == G) {
                if (cell != G) { return 0; }
            } else {
                if (cell <= G) { return 0; }
            }
        }
        return hash;
    }

    u64 from_hash(u64 hash) const {
        u64 board = 0;
        array<u8, 16> cells{};
        u8 index = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            if (base == 0) {
                cells[index++] = hash % (G + 1);
                hash /= G + 1;
            }
        }
        u8 large = G + 1;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            u8 cell;
            if (base == 0) {
                cell = cells[--index];
            } else if (base == G) {
                cell = base;
            } else {
                cell = large++;
            }
            board |= u64(cell) << (i * 4);
        }
        return board;
    }

    u8 get_largest_small(const u64 board) const {
        u8 largest = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (board >> (i * 4)) & 0xFu;
            if (base == 0) {
                largest = max(largest, cell);
            }
        }
        return largest;
    }

    bool is_goal(const u64 board) const {
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (board >> (i * 4)) & 0xFu;
            if (base == 0) {
                if (cell > G) { return false; }
            } else if (base == G) {
                if (cell != G + 1) { return false; }
            } else {
                if (cell <= G) { return false; }
            }
        }
        return true;
    }

    bool is_goal_state(const u64 state) const {
        for (const Dir dir: DIRS) {
            const u64 afterstate = moved_board(state, dir);
            if (afterstate == state) { continue; }
            if (is_goal(afterstate)) { return true; }
        }
        return false;
    }

    r_t eval_goal_state(const u64 state) const {
        //++cnt;
        //return expectimax_limited_states(state, 500, 0.05, tuples_4_stage_2).eval;
        //return 1 + add_weights(state, tuples_4_stage_2) / r_t(1e8);
        //return add_weights(state, tuples_4_stage_2);
        return 1;
    }

    r_t eval_state(const u64 state) {
        const u64 hash = to_hash(state);
        if (hash == 0) { return 0; }
        if (evals[hash] == UNKNOWN) { evals[hash] = eval_goal_state(state); }
        if (!isnan(evals[hash])) { return evals[hash]; }
        r_t max_eval = 0;
        for (const Dir dir: DIRS) {
            const u64 afterstate = moved_board(state, dir);
            if (afterstate == state) { continue; }
            r_t reward = r_t(get_reward(state, dir));
            //TODO remove
            reward = 0;
            const r_t eval = reward + eval_afterstate(afterstate);
            max_eval = max(max_eval, eval);
        }
        evals[hash] = max_eval;
        return max_eval;
    }

    r_t eval_afterstate(const u64 afterstate) {
        if (to_hash(afterstate) == 0) { return 0; }
        const u64 empty = empty_mask(afterstate);
        const u8 empty_count = popcnt(empty);
        u64 mask = 1;
        r_t sum = 0;
        for (u8 i = 0; i < empty_count; mask <<= 4) {
            if (empty & mask) {
                for (const auto &[shift, prob]: SHIFTS) {
                    sum += prob * eval_state(afterstate | (mask << shift));
                }
                ++i;
            }
        }
        return sum / r_t(empty_count);
    }

public:
    explicit Endgame(const u64 base) : B(base), G(get_G(B)), size(get_size(B)) {
        auto start = time_now();
        cout << "Goal: " << u32(G) << endl;
        cout << "Space: " << u32(count_empty(B)) << endl;
        cout << "Size: " << size << endl;
        evals.resize(size, numeric_limits<r_t>::quiet_NaN());
        cout << "GBs: " << r_t(size) * sizeof(r_t) / 1e9 << endl;
    }

    void init_goal_states() {
        auto start = time_now();
        u64 goal_states = 0;
        for (u64 hash = 0; hash < size; ++hash) {
            const u64 board = from_hash(hash);
            if (is_goal_state(board)) {
                ++goal_states;
                evals[hash] = UNKNOWN;
            }
        }
        cout << "Goal states: " << goal_states << endl;
        cout << "Init finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void bruteforce_states() {
        auto start = time_now();
        const u64 board = from_hash(0);
        const u64 empty = empty_mask(board);
        const u8 empty_count = popcnt(empty);
        u64 mask = 1;
        for (u8 i = 0; i < empty_count; mask <<= 4) {
            if (empty & mask) {
                for (const auto &[shift, prob]: SHIFTS) {
                    eval_state(board | (mask << shift));
                }
                ++i;
            }
        }
        cout << "Bruteforce finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void save_packed_evals(const string &id) {
        const string filename = "evals-" + id + ".bin";
        auto start = time_now();
        ofstream file("../evals_backups/" + filename, ios::binary);
        if (!file.is_open()) {
            cout << "Error opening: " << filename << endl;
            return;
        }
        file.write((const char *) &B, sizeof(B));
        file.write((const char *) &size, sizeof(size));
        file.write((const char *) &evals[0], streamsize(size * sizeof(evals[0])));
        file.close();
        cout << "Saving " << filename << " finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void load_packed_evals(const string &id) {
        const string filename = "evals-" + id + ".bin";
        auto start = time_now();
        ifstream file("../evals_backups/" + filename, ios::binary);
        if (!file.is_open()) {
            cout << "Error opening: " << filename << endl;
            return;
        }
        u64 B_buffer;
        file.read((char *) &B_buffer, sizeof(B_buffer));
        assert(B_buffer == B);
        u64 size_buffer;
        file.read((char *) &size_buffer, sizeof(size_buffer));
        assert(size_buffer == size);
        file.read((char *) &evals[0], streamsize(size * sizeof(evals[0])));
        file.close();
        cout << "Loading " << filename << " finished: " << time_since(start) / 1e6 << " s" << endl;
    }

    void print_evals() {
        r_t max_eval = 0;
        r_t min_eval = 0;
        u64 cnt_nan = 0;
        u64 cnt_inf = 0;
        for (u64 hash = 0; hash < size; ++hash) {
            if (isnan(evals[hash])) {
                ++cnt_nan;
                continue;
            }
            if (evals[hash] == -numeric_limits<r_t>::infinity()) {
                ++cnt_inf;
                continue;
            }
            if (max_eval == 0 || evals[hash] > max_eval) { max_eval = evals[hash]; }
            if (min_eval == 0 || evals[hash] < min_eval) { min_eval = evals[hash]; }
        }
        cout << "Max eval: " << max_eval << endl;
        cout << "Min eval: " << min_eval << endl;
        cout << "NaN evals: " << cnt_nan << endl;
        cout << "Inf evals: " << cnt_inf << endl;
    }

    r_t get_state_eval(const u64 board) {
        for (const auto &b: get_transformations(board)) {
            if (to_hash(b) != 0) {
                return eval_state(b);
            }
        }
        return 0;
    }

    r_t get_afterstate_eval(const u64 board) {
        for (const auto &b: get_transformations(board)) {
            if (to_hash(b) != 0) {
                return eval_afterstate(b);
            }
        }
        return 0;
    }

    Eval eval_board(const u64 board) {
        Eval best = Eval::None();
        for (const Dir dir: DIRS) {
            u64 afterstate = moved_board(board, dir);
            if (afterstate == board) { continue; }
            r_t eval = eval_afterstate(afterstate);
            if (eval > best.eval) {
                best = {dir, eval, get_reward(board, dir), afterstate};
            }
        }
        return best;
    }

    bool is_goal_state_2(const u64 state) const {
        return is_goal_state(state);
    }

    Dir best_dir(u64 board) {
        Dir best_dir = None;
        r_t best_eval = 0;
        for (const Dir dir: DIRS) {
            u64 afterstate = moved_board(board, dir);
            if (afterstate == board) { continue; }
            r_t eval = eval_afterstate(afterstate);
            if (eval > best_eval) {
                best_dir = dir;
                best_eval = eval;
            }
        }
        return best_dir;
    }

    void play_game() {
        u64 board = B;
        fill_board(board);
        print_board(board);
        while (!is_goal_state(board)) {
            Dir dir = best_dir(board);
            if (dir == None) { break; }
            board = moved_board(board, dir);
            fill_board(board);
            print_board(board);
        }
        cout << "done" << endl;
    }

    vector<u8> pack_dirs() {
        vector<u8> packed(size / 4 + 1, 0);
        for (u64 hash = 0; hash < size; ++hash) {
            u8 dir = best_dir(from_hash(hash));
            if (dir != 0) { --dir; }
            packed[hash / 4] |= dir << ((hash % 4) * 2);
        }
        return packed;
    }

    Dir unpack_dir(const vector<u8> &packed, const u64 hash) {
        u8 byte = packed[hash / 4];
        u8 bits = (byte >> ((hash % 4) * 2)) & 0b11u;
        return Dir(bits + 1);
    }
};*/

/*u64 filter_large_tiles(const u64 board, u8 threshold) {
    u64 filtered = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = (board >> (i * 4)) & 0xFu;
        if (cell >= threshold) {
            filtered |= u64(cell) << (i * 4);
        }
    }
    return filtered;
}

u64 get_filtered(u64 board, u8 threshold) {
    u64 filtered = 0;
    for (u64 b: get_transformations(board)) {
        filtered = max(filtered, filter_large_tiles(b, threshold));
    }
    return filtered;
}

u16 get_large_mask(u64 board, u8 threshold) {
    u16 mask = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = (board >> (i * 4)) & 0xFu;
        if (cell >= threshold) {
            mask |= u16(1) << cell;
        }
    }
    return mask;
}

unordered_map<u16, unordered_map<u64, u32>> occurrences;

void count_occurrences(u8 threshold, u32 games) {
    //now in 10 threads
    vector<thread> all_threads;
    for (u8 t = 0; t < 10; ++t) {
        u32 game_cnt = games / 10;
        all_threads.emplace_back([game_cnt, threshold]() {
            init_rng();
            for (u32 t = 0; t < game_cnt; ++t) {
                u64 board = 0;
                fill_board(board);
                fill_board(board);
                while (true) {
                    //Dir dir= expectimax_limited_states(board, 100).dir;
                    Dir dir = eval_state(board, tuples_4_stage_1).dir;
                    if (dir == None) { break; }
                    move_board(board, dir);
                    fill_board(board);

                    u64 filtered = get_filtered(board, threshold);
                    u16 mask = get_large_mask(filtered, threshold);
                    if (mask == 0) { continue; }
                    if (popcnt(mask) < 4) { continue; }
                    if (occurrences[mask].count(filtered) == 0) {
                        occurrences[mask][filtered] = 1;
                    } else {
                        ++occurrences[mask][filtered];
                    }
                }
            }
        });
    }
    for (auto &thread: all_threads) {
        thread.join();
    }
}

vector<pair<u16, u64>> print_mask_probs() {
    unordered_map<u16, u64> mask_counts;
    for (const auto &[mask, boards]: occurrences) {
        u64 count = 0;
        for (const auto &[board, occu]: boards) {
            count += occu;
        }
        mask_counts[mask] = count;
    }
    vector<pair<u16, u64>> masks;
    u64 total_count = 0;
    for (const auto &[mask, count]: mask_counts) {
        masks.emplace_back(mask, count);
        total_count += count;
    }
    sort(masks.begin(), masks.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
    for (const auto &[mask, prob]: masks) {
        for (u8 i = 0; i < 16; ++i) {
            if (mask & (u16(1) << i)) {
                cout << "1";
            } else {
                cout << "0";
            }
        }
        cout << " prob: " << r_t(prob) / total_count << endl;
    }
    return masks;
}

void print_board_probs(u16 mask) {
    vector<pair<u64, u32>> boards;
    u64 total_count = 0;
    for (const auto &[board, occu]: occurrences[mask]) {
        boards.emplace_back(board, occu);
        total_count += occu;
    }
    sort(boards.begin(), boards.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
    for (const auto &[board, occu]: boards) {
        cout << "prob: " << r_t(occu) / total_count << endl;
        cout << hex << board << endl;
        print_board(board);
    }
}*/


/*
TO BUILD	1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16
SPACE	1	8e-9	1e-8	2e-8	2e-8	2e-8	3e-8	3e-8	4e-8	4e-8	4e-8	5e-8	5e-8	6e-8	6e-8	6e-8	7e-8
		2	2e-8	4e-8	6e-8	1e-7	1e-7	2e-7	3e-7	3e-7	4e-7	5e-7	6e-7	7e-7	8e-7	9e-7	1e-6	1e-6
		3	3e-8	1e-7	3e-7	5e-7	9e-7	1e-6	2e-6	3e-6	4e-6	5e-6	7e-6	9e-6	1e-5	1e-5	2e-5	2e-5
		4	6e-8	3e-7	1e-6	3e-6	5e-6	1e-5	2e-5	3e-5	4e-5	6e-5	8e-5	1e-4	2e-4	2e-4	3e-4	3e-4
		5	1e-7	1e-6	4e-6	1e-5	3e-5	7e-5	1e-4	2e-4	4e-4	6e-4	1e-3	1e-3	2e-3	3e-3	4e-3	6e-3
		6	3e-7	3e-6	2e-5	6e-5	2e-4	5e-4	1e-3	2e-3	4e-3	7e-3	1e-2	2e-2	3e-2	5e-2	7e-2	1e-1
		7	5e-7	9e-6	7e-5	3e-4	1e-3	3e-3	8e-3	2e-2	4e-2	8e-2	1e-1	3e-1	4e-1	7e-1	1e+0	2e+0
		8	1e-6	3e-5	3e-4	2e-3	7e-3	2e-2	7e-2	2e-1	4e-1	9e-1	2e+0	3e+0	6e+0	1e+1	2e+1	3e+1
		9	2e-6	8e-5	1e-3	8e-3	4e-2	2e-1	5e-1	2e+0	4e+0	9e+0	2e+1	4e+1	8e+1	2e+2	3e+2	5e+2
		10	4e-6	2e-4	4e-3	4e-2	2e-1	1e+0	4e+0	1e+1	4e+1	1e+2	2e+2	6e+2	1e+3	2e+3	4e+3	8e+3
		11	8e-6	7e-4	2e-2	2e-1	1e+0	8e+0	3e+1	1e+2	4e+2	1e+3	3e+3	7e+3	2e+4	3e+4	7e+4	1e+5
		12	2e-5	2e-3	7e-2	1e+0	9e+0	6e+1	3e+2	1e+3	4e+3	1e+4	4e+4	9e+4	2e+5	5e+5	1e+6	2e+6
		13	3e-5	6e-3	3e-1	5e+0	5e+1	4e+2	2e+3	1e+4	4e+4	1e+5	4e+5	1e+6	3e+6	8e+6	2e+7	4e+7
		14	7e-5	2e-2	1e+0	2e+1	3e+2	3e+3	2e+4	9e+4	4e+5	2e+6	5e+6	2e+7	4e+7	1e+8	3e+8	7e+8
		15	1e-4	6e-2	4e+0	1e+2	2e+3	2e+4	1e+5	8e+5	4e+6	2e+7	6e+7	2e+8	6e+8	2e+9	5e+9	1e+10
		16	3e-4	2e-1	2e+1	6e+2	1e+4	1e+5	1e+6	7e+6	4e+7	2e+8	7e+8	3e+9	9e+9	3e+10	7e+10	2e+11
*/

/*
TO BUILD	1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16
SPACE	1	5e-10	8e-10	1e-9	1e-9	2e-9	2e-9	2e-9	2e-9	3e-9	3e-9	3e-9	3e-9	4e-9	4e-9	4e-9	4e-9
		2	1e-9	2e-9	4e-9	6e-9	9e-9	1e-8	2e-8	2e-8	3e-8	3e-8	4e-8	4e-8	5e-8	6e-8	6e-8	7e-8
		3	2e-9	7e-9	2e-8	3e-8	5e-8	9e-8	1e-7	2e-7	3e-7	3e-7	4e-7	5e-7	7e-7	8e-7	1e-6	1e-6
		4	4e-9	2e-8	6e-8	2e-7	3e-7	6e-7	1e-6	2e-6	3e-6	4e-6	5e-6	7e-6	1e-5	1e-5	2e-5	2e-5
		5	8e-9	6e-8	3e-7	8e-7	2e-6	4e-6	8e-6	1e-5	3e-5	4e-5	6e-5	9e-5	1e-4	2e-4	3e-4	4e-4
		6	2e-8	2e-7	1e-6	4e-6	1e-5	3e-5	7e-5	1e-4	3e-4	4e-4	7e-4	1e-3	2e-3	3e-3	4e-3	6e-3
		7	3e-8	5e-7	4e-6	2e-5	7e-5	2e-4	5e-4	1e-3	3e-3	5e-3	9e-3	2e-2	3e-2	4e-2	7e-2	1e-1
		8	6e-8	2e-6	2e-5	1e-4	4e-4	1e-3	4e-3	1e-2	3e-2	5e-2	1e-1	2e-1	4e-1	6e-1	1e+0	2e+0
		9	1e-7	5e-6	7e-5	5e-4	3e-3	1e-2	3e-2	1e-1	3e-1	6e-1	1e+0	3e+0	5e+0	1e+1	2e+1	3e+1
		10	3e-7	1e-5	3e-4	2e-3	2e-2	7e-2	3e-1	9e-1	3e+0	6e+0	2e+1	3e+1	7e+1	1e+2	3e+2	5e+2
		11	5e-7	4e-5	1e-3	1e-2	9e-2	5e-1	2e+0	8e+0	3e+1	7e+1	2e+2	4e+2	1e+3	2e+3	4e+3	9e+3
		12	1e-6	1e-4	4e-3	6e-2	5e-1	3e+0	2e+1	7e+1	3e+2	8e+2	2e+3	6e+3	1e+4	3e+4	7e+4	1e+5
		13	2e-6	4e-4	2e-2	3e-1	3e+0	2e+1	1e+2	6e+2	3e+3	9e+3	3e+4	8e+4	2e+5	5e+5	1e+6	2e+6
		14	4e-6	1e-3	7e-2	2e+0	2e+1	2e+2	1e+3	6e+3	3e+4	9e+4	3e+5	1e+6	3e+6	7e+6	2e+7	4e+7
		15	8e-6	4e-3	3e-1	8e+0	1e+2	1e+3	9e+3	5e+4	3e+5	1e+6	4e+6	1e+7	4e+7	1e+8	3e+8	7e+8
		16	2e-5	1e-2	1e+0	4e+1	7e+2	8e+3	7e+4	5e+5	3e+6	1e+7	5e+7	2e+8	5e+8	2e+9	5e+9	1e+10
*/