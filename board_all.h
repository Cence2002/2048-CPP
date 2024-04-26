#pragma once

#include "assets.h"

struct line_base {
    line_base() = default;

    virtual void load_array(const array<u8, 4> &cells) = 0;

    virtual array<u8, 4> get_array() const = 0;

    virtual u8 get_item(u8 i) const = 0;

    virtual void set_item(u8 i, u8 cell) = 0;

    virtual void reverse() = 0;

    virtual u8 empty_count() const = 0;

    virtual void left() = 0;

    virtual void right() = 0;

    virtual s_t reward() const = 0;

    std::string to_string() const {
        std::string str;
        for (u8 i = 0; i < 4; i++) {
            str.push_back(cell_str[get_item(i)]);
            str += " ";
        }
        str += "\n";
        return str;
    }

    friend std::ostream &operator<<(std::ostream &os, const line_base &line) {
        os << line.to_string();
        return os;
    }
};

template<typename T>
concept line_concept = std::is_base_of_v<line_base, T> &&
                       std::is_default_constructible_v<T> &&
                       std::is_copy_constructible_v<T> &&
                       std::is_copy_assignable_v<T> &&
                       std::is_assignable_v<T, T> && requires() {
    { T::init_tables() } -> std::same_as<void>;
};

struct l_t_16 : public line_base {
private:
    u16 line;

public:
    l_t_16() : line(0) {}

    l_t_16(const u16 line) : line(line) {}

    l_t_16(const l_t_16 &line) : line(line.line) {}

    l_t_16 &operator=(const l_t_16 &other) {
        line = other.line;
        return *this;
    }

    u16 get_bits() const {
        return line;
    }

    void load_array(const array<u8, 4> &cells) override {
        line = 0;
        for (u8 i = 0; i < 4; i++) {
            line |= u16(cells[i] & 0xFu) << (i * 4);
        }
    }

    array<u8, 4> get_array() const override {
        array<u8, 4> cells{};
        for (u8 i = 0; i < 4; i++) {
            cells[i] = (line >> (i * 4)) & 0xFu;
        }
        return cells;
    }

    u8 get_item(const u8 i) const override {
        return (line >> (i * 4)) & 0xFu;
    }

    void set_item(const u8 i, const u8 cell) override {
        line &= ~(0xFu << (i * 4));
        line |= u16(cell & 0xFu) << (i * 4);
    }

    void reverse() override {
        for (u8 i = 0; i < 2; i++) {
            const u8 temp = get_item(i);
            set_item(i, get_item(3 - i));
            set_item(3 - i, temp);
        }
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 i = 0; i < 4; i++) {
            count += get_item(i) == 0;
        }
        return count;
    }

    void left() override {
        line = left_table[line];
    }

    void right() override {
        line = right_table[line];
    }

    s_t reward() const override {
        return reward_table[line];
    }

    static void init_tables() {
        for (u16 bits = E(16) - 1; bits > 0; --bits) {
            array<u8, 4> cells{};
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
            reward_table[bits] = reward;

            l_t_16 moved_line = 0;
            l_t_16 moved_reversed_line = 0;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = cells[i];
                moved_line.set_item(i, cell);
                moved_reversed_line.set_item(3 - i, cell);
            }
            left_table[line.get_bits()] = moved_line.get_bits();
            right_table[reversed_line.get_bits()] = moved_reversed_line.get_bits();
        }
    }

private:
    static u16 left_table[E(16)];
    static u16 right_table[E(16)];
    static s_t reward_table[E(16)];
};

struct l_t_32 : public line_base {
private:
    u32 line;
public:
    l_t_32() : line(0) {}
    l_t_32(const u32 line) : line(line) {}
    l_t_32(const l_t_32 &line) : line(line.line) {}

    l_t_32 &operator=(const l_t_32 &other) {
        line = other.line;
        return *this;
    }

    u32 get_bits() const {
        return line;
    }

    void load_array(const array<u8, 4> &cells) override {
        line = 0;
        for (u8 i = 0; i < 4; i++) {
            line |= u32(cells[i] & 0x1Fu) << (i * 5);
        }
    }

    array<u8, 4> get_array() const override {
        array<u8, 4> cells{};
        for (u8 i = 0; i < 4; i++) {
            cells[i] = (line >> (i * 5)) & 0x1Fu;
        }
        return cells;
    }

    u8 get_item(const u8 i) const override {
        return (line >> (i * 5)) & 0x1Fu;
    }

    void set_item(const u8 i, const u8 cell) override {
        line &= ~(0x1Fu << (i * 5));
        line |= u32(cell & 0x1Fu) << (i * 5);
    }

    void reverse() override {
        for (u8 i = 0; i < 2; i++) {
            const u8 temp = get_item(i);
            set_item(i, get_item(3 - i));
            set_item(3 - i, temp);
        }
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 i = 0; i < 4; i++) {
            count += get_item(i) == 0;
        }
        return count;
    }

    void left() override {
        line = left_table[line];
    }

    void right() override {
        line = right_table[line];
    }

    s_t reward() const override {
        return reward_table[line];
    }

    static void init_tables() {
        for (u32 bits = E(20) - 1; bits > 0; --bits) {
            array<u8, 4> cells{};
            l_t_32 line;
            l_t_32 reversed_line;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = (bits >> (i * 5)) & 0x1Fu;
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
                if (to > 0 && cells[from] == cells[to - 1] && can_merge) {
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

            l_t_32 moved_line = 0;
            l_t_32 moved_reversed_line = 0;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = cells[i];
                moved_line.set_item(i, cell);
                moved_reversed_line.set_item(3 - i, cell);
            }
            left_table[line.get_bits()] = moved_line.get_bits();
            right_table[reversed_line.get_bits()] = moved_reversed_line.get_bits();
        }
    }

private:
    static u32 left_table[E(20)];
    static u32 right_table[E(20)];
    static s_t reward_table[E(20)];
};

struct board_base {
    virtual void load_matrix(const array<array<u8, 4>, 4> &cells) = 0;

    virtual array<array<u8, 4>, 4> get_matrix() const = 0;


    virtual u8 get_cell(u8 x, u8 y) const = 0;

    virtual u8 get_cell(u8 i) const = 0;

    virtual void set_cell(u8 x, u8 y, u8 cell) = 0;

    virtual void set_cell(u8 i, u8 cell) = 0;


    virtual u8 empty_count() const = 0;

    virtual void spawn() = 0;

    virtual void slide(Dir dir) = 0;

    virtual s_t reward(Dir dir) const = 0;


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

struct b_t_mat : public board_base {
private:
    u8 board[4][4]{};

public:
    b_t_mat() : board{{0, 0, 0, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}} {}

    b_t_mat(const b_t_mat &board) : board{{board.board[0][0], board.board[0][1], board.board[0][2], board.board[0][3]},
                                          {board.board[1][0], board.board[1][1], board.board[1][2], board.board[1][3]},
                                          {board.board[2][0], board.board[2][1], board.board[2][2], board.board[2][3]},
                                          {board.board[3][0], board.board[3][1], board.board[3][2], board.board[3][3]}} {}

    b_t_mat &operator=(const b_t_mat &other) {
        if (this == &other) { return *this; }
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y][x] = other.board[y][x];
            }
        }
        return *this;
    }

    void load_matrix(const array<array<u8, 4>, 4> &cells) override {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y][x] = cells[y][x];
            }
        }
    }

    array<array<u8, 4>, 4> get_matrix() const override {
        array<array<u8, 4>, 4> cells{};
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
                    if (index-- == 0) {
                        const u8 cell = random(10) > 0 ? 1 : 2;
                        set_cell(x, y, cell);
                        return;
                    }
                }
            }
        }
    }

    s_t reward(const Dir dir) const override {
        switch (dir) {
            case Left:
            case Right: {
                s_t reward = 0;
                for (u8 y = 0; y < 4; y++) {
                    l_t_32 line;
                    for (u8 x = 0; x < 4; x++) {
                        line.set_item(x, get_cell(x, y));
                    }
                    reward += line.reward();
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
                    reward += line.reward();
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

    static void init_tables() {
        l_t_32::init_tables();
    }
};

template<line_concept line_type = l_t_32>
struct b_t_arr : public board_base {
private:
    line_type board[4];

public:
    b_t_arr() : board{0, 0, 0, 0} {}

    b_t_arr(const b_t_arr &board) : board{board.board[0], board.board[1], board.board[2], board.board[3]} {}

    b_t_arr &operator=(const b_t_arr &other) {
        if (this == &other) { return *this; }
        for (u8 y = 0; y < 4; y++) {
            board[y] = other.board[y];
        }
        return *this;
    }

    void load_matrix(const array<array<u8, 4>, 4> &cells) override {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board[y].set_item(x, cells[y][x]);
            }
        }
    }

    array<array<u8, 4>, 4> get_matrix() const override {
        array<array<u8, 4>, 4> cells{};
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
                    if (index-- == 0) {
                        set_cell(x, y, random(10) == 0 ? 2 : 1);
                        return;
                    }
                }
            }
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

    s_t reward(const Dir dir) const override {
        switch (dir) {
            case Left:
            case Right: {
                s_t reward = 0;
                for (u8 y = 0; y < 4; y++) {
                    reward += board[y].reward();
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
                    reward += line.reward();
                }
                return reward;
            }
            default:
                return 0;
        }
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

    b_t_64(const b_t_64 &board) : board(board.board) {}

    b_t_64 &operator=(const b_t_64 &other) {
        if (this == &other) { return *this; }
        board = other.board;
        return *this;
    }

    u64 get_bits() const {
        return board;
    }

    void load_matrix(const array<array<u8, 4>, 4> &cells) override {
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                set_cell(x, y, cells[y][x]);
            }
        }
    }

    array<array<u8, 4>, 4> get_matrix() const override {
        array<array<u8, 4>, 4> cells{};
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

    s_t reward(const Dir dir) const override {
        switch (dir) {
            case Left:
            case Right: {
                s_t reward = 0;
                for (u8 y = 0; y < 4; y++) {
                    l_t_16 line(u16(board >> (y * 16)));
                    reward += line.reward();
                }
                return reward;
            }
            case Up:
            case Down: {
                s_t reward = 0;
                for (u8 x = 0; x < 4; x++) {
                    l_t_16 line(pext(board, 0x000F000F000F000Full << (x * 4)));
                    reward += line.reward();
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

    static void init_tables() {
        l_t_16::init_tables();
    }
};

struct b_t_opt : public board_base {
private:
    u64 board{};

public:
    inline b_t_opt() : board(0) {}

    inline b_t_opt(u64 board) : board(board) {}

    inline b_t_opt(const b_t_opt &board) : board(board.board) {}

    inline b_t_opt &operator=(const b_t_opt &other) {
        if (this == &other) { return *this; }
        board = other.board;
        return *this;
    }

    inline u64 get_bits() const {
        return board;
    }

    inline void load_matrix(const array<array<u8, 4>, 4> &cells) override {
        board = 0;
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                board |= u64(cells[y][x] & 0xFu) << ((y << 4) | (x << 2));
            }
        }
    }

    inline array<array<u8, 4>, 4> get_matrix() const override {
        array<array<u8, 4>, 4> cells{};
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
        return ~(board
                 | (board >> 1)
                 | (board >> 2)
                 | (board >> 3)
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
        while (!(empty & place) || index-- != 0) {
            place <<= 4;
        }
        board |= place << (random(10) == 0);
    }

    inline s_t reward(const Dir dir) const override {
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

    inline array<b_t_opt, 8> get_transformations() const {
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
            array<u8, 4> cells{};
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

private:
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
    static s_t reward_table[E(16)];
};

u32 l_t_32::left_table[E(20)] = {};
u32 l_t_32::right_table[E(20)] = {};
s_t l_t_32::reward_table[E(20)] = {};


s_t l_t_16::reward_table[E(16)] = {};
u16 l_t_16::left_table[E(16)] = {};
u16 l_t_16::right_table[E(16)] = {};


u64 b_t_opt::left_table_0[E(16)] = {};
u64 b_t_opt::left_table_1[E(16)] = {};
u64 b_t_opt::left_table_2[E(16)] = {};
u64 b_t_opt::left_table_3[E(16)] = {};

u64 b_t_opt::right_table_0[E(16)] = {};
u64 b_t_opt::right_table_1[E(16)] = {};
u64 b_t_opt::right_table_2[E(16)] = {};
u64 b_t_opt::right_table_3[E(16)] = {};

u64 b_t_opt::up_table_0[E(16)] = {};
u64 b_t_opt::up_table_1[E(16)] = {};
u64 b_t_opt::up_table_2[E(16)] = {};
u64 b_t_opt::up_table_3[E(16)] = {};

u64 b_t_opt::down_table_0[E(16)] = {};
u64 b_t_opt::down_table_1[E(16)] = {};
u64 b_t_opt::down_table_2[E(16)] = {};
u64 b_t_opt::down_table_3[E(16)] = {};

s_t b_t_opt::reward_table[E(16)] = {};

using l_t = l_t_16;
using b_t = b_t_opt;