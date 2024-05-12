#pragma once

#include "assets.h"
#include "lines.h"

struct board_base {
    virtual void load_matrix(const std::array<std::array<u8, 4>, 4> &cells) = 0;

    virtual std::array<std::array<u8, 4>, 4> get_matrix() const = 0;


    virtual u8 get_cell(u8 x, u8 y) const = 0;

    virtual u8 get_cell(u8 i) const = 0;

    virtual void set_cell(u8 x, u8 y, u8 cell) = 0;

    virtual void set_cell(u8 i, u8 cell) = 0;

    inline u8 get_max_cell() const {
        u8 max_cell = 0;
        for (u8 i = 0; i < 16; i++) {
            max_cell = std::max(max_cell, get_cell(i));
        }
        return max_cell;
    }


    virtual u8 empty_count() const = 0;

    virtual void spawn() = 0;

    virtual s_t get_reward(Dir dir) const = 0;

    virtual void slide(Dir dir) = 0;


    std::string to_string() const {
        std::string str;
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                const u8 cell = get_cell(x, y);
                str.push_back(cell_str[cell]);
                str += " ";
            }
            str += "\n";
        }
        str += "\n";
        return str;
    }

    friend std::ostream &operator<<(std::ostream &os, const board_base &board) {
        os << board.to_string();
        return os;
    }
};

template<typename T>
concept board_concept = std::is_base_of_v<board_base, T> &&
                        std::is_default_constructible_v<T> &&
                        std::is_copy_constructible_v<T> &&
                        std::is_copy_assignable_v<T>
                        && requires() {{ T::init_tables() } -> std::same_as<void>; }
                        && requires(T board, Dir dir) {{ board.slid(dir) } -> std::same_as<T>; }
                        && requires(T board, T other) {
    { board == other } -> std::same_as<bool>;
    { board != other } -> std::same_as<bool>;
};

struct b_t_sim : public board_base {
private:
    std::array<std::array<u8, 4>, 4> board;

public:
    b_t_sim() : board{{{0, 0, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}}} {}

    b_t_sim(const b_t_sim &other) : board{{{other.board[0][0], other.board[0][1], other.board[0][2], other.board[0][3]},
                                           {other.board[1][0], other.board[1][1], other.board[1][2], other.board[1][3]},
                                           {other.board[2][0], other.board[2][1], other.board[2][2], other.board[2][3]},
                                           {other.board[3][0], other.board[3][1], other.board[3][2], other.board[3][3]}}} {}

    b_t_sim &operator=(const b_t_sim &other) {
        if (this == &other) { return *this; }
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y][x] = other.board[y][x];
            }
        }
        return *this;
    }

    bool operator==(const b_t_sim &other) const {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                if (board[y][x] != other.board[y][x]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const b_t_sim &other) const {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                if (board[y][x] != other.board[y][x]) {
                    return true;
                }
            }
        }
        return false;
    }

    void load_matrix(const std::array<std::array<u8, 4>, 4> &cells) override {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y][x] = cells[y][x];
            }
        }
    }

    std::array<std::array<u8, 4>, 4> get_matrix() const override {
        std::array<std::array<u8, 4>, 4> cells{};
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                cells[y][x] = board[y][x];
            }
        }
        return cells;
    }

    u8 get_cell(const u8 x, const u8 y) const override {
        return board[y][x];
    }

    u8 get_cell(const u8 i) const override {
        return board[i / 4][i % 4];
    }

    void set_cell(const u8 x, const u8 y, const u8 cell) override {
        board[y][x] = cell;
    }

    void set_cell(const u8 i, const u8 cell) override {
        board[i / 4][i % 4] = cell;
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                count += get_cell(x, y) == 0;
            }
        }
        return count;
    }

    void spawn() override {
        int empties = empty_count();
        if (empties == 0) [[unlikely]] { return; }
        int target_index = random(empties);
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (get_cell(x, y) != 0) {
                    continue;
                }
                if (target_index == 0) {
                    if (random(10) == 0) [[unlikely]] {
                        set_cell(x, y, 2);
                    } else [[likely]] {
                        set_cell(x, y, 1);
                    }
                    return;
                }
                target_index--;
            }
        }
    }

    void spawn_2() {
        int empties = empty_count();
        if (empties == 0) { return; }
        int target_index = random(empties);
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (get_cell(x, y) != 0) { continue; }
                if (target_index == 0) {
                    set_cell(x, y, random(10) == 0 ? 2 : 1);
                    return;
                }
                target_index--;
            }
        }
    }

    s_t get_reward(const Dir dir) const override {
        switch (dir) {
            case Left:
            case Right: {
                s_t reward = 0;
                for (u8 y = 0; y < 4; y++) {
                    u8 prev = 0;
                    for (u8 x = 0; x < 4; x++) {
                        const u8 cell = get_cell(x, y);
                        if (cell == 0) { continue; }
                        if (prev == cell) {
                            prev = 0;
                            reward += E(cell + 1);
                        } else {
                            prev = cell;
                        }
                    }
                }
                return reward;
            }
            case Up:
            case Down: {
                s_t reward = 0;
                for (u8 x = 0; x < 4; x++) {
                    u8 prev = 0;
                    for (u8 y = 0; y < 4; y++) {
                        const u8 cell = get_cell(x, y);
                        if (cell == 0) { continue; }
                        if (prev == cell) {
                            prev = 0;
                            reward += E(cell + 1);
                        } else {
                            prev = cell;
                        }
                    }
                }
                return reward;
            }
            default:
                return 0;
        }
    }

    void left() {
        for (u8 y = 0; y < 4; y++) {
            u8 to_x = 0;
            bool can_merge = true;
            for (u8 from_x = 0; from_x < 4; ++from_x) {
                if (get_cell(from_x, y) == 0) { continue; }
                if (to_x > 0
                    && get_cell(from_x, y) == get_cell(to_x - 1, y)
                    && can_merge) [[unlikely]] {
                    ++board[y][to_x - 1];
                    board[y][from_x] = 0;
                    can_merge = false;
                } else [[likely]] {
                    if (to_x != from_x) {
                        board[y][to_x] = board[y][from_x];
                        board[y][from_x] = 0;
                    }
                    ++to_x;
                    can_merge = true;
                }
            }
        }
    }

    void right() {
        for (u8 y = 0; y < 4; y++) {
            u8 to_x = 3;
            bool can_merge = true;
            for (u8 from_x = 3; from_x < 4; --from_x) {
                if (get_cell(from_x, y) == 0) {
                    continue;
                }
                if (to_x < 3
                    && get_cell(from_x, y) == get_cell(to_x + 1, y)
                    && can_merge) [[unlikely]] {
                    ++board[y][to_x + 1];
                    board[y][from_x] = 0;
                    can_merge = false;
                } else [[likely]] {
                    if (to_x != from_x) {
                        board[y][to_x] = board[y][from_x];
                        board[y][from_x] = 0;
                    }
                    --to_x;
                    can_merge = true;
                }
            }
        }
    }

    void up() {
        for (u8 x = 0; x < 4; x++) {
            u8 to_y = 0;
            bool can_merge = true;
            for (u8 from_y = 0; from_y < 4; ++from_y) {
                if (get_cell(x, from_y) == 0) {
                    continue;
                }
                if (to_y > 0
                    && get_cell(x, from_y) == get_cell(x, to_y - 1)
                    && can_merge) [[unlikely]] {
                    ++board[to_y - 1][x];
                    board[from_y][x] = 0;
                    can_merge = false;
                } else [[likely]] {
                    if (to_y != from_y) {
                        board[to_y][x] = board[from_y][x];
                        board[from_y][x] = 0;
                    }
                    ++to_y;
                    can_merge = true;
                }
            }
        }
    }

    void down() {
        for (u8 x = 0; x < 4; x++) {
            u8 to_y = 3;
            bool can_merge = true;
            for (u8 from_y = 3; from_y < 4; --from_y) {
                if (get_cell(x, from_y) == 0) {
                    continue;
                }
                if (to_y < 3
                    && get_cell(x, from_y) == get_cell(x, to_y + 1)
                    && can_merge) [[unlikely]] {
                    ++board[to_y + 1][x];
                    board[from_y][x] = 0;
                    can_merge = false;
                } else [[likely]] {
                    if (to_y != from_y) {
                        board[to_y][x] = board[from_y][x];
                        board[from_y][x] = 0;
                    }
                    --to_y;
                    can_merge = true;
                }
            }
        }
    }

    void slide(const Dir dir) override {
        switch (dir) {
            case Left:
                left();
                break;
            case Right:
                right();
                break;
            case Up:
                up();
                break;
            case Down:
                down();
                break;
            default:
                break;
        }
    }

    b_t_sim slid(const Dir dir) const {
        b_t_sim new_board = *this;
        new_board.slide(dir);
        return new_board;
    }

    static void init_tables() {}
};

struct b_t_mat : public board_base {
private:
    u8 board[4][4]{};

public:
    b_t_mat() : board{{0, 0, 0, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}} {}

    b_t_mat(const b_t_mat &other) : board{{other.board[0][0], other.board[0][1], other.board[0][2], other.board[0][3]},
                                          {other.board[1][0], other.board[1][1], other.board[1][2], other.board[1][3]},
                                          {other.board[2][0], other.board[2][1], other.board[2][2], other.board[2][3]},
                                          {other.board[3][0], other.board[3][1], other.board[3][2], other.board[3][3]}} {}

    b_t_mat &operator=(const b_t_mat &other) {
        if (this == &other) { return *this; }
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y][x] = other.board[y][x];
            }
        }
        return *this;
    }

    bool operator==(const b_t_mat &other) const {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                if (board[y][x] != other.board[y][x]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const b_t_mat &other) const {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                if (board[y][x] != other.board[y][x]) {
                    return true;
                }
            }
        }
        return false;
    }

    void load_matrix(const std::array<std::array<u8, 4>, 4> &cells) override {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y][x] = cells[y][x];
            }
        }
    }

    std::array<std::array<u8, 4>, 4> get_matrix() const override {
        std::array<std::array<u8, 4>, 4> cells{};
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                cells[y][x] = board[y][x];
            }
        }
        return cells;
    }

    u8 get_cell(const u8 x, const u8 y) const override {
        return board[y][x];
    }

    u8 get_cell(const u8 i) const override {
        return board[i / 4][i % 4];
    }

    void set_cell(const u8 x, const u8 y, const u8 cell) override {
        board[y][x] = cell;
    }

    void set_cell(const u8 i, const u8 cell) override {
        board[i / 4][i % 4] = cell;
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                count += get_cell(x, y) == 0;
            }
        }
        return count;
    }

    void spawn() override {
        const u8 empty = empty_count();
        if (empty == 0) { return; }
        u8 index = random(empty);
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                if (get_cell(x, y) == 0) {
                    if (index == 0) {
                        const u8 cell = random(10) > 0 ? 1 : 2;
                        set_cell(x, y, cell);
                        return;
                    } else {
                        index--;
                    }
                }
            }
        }
    }

    s_t get_reward(const Dir dir) const override {
        switch (dir) {
            case Left:
            case Right: {
                s_t reward = 0;
                for (u8 y = 0; y < 4; y++) {
                    l_t_32 line;
                    for (u8 x = 0; x < 4; x++) {
                        line.set_item(x, get_cell(x, y));
                    }
                    reward += line.get_reward();
                }
                return reward;
            }
            case Up:
            case Down: {
                s_t reward = 0;
                for (u8 x = 0; x < 4; x++) {
                    l_t_32 line;
                    for (u8 y = 0; y < 4; y++) {
                        line.set_item(y, get_cell(x, y));
                    }
                    reward += line.get_reward();
                }
                return reward;
            }
            default:
                return 0;
        }
    }

    void left() {
        for (u8 y = 0; y < 4; y++) {
            l_t_32 line;
            for (u8 x = 0; x < 4; x++) {
                line.set_item(x, get_cell(x, y));
            }
            line.left();
            for (u8 x = 0; x < 4; x++) {
                set_cell(x, y, line.get_item(x));
            }
        }
    }

    void right() {
        for (u8 y = 0; y < 4; y++) {
            l_t_32 line;
            for (u8 x = 0; x < 4; x++) {
                line.set_item(x, get_cell(x, y));
            }
            line.right();
            for (u8 x = 0; x < 4; x++) {
                set_cell(x, y, line.get_item(x));
            }
        }
    }

    void up() {
        for (u8 x = 0; x < 4; x++) {
            l_t_32 line;
            for (u8 y = 0; y < 4; y++) {
                line.set_item(y, get_cell(x, y));
            }
            line.left();
            for (u8 y = 0; y < 4; y++) {
                set_cell(x, y, line.get_item(y));
            }
        }
    }

    void down() {
        for (u8 x = 0; x < 4; x++) {
            l_t_32 line;
            for (u8 y = 0; y < 4; y++) {
                line.set_item(y, get_cell(x, y));
            }
            line.right();
            for (u8 y = 0; y < 4; y++) {
                set_cell(x, y, line.get_item(y));
            }
        }
    }

    void slide(const Dir dir) override {
        switch (dir) {
            case Left:
                left();
                break;
            case Right:
                right();
                break;
            case Up:
                up();
                break;
            case Down:
                down();
                break;
            default:
                break;
        }
    }

    b_t_mat slid(const Dir dir) const {
        b_t_mat new_board = *this;
        new_board.slide(dir);
        return new_board;
    }

    static void init_tables() {
        l_t_32::init_tables();
    }
};

template<line_concept line_type>
struct b_t_arr : public board_base {
private:
    line_type board[4];

public:
    b_t_arr() : board{0, 0, 0, 0} {}

    b_t_arr(const b_t_arr<line_type> &other) : board{other.board[0], other.board[1], other.board[2], other.board[3]} {}

    b_t_arr<line_type> &operator=(const b_t_arr<line_type> &other) {
        if (this == &other) { return *this; }
        for (u8 y = 0; y < 4; y++) {
            board[y] = other.board[y];
        }
        return *this;
    }

    bool operator==(const b_t_arr<line_type> &other) const {
        for (u8 y = 0; y < 4; y++) {
            if (board[y] != other.board[y]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const b_t_arr<line_type> &other) const {
        for (u8 y = 0; y < 4; y++) {
            if (board[y] != other.board[y]) {
                return true;
            }
        }
        return false;
    }

    void load_matrix(const std::array<std::array<u8, 4>, 4> &cells) override {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y].set_item(x, cells[y][x]);
            }
        }
    }

    std::array<std::array<u8, 4>, 4> get_matrix() const override {
        std::array<std::array<u8, 4>, 4> cells{};
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                cells[y][x] = board[y].get_item(x);
            }
        }
        return cells;
    }

    u8 get_cell(const u8 x, const u8 y) const override {
        return board[y].get_item(x);
    }

    u8 get_cell(const u8 i) const override {
        return board[i / 4].get_item(i % 4);
    }

    void set_cell(const u8 x, const u8 y, const u8 cell) override {
        board[y].set_item(x, cell);
    }

    void set_cell(const u8 i, const u8 cell) override {
        board[i / 4].set_item(i % 4, cell);
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 y = 0; y < 4; y++) {
            count += board[y].empty_count();
        }
        return count;
    }

    void spawn() override {
        u8 empty = empty_count();
        if (empty == 0) { return; }
        u8 index = random(empty);
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                if (get_cell(x, y) == 0) {
                    if (index == 0) {
                        set_cell(x, y, random(10) == 0 ? 2 : 1);
                        return;
                    } else {
                        index--;
                    }
                }
            }
        }
    }

    s_t get_reward(const Dir dir) const override {
        switch (dir) {
            case Left:
            case Right: {
                s_t reward = 0;
                for (u8 y = 0; y < 4; y++) {
                    reward += board[y].get_reward();
                }
                return reward;
            }
            case Down:
            case Up: {
                s_t reward = 0;
                for (u8 x = 0; x < 4; x++) {
                    line_type line;
                    for (u8 y = 0; y < 4; y++) {
                        line.set_item(y, get_cell(x, y));
                    }
                    reward += line.get_reward();
                }
                return reward;
            }
            default:
                return 0;
        }
    }

    void left() {
        for (u8 y = 0; y < 4; y++) {
            board[y].left();
        }
    }

    void right() {
        for (u8 y = 0; y < 4; y++) {
            board[y].right();
        }
    }

    void up() {
        for (u8 x = 0; x < 4; x++) {
            line_type line;
            for (u8 y = 0; y < 4; y++) {
                line.set_item(y, get_cell(x, y));
            }
            line.left();
            for (u8 y = 0; y < 4; y++) {
                set_cell(x, y, line.get_item(y));
            }
        }
    }

    void down() {
        for (u8 x = 0; x < 4; x++) {
            line_type line;
            for (u8 y = 0; y < 4; y++) {
                line.set_item(y, get_cell(x, y));
            }
            line.right();
            for (u8 y = 0; y < 4; y++) {
                set_cell(x, y, line.get_item(y));
            }
        }
    }

    void slide(const Dir dir) override {
        switch (dir) {
            case Left:
                left();
                break;
            case Right:
                right();
                break;
            case Up:
                up();
                break;
            case Down:
                down();
                break;
            default:
                break;
        }
    }

    b_t_arr<line_type> slid(const Dir dir) const {
        b_t_arr<line_type> temp = *this;
        temp.slide(dir);
        return temp;
    }

    static void init_tables() {
        line_type::init_tables();
    }
};

struct b_t_64 : public board_base {
private:
    u64 board{};

public:
    b_t_64() : board(0) {}

    b_t_64(u64 board) : board(board) {}

    b_t_64(const b_t_64 &other) : board(other.board) {}

    b_t_64 &operator=(const b_t_64 &other) {
        if (this == &other) { return *this; }
        board = other.board;
        return *this;
    }

    bool operator==(const b_t_64 &other) const {
        return board == other.board;
    }

    bool operator!=(const b_t_64 &other) const {
        return board != other.board;
    }

    u64 get_bits() const {
        return board;
    }

    void load_matrix(const std::array<std::array<u8, 4>, 4> &cells) override {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                set_cell(x, y, cells[y][x]);
            }
        }
    }

    std::array<std::array<u8, 4>, 4> get_matrix() const override {
        std::array<std::array<u8, 4>, 4> cells{};
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                cells[y][x] = get_cell(x, y);
            }
        }
        return cells;
    }

    u8 get_cell(const u8 x, const u8 y) const override {
        return (board >> ((y * 4 + x) * 4)) & 0xFu;
    }

    u8 get_cell(const u8 i) const override {
        return (board >> (i * 4)) & 0xFu;
    }

    void set_cell(const u8 x, const u8 y, const u8 cell) override {
        board &= ~(0xFull << ((y * 4 + x) * 4));
        board |= u64(cell & 0xFu) << ((y * 4 + x) * 4);
    }

    void set_cell(const u8 i, const u8 cell) override {
        board &= ~(0xFull << (i * 4));
        board |= u64(cell & 0xFu) << (i * 4);
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 i = 0; i < 16; i++) {
            count += get_cell(i) == 0;
        }
        return count;
    }

    void spawn() override {
        u8 empty = empty_count();
        if (empty == 0) { return; }
        u8 index = random(empty);
        for (u8 i = 0; i < 16; i++) {
            if (get_cell(i) == 0) {
                if (index-- == 0) {
                    const u8 cell = random(10) > 0 ? 1 : 2;
                    set_cell(i, cell);
                    return;
                }
            }
        }
    }

    s_t get_reward(const Dir dir) const override {
        switch (dir) {
            case Left:
            case Right: {
                s_t reward = 0;
                for (u8 y = 0; y < 4; y++) {
                    l_t_16 line(u16(board >> (y * 16)));
                    reward += line.get_reward();
                }
                return reward;
            }
            case Up:
            case Down: {
                s_t reward = 0;
                for (u8 x = 0; x < 4; x++) {
                    l_t_16 line(pext(board, 0x000F000F000F000Full << (x * 4)));
                    reward += line.get_reward();
                }
                return reward;
            }
            default:
                return 0;
        }
    }

    void left() {
        u64 new_board = 0;
        for (u8 y = 0; y < 4; y++) {
            l_t_16 line(u16(board >> (y * 16)));
            line.left();
            new_board |= u64(line.get_bits()) << (y * 16);
        }
        board = new_board;
    }

    void right() {
        u64 new_board = 0;
        for (u8 y = 0; y < 4; y++) {
            l_t_16 line(u16(board >> (y * 16)));
            line.right();
            new_board |= u64(line.get_bits()) << (y * 16);
        }
        board = new_board;
    }

    void up() {
        u64 new_board = 0;
        for (u8 x = 0; x < 4; x++) {
            l_t_16 line(pext(board, 0x000F000F000F000Full << (x * 4)));
            line.left();
            new_board |= pdep(u64(line.get_bits()), 0x000F000F000F000Full << (x * 4));
        }
        board = new_board;
    }

    void down() {
        u64 new_board = 0;
        for (u8 x = 0; x < 4; x++) {
            l_t_16 line(pext(board, 0x000F000F000F000Full << (x * 4)));
            line.right();
            new_board |= pdep(u64(line.get_bits()), 0x000F000F000F000Full << (x * 4));
        }
        board = new_board;
    }

    void slide(const Dir dir) override {
        switch (dir) {
            case Left:
                left();
                break;
            case Right:
                right();
                break;
            case Up:
                up();
                break;
            case Down:
                down();
                break;
            default:
                break;
        }
    }

    b_t_64 slid(const Dir dir) const {
        b_t_64 temp = *this;
        temp.slide(dir);
        return temp;
    }

    static void init_tables() {
        l_t_16::init_tables();
    }
};

struct b_t_opt : public board_base {
private:
    u64 board{};

    static s_t reward_table[E(16)];
    static u64 left_table_0[E(16)];
    static u64 left_table_1[E(16)];
    static u64 left_table_2[E(16)];
    static u64 left_table_3[E(16)];
    static u64 right_table_0[E(16)];
    static u64 right_table_1[E(16)];
    static u64 right_table_2[E(16)];
    static u64 right_table_3[E(16)];
    static u64 up_table_0[E(16)];
    static u64 up_table_1[E(16)];
    static u64 up_table_2[E(16)];
    static u64 up_table_3[E(16)];
    static u64 down_table_0[E(16)];
    static u64 down_table_1[E(16)];
    static u64 down_table_2[E(16)];
    static u64 down_table_3[E(16)];

public:
    inline b_t_opt() : board(0) {}

    inline b_t_opt(u64 board) : board(board) {}

    inline b_t_opt(const b_t_opt &other) : board(other.board) {}

    inline b_t_opt &operator=(const b_t_opt &other) {
        if (this == &other) { return *this; }
        board = other.board;
        return *this;
    }

    inline bool operator==(const b_t_opt &other) const {
        return board == other.board;
    }

    inline bool operator!=(const b_t_opt &other) const {
        return board != other.board;
    }

    inline b_t_opt operator|(const u64 &mask) const {
        return board | mask;
    }

    inline u64 get_bits() const {
        return board;
    }

    inline void load_matrix(const std::array<std::array<u8, 4>, 4> &cells) override {
        board = 0;
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board |= u64(cells[y][x] & 0xFu) << ((y << 4) | (x << 2));
            }
        }
    }

    inline std::array<std::array<u8, 4>, 4> get_matrix() const override {
        std::array<std::array<u8, 4>, 4> cells{};
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                cells[y][x] = get_cell(x, y);
            }
        }
        return cells;
    }

    inline u8 get_cell(const u8 x, const u8 y) const override {
        return (board >> ((y << 4) | (x << 2))) & 0xFu;
    }

    inline u8 get_cell(const u8 i) const override {
        return (board >> (i << 2)) & 0xFu;
    }

    inline void set_cell(const u8 x, const u8 y, const u8 cell) override {
        board &= ~(0xFull << ((y << 4) | (x << 2)));
        board |= u64(cell & 0xFu) << ((y << 4) | (x << 2));
    }

    inline void set_cell(const u8 i, const u8 cell) override {
        board &= ~(0xFull << (i << 2));
        board |= u64(cell & 0xFu) << (i << 2);
    }

    template<u8 y>
    inline u16 get_row() const {
        return (board >> (y << 4)) & 0xFFFFu;
    }

    template<u8 x>
    inline u16 get_col() const {
        return pext(board, 0x000F000F000F000Full << (x << 2));
    }

    inline u64 empty_mask() const {
        return ~(board |
                 (board >> 1) |
                 (board >> 2) |
                 (board >> 3)
        ) & 0x1111111111111111ull;
    }

    inline u8 empty_count() const override {
        return popcnt(empty_mask());
    }

    static inline u8 empty_count(const u64 empty_mask) {
        return popcnt(empty_mask);
    }

    inline void spawn() override {
        const u64 empty = empty_mask();
        u8 index = random(empty_count());
        u64 place = 0x1ull;
        while (!(empty & place) || index-- != 0) { place <<= 4; }
        board |= place << (__builtin_expect(random(10) == 0, true) ? 1 : 0);
    }

    inline s_t get_reward(const Dir dir) const override {
        if (dir & 1) {
            return reward_table[get_row<0>()] +
                   reward_table[get_row<1>()] +
                   reward_table[get_row<2>()] +
                   reward_table[get_row<3>()];
        } else {
            return reward_table[get_col<0>()] +
                   reward_table[get_col<1>()] +
                   reward_table[get_col<2>()] +
                   reward_table[get_col<3>()];
        }
    }

    inline void slide(const Dir dir) override {
        switch (dir) {
            case Left:
                board = left_table_0[get_row<0>()] |
                        left_table_1[get_row<1>()] |
                        left_table_2[get_row<2>()] |
                        left_table_3[get_row<3>()];
                break;
            case Right:
                board = right_table_0[get_row<0>()] |
                        right_table_1[get_row<1>()] |
                        right_table_2[get_row<2>()] |
                        right_table_3[get_row<3>()];
                break;
            case Up:
                board = up_table_0[get_col<0>()] |
                        up_table_1[get_col<1>()] |
                        up_table_2[get_col<2>()] |
                        up_table_3[get_col<3>()];
                break;
            case Down:
                board = down_table_0[get_col<0>()] |
                        down_table_1[get_col<1>()] |
                        down_table_2[get_col<2>()] |
                        down_table_3[get_col<3>()];
                break;
            default:
                break;
        }
    }

    inline b_t_opt slid(const Dir dir) const {
        b_t_opt temp(board);
        temp.slide(dir);
        return temp;
    }

    inline u64 reversed_rows() const {
        return ((board & 0x000F000F000F000Full) << 12) |
               ((board & 0x00F000F000F000F0ull) << 04) |
               ((board & 0x0F000F000F000F00ull) >> 04) |
               ((board & 0xF000F000F000F000ull) >> 12);
    }

    inline u64 reversed_cols() const {
        return ((board & 0x000000000000FFFFull) << 48) |
               ((board & 0x00000000FFFF0000ull) << 16) |
               ((board & 0x0000FFFF00000000ull) >> 16) |
               ((board & 0xFFFF000000000000ull) >> 48);
    }

    inline u64 transposed() const {
        return (pext(board, 0x000F000F000F000Full) << 00) |
               (pext(board, 0x00F000F000F000F0ull) << 16) |
               (pext(board, 0x0F000F000F000F00ull) << 32) |
               (pext(board, 0xF000F000F000F000ull) << 48);
    }

    inline std::array<b_t_opt, 8> get_transformations() const {
        const b_t_opt transposed(this->transposed());
        const b_t_opt reversed_rows(this->reversed_rows());
        const b_t_opt reversed_rows_transposed(transposed.reversed_rows());
        return {
                *this, reversed_rows,
                transposed, reversed_rows_transposed,
                reversed_cols(), reversed_rows.reversed_cols(),
                transposed.reversed_cols(), reversed_rows_transposed.reversed_cols()
        };
    }

    static void init_tables() {
        for (u16 bits = E(16) - 1; bits > 0; --bits) {
            std::array<u8, 4> cells{};
            l_t_16 line;
            l_t_16 reversed_line;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = (bits >> (i * 4)) & 0xFu;
                cells[i] = cell;
                line.set_item(i, cell);
                reversed_line.set_item(3 - i, cell);
            }
            s_t reward = 0;
            u8 to = 0;
            bool can_merge = true;
            for (u8 from = 0; from < 4; ++from) {
                if (cells[from] == 0) {
                    continue;
                }
                if (to > 0 && cells[from] == cells[to - 1] && can_merge
                    && cells[from] != 0xFu) {
                    ++cells[to - 1];
                    cells[from] = 0;
                    reward += E(cells[to - 1]);
                    can_merge = false;
                } else {
                    if (to != from) {
                        cells[to] = cells[from];
                        cells[from] = 0;
                    }
                    ++to;
                    can_merge = true;
                }
            }
            reward_table[line.get_bits()] = reward;

            l_t_16 moved_line = 0;
            l_t_16 moved_reversed_line = 0;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = cells[i];
                moved_line.set_item(i, cell);
                moved_reversed_line.set_item(3 - i, cell);
            }
            left_table_0[line.get_bits()] = moved_line.get_bits();
            right_table_0[reversed_line.get_bits()] = moved_reversed_line.get_bits();
            up_table_0[line.get_bits()] = pdep(moved_line.get_bits(), 0x000F000F000F000Full);
            down_table_0[reversed_line.get_bits()] = pdep(moved_reversed_line.get_bits(), 0x000F000F000F000Full);
        }

        for (u16 bits = E(16) - 1; bits > 0; --bits) {
            left_table_1[bits] = left_table_0[bits] << 16;
            left_table_2[bits] = left_table_1[bits] << 16;
            left_table_3[bits] = left_table_2[bits] << 16;

            right_table_1[bits] = right_table_0[bits] << 16;
            right_table_2[bits] = right_table_1[bits] << 16;
            right_table_3[bits] = right_table_2[bits] << 16;

            up_table_1[bits] = up_table_0[bits] << 4;
            up_table_2[bits] = up_table_1[bits] << 4;
            up_table_3[bits] = up_table_2[bits] << 4;

            down_table_1[bits] = down_table_0[bits] << 4;
            down_table_2[bits] = down_table_1[bits] << 4;
            down_table_3[bits] = down_table_2[bits] << 4;
        }
    }
};

s_t b_t_opt::reward_table[E(16)];
u64 b_t_opt::left_table_0[E(16)];
u64 b_t_opt::left_table_1[E(16)];
u64 b_t_opt::left_table_2[E(16)];
u64 b_t_opt::left_table_3[E(16)];
u64 b_t_opt::right_table_0[E(16)];
u64 b_t_opt::right_table_1[E(16)];
u64 b_t_opt::right_table_2[E(16)];
u64 b_t_opt::right_table_3[E(16)];
u64 b_t_opt::up_table_0[E(16)];
u64 b_t_opt::up_table_1[E(16)];
u64 b_t_opt::up_table_2[E(16)];
u64 b_t_opt::up_table_3[E(16)];
u64 b_t_opt::down_table_0[E(16)];
u64 b_t_opt::down_table_1[E(16)];
u64 b_t_opt::down_table_2[E(16)];
u64 b_t_opt::down_table_3[E(16)];

using l_t = l_t_16;
using b_t = b_t_opt;

struct Eval {
    Dir dir;
    r_t eval;
    s_t reward;
    b_t afterstate;

    Eval() : dir(Dir::None), eval(0), reward(0), afterstate() {}

    Eval(Dir dir, r_t eval, s_t reward, const b_t afterstate) : dir(dir), eval(eval), reward(reward), afterstate(afterstate) {}
};

struct Game_stat {
    b_t board;
    s_t score;
    u32 moves;
};