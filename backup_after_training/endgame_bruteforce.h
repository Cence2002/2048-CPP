#pragma once

#include "board.h"
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

class Endgame {
private:
    const u64 B;
    const u8 G;
    const u64 size;
    vector<r_t> evals;

    static u8 get_G(const u64 B) {
        u8 G = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 cell = (B >> (i * 4)) & 0xFu;
            if (cell == 0xFu) { continue; }
            G = max(G, cell);
        }
        return G;
    }

    static u64 get_size(const u64 B) {
        return power(get_G(B) + 1, count_empty<4>(B));
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
                if (cell != 0xFu) { return 0; }
            }
        }
        return hash;
    }

    u64 general_to_hash(const u64 board) const {
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
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            u8 cell;
            if (base == 0) {
                cell = cells[--index];
            } else {
                cell = base;
            }
            board |= u64(cell) << (i * 4);
        }
        return board;
    }

    u64 general_from_hash(u64 hash) const {
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
        u8 big = G + 1;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            u8 cell;
            if (base == 0) {
                cell = cells[--index];
            } else if (base == G) {
                cell = base;
            } else {
                cell = big++;
            }
            board |= u64(cell) << (i * 4);
        }
        return board;
    }

    bool is_goal(const u64 board) const {
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (board >> (i * 4)) & 0xFu;
            if (base == 0xFu && cell != 0xFu) { return false; }
            if (base == G && cell != G + 1) { return false; }
        }
        return true;
    }

    bool is_goal_state(const u64 state) const {
        for (const Dir dir: DIRS) {
            const u64 afterstate = moved_board<4>(state, dir);
            if (afterstate == state) { continue; }
            if (is_goal(afterstate)) { return true; }
        }
        return false;
    }

    r_t eval_state(const u64 state) {
        const u64 hash = to_hash(state);
        if (hash == 0) { return 0; }
        if (evals[hash] != -1) { return evals[hash]; }
        r_t max_eval = 0;
        for (const Dir dir: DIRS) {
            const u64 afterstate = moved_board<4>(state, dir);
            if (afterstate == state) { continue; }
            r_t reward = r_t(get_reward<4>(state, dir));
            //TODO remove
            reward *= 0;
            const r_t eval = reward + eval_afterstate(afterstate);
            max_eval = max(max_eval, eval);
        }
        evals[hash] = max_eval;
        return max_eval;
    }

    r_t eval_afterstate(const u64 afterstate) {
        if (to_hash(afterstate) == 0) { return 0; }
        u64 empty = empty_mask<4>(afterstate);
        const u8 count = popcnt(empty);
        u64 mask = 1;
        r_t sum = 0;
        for (u8 i = 0; i < count;) {
            if (empty & mask) {
                for (const auto &[shift, prob]: SHIFTS) {
                    sum += prob * eval_state(afterstate | (mask << shift));
                }
                ++i;
            }
            mask <<= 4;
        }
        return sum / r_t(count);
    }

public:
    explicit Endgame(const u64 base) : B(base), G(get_G(B)), size(get_size(B)) {
        cout << size << endl;
        evals.resize(size, -1);
    }

    void init_goals(u32 threads) {
        //TODO replace with n-tuple evaluation with not-too-large depth
        if (threads == 0) {
            for (u64 hash = 0; hash < size; ++hash) {
                const u64 board = from_hash(hash);
                if (is_goal_state(board)) {
                    //TODO replace
                    //const u64 general_board = general_from_hash(hash);
                    //const r_t eval = expectimax_limited_states<4>(general_board, 100).eval;
                    //evals[hash] = eval;
                    evals[hash] = 1;
                }
            }
        } else {
            vector<thread> all_threads;
            for (u32 t = 0; t < threads; ++t) {
                all_threads.emplace_back([this, t, threads]() {
                    for (u64 hash = t; hash < size; hash += threads) {
                        const u64 board = from_hash(hash);
                        if (is_goal_state(board)) {
                            //TODO replace
                            //const u64 general_board = general_from_hash(hash);
                            //const r_t eval = expectimax_limited_states<4>(general_board, 100).eval;
                            //evals[hash] = eval;
                            evals[hash] = 1;
                        }
                    }
                });
            }
            for (auto &thread: all_threads) {
                thread.join();
            }
        }
    }

    void bruteforce_states() {
        for (u64 hash = 0; hash < size; ++hash) {
            const u64 board = from_hash(hash);
            eval_state(board);
            eval_afterstate(board);
        }
    }

    //returns 8 if no transformation is found (board doesn't match B)
    u8 transform_index(const u64 board) {
        u8 index = 0;
        for (const auto &b: get_transformations<4>(board)) {
            const u64 hash = general_to_hash(b);
            if (hash != 0) {
                return index;
            }
            ++index;
        }
        return index;
    }

    r_t general_state_eval(const u64 board, const u8 index) {
        if (index == 8) { cout << "NOP" << endl; }
        const u64 b = get_transformations<4>(board)[index];
        const u64 hash = general_to_hash(b);
        if (hash == 0) { return 0; }
        return eval_state(b);
    }

    r_t general_afterstate_eval(const u64 board, const u8 index) {
        if (index == 8) { cout << "NOP" << endl; }
        const u64 b = get_transformations<4>(board)[index];
        const u64 hash = general_to_hash(b);
        if (hash == 0) { return 0; }
        return eval_afterstate(b);
    }

    /*Dir best_dir(u64 board) {
        Dir best_dir = None;
        r_t best_eval = 0;
        for (const Dir dir: DIRS) {
            u64 afterstate = moved_board<4>(board, dir);
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
        fill_board<4>(board);
        print_board<4>(board);
        while (!is_goal_state(board)) {
            Dir dir = best_dir(board);
            if (dir == None) { break; }
            board = moved_board<4>(board, dir);
            fill_board<4>(board);
            print_board<4>(board);
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
    }*/
};

//set all tiles smaller than threshold to 0
u64 filter_large_tiles(u64 board, u8 threshold) {
    u64 filtered = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = (board >> (i * 4)) & 0xFu;
        if (cell >= threshold) {
            filtered |= u64(cell) << (i * 4);
        }
    }
    return filtered;
}

//i-th bit 1 if there is a tile exactly 2^i (only for i larger than threshold)
u16 large_tiles_mask(u64 board, u8 threshold) {
    u16 mask = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = (board >> (i * 4)) & 0xFu;
        if (cell >= threshold) {
            mask |= u16(1) << i;
        }
    }
    return mask;
}

//how many times a filtered board occurred for a specific mask for a specific threshold
unordered_map<u16, unordered_map<u64, u32>> count_occurrences(u8 threshold, u32 games) {
    unordered_map<u16, unordered_map<u64, u32>> occurrences;
    for (u64 t = 0; t < games; ++t) {
        if (t % 1000 == 0) {
            cout << "Progress: " << t << endl;
        }
        u64 board = 0;
        fill_board<4>(board);
        fill_board<4>(board);
        for (u8 i = 0; i < threshold; ++i) {
            //Dir dir= expectimax_limited_states<4>(board, 100).dir;
            Dir dir = eval_state<4>(board).dir;
            if (dir == None) { break; }
            move_board<4>(board, dir);
            fill_board<4>(board);
            u64 filtered = 0;
            for (u64 b: get_transformations<4>(board)) {
                filtered = max(filtered, filter_large_tiles(b, threshold));
            }
            u16 mask = large_tiles_mask(filtered, threshold);
            if (mask == 0) { continue; }
            if (occurrences[mask].count(filtered) == 0) {
                occurrences[mask][filtered] = 1;
            } else {
                ++occurrences[mask][filtered];
            }
        }
    }
    return occurrences;
}

//first, count total number of occurrences for each mask
//then, sort masks by frequency
//then, print masks and frequencies
vector<pair<u16, u64>> print_mask_probs(const unordered_map<u16, unordered_map<u64, u32>> &occurrences) {
    unordered_map<u16, u64> mask_counts;
    for (const auto &[mask, boards]: occurrences) {
        u64 count = 0;
        for (const auto &[board, occurrences]: boards) {
            count += occurrences;
        }
        mask_counts[mask] = count;
    }
    vector<pair<u16, u64>> masks;
    for (const auto &[mask, prob]: mask_counts) {
        for (u8 i = 0; i < 16; ++i) {
            if (mask & (u16(1) << i)) {
                cout << "1";
            } else {
                cout << "0";
            }
        }
        cout << " " << prob << endl;
    }
    return masks;
}

//sort boards by frequency
void print_board_probs(unordered_map<u64, u32> &occurrences) {
    vector<pair<u64, u32>> boards;
    for (const auto &[board, occurrences]: occurrences) {
        boards.emplace_back(board, occurrences);
    }
    sort(boards.begin(), boards.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
    for (const auto &[board, occurrences]: boards) {
        print_board<4>(board);
        cout << " " << occurrences << endl;
    }
}


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