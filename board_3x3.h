#pragma once

#include "board_all.h"

struct b_t_3x3 {
private:
    u64 board{};

    static s_t reward_table[E(16)];
    static u64 left_table[E(16)];
    static u64 right_table[E(16)];
    static u64 up_table[E(16)];
    static u64 down_table[E(16)];

public:
    b_t_3x3() : board(0) {}

    b_t_3x3(const u64 board) : board(board) {}

    b_t_3x3(const b_t_3x3 &board) : board(board.board) {}

    b_t_3x3 &operator=(const b_t_3x3 &board) {
        if (this == &board) { return *this; }
        this->board = board.board;
        return *this;
    }

    bool operator==(const b_t_3x3 &other) const {
        return board == other.board;
    }

    bool operator!=(const b_t_3x3 &other) const {
        return board != other.board;
    }

    b_t_3x3 operator|(const b_t_3x3 &other) const {
        return b_t_3x3(board | other.board);
    }

    u64 get_bits() const {
        return board;
    }

    void load_matrix(const std::array<std::array<u8, 3>, 3> &cells) {
        board = 0;
        for (u8 y = 0; y < 3; ++y) {
            for (u8 x = 0; x < 3; ++x) {
                board |= u64(cells[y][x]) << ((y * 3 + x) << 2);
            }
        }
    }

    std::array<std::array<u8, 3>, 3> get_matrix() const {
        std::array<std::array<u8, 3>, 3> cells{};
        for (u8 y = 0; y < 3; ++y) {
            for (u8 x = 0; x < 3; ++x) {
                cells[y][x] = get_cell(x, y);
            }
        }
        return cells;
    }

    u8 get_cell(const u8 x, const u8 y) const {
        return (board >> ((y * 3 + x) << 2)) & 0xFu;
    }

    u8 get_cell(const u8 index) const {
        return (board >> (index << 2)) & 0xF;
    }

    void set_cell(const u8 x, const u8 y, const u8 value) {
        board &= ~(0xFu << ((y * 3 + x) << 2));
        board |= u64(value) << ((y * 3 + x) << 2);
    }

    void set_cell(const u8 index, const u8 value) {
        board &= ~(0xFu << (index << 2));
        board |= u64(value) << (index << 2);
    }

    template<u8 y>
    u64 get_row() const {
        return (board >> (y * 12)) & 0xFFF;
    }

    template<u8 x>
    u64 get_col() const {
        return pext(board, 0x00F00F00Full << (x << 2));
    }

    u64 empty_mask() const {
        return ~(board | (board >> 1) | (board >> 2)) & 0x111111111ull;
    }

    u8 empty_count() const {
        return popcnt(empty_mask());
    }

    u8 empty_count(const u64 empty_mask) const {
        return popcnt(empty_mask);
    }

    void spawn() {
        const u64 empty = empty_mask();
        u8 index = random(empty_count());
        u64 place = 0x1ull;
        while (!(empty & place) || index-- != 0) {
            place <<= 4;
        }
        board |= place << (random(10) == 0);
    }

    s_t get_reward(const Dir dir) const {
        if (dir & 1) {
            return reward_table[get_row<0>()] +
                   reward_table[get_row<1>()] +
                   reward_table[get_row<2>()];
        } else {
            return reward_table[get_col<0>()] +
                   reward_table[get_col<1>()] +
                   reward_table[get_col<2>()];
        }
    }

    void slide(const Dir dir) {
        switch (dir) {
            case Left:
                board = left_table[get_row<0>()] << 0 |
                        left_table[get_row<1>()] << 12 |
                        left_table[get_row<2>()] << 24;
                break;
            case Right:
                board = right_table[get_row<0>()] << 0 |
                        right_table[get_row<1>()] << 12 |
                        right_table[get_row<2>()] << 24;
                break;
            case Up:
                board = up_table[get_col<0>()] << 0 |
                        up_table[get_col<1>()] << 4 |
                        up_table[get_col<2>()] << 8;
                break;
            case Down:
                board = down_table[get_col<0>()] << 0 |
                        down_table[get_col<1>()] << 4 |
                        down_table[get_col<2>()] << 8;
                break;
            default:
                break;
        }
    }

    b_t_3x3 slid(const Dir dir) const {
        b_t_3x3 temp(board);
        temp.slide(dir);
        return temp;
    }

    static void init_tables() {
        for (u16 bits = E(12) - 1; bits > 0; --bits) {
            std::array<u8, 3> cells{};
            u16 line, reversed_line;
            for (u8 i = 0; i < 3; ++i) {
                const u8 cell = (bits >> (i << 2)) & 0xFu;
                cells[i] = cell;
                line |= u64(cell) << (i << 2);
                reversed_line |= u64(cell) << ((2 - i) << 2);
            }
            s_t reward = 0;
            u8 to = 0;
            bool can_merge = true;
            for (u8 from = 0; from < 3; ++from) {
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
            reward_table[line] = reward;

            u16 moved_line = 0;
            u16 moved_reversed_line = 0;
            for (u8 i = 0; i < 3; ++i) {
                const u8 cell = cells[i];
                moved_line |= u64(cell) << (i << 2);
                moved_reversed_line |= u64(cell) << ((2 - i) << 2);
            }
            left_table[line] = moved_line;
            right_table[reversed_line] = moved_reversed_line;
            up_table[line] = pdep(moved_line, 0x00F00F00Full);
            down_table[reversed_line] = pdep(moved_reversed_line, 0x00F00F00Full);
        }
    }
};

s_t b_t_3x3::reward_table[E(16)];
u64 b_t_3x3::left_table[E(16)];
u64 b_t_3x3::right_table[E(16)];
u64 b_t_3x3::up_table[E(16)];
u64 b_t_3x3::down_table[E(16)];
