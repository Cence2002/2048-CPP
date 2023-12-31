#pragma once

#include "../assets.h"

u64 from_array(const array<array<u8, 3>, 3> &cells) {
    u64 board = 0;
    for (u8 y = 0; y < 3; y++) {
        for (u8 x = 0; x < 3; x++) {
            board |= u64(cells[y][x]) << (y * 12 + x * 4);
        }
    }
    return board;
}

array<array<u8, 3>, 3> to_array(const u64 board) {
    array<array<u8, 3>, 3> cells{};
    for (u8 y = 0; y < 3; y++) {
        for (u8 x = 0; x < 3; x++) {
            cells[y][x] = (board >> (y * 12 + x * 4)) & 0xFu;
        }
    }
    return cells;
}

template<u8 y>
inline u16 get_row(const u64 board) {
    return (board >> (y * 12)) & 0xFFFu;
}

template<u8 x>
inline u16 get_col(const u64 board) {
    return pext(board, 0x00F00F00Full << (x * 4));
}

inline u64 empty_mask(u64 board) {
    board |= board >> 2;
    board |= board >> 1;
    return ~board & 0x111111111ull;
}

inline u8 count_empty(const u64 board) {
    return popcnt(empty_mask(board));
}

inline void fill_board(u64 &board) {
    u64 empty = empty_mask(board);
    u8 pos = random(count_empty(board));
    u64 mask = 1ull;
    while (!(empty & mask) || pos-- != 0) { mask <<= 4; }
    board |= mask << !random(10);
}

u16 rewards[1 << 12];

inline u16 get_reward(const u64 board, const Dir d) {
    if (d & 1) {
        return rewards[get_row<0>(board)] + rewards[get_row<1>(board)] + rewards[get_row<2>(board)];
    } else {
        return rewards[get_col<0>(board)] + rewards[get_col<1>(board)] + rewards[get_col<2>(board)];
    }
}

u64 left_0[1 << 12];
u64 left_1[1 << 12];
u64 left_2[1 << 12];
u64 up_0[1 << 12];
u64 up_1[1 << 12];
u64 up_2[1 << 12];
u64 right_0[1 << 12];
u64 right_1[1 << 12];
u64 right_2[1 << 12];
u64 down_0[1 << 12];
u64 down_1[1 << 12];
u64 down_2[1 << 12];

inline void move_board(u64 &board, const Dir d) {
    switch (d) {
        case Left:
            board ^= left_0[get_row<0>(board)] ^ left_1[get_row<1>(board)] ^ left_2[get_row<2>(board)];
            break;
        case Up:
            board ^= up_0[get_col<0>(board)] ^ up_1[get_col<1>(board)] ^ up_2[get_col<2>(board)];
            break;
        case Right:
            board ^= right_0[get_row<0>(board)] ^ right_1[get_row<1>(board)] ^ right_2[get_row<2>(board)];
            break;
        case Down:
            board ^= down_0[get_col<0>(board)] ^ down_1[get_col<1>(board)] ^ down_2[get_col<2>(board)];
            break;
        default:
            break;
    }
}

inline u64 moved_board(const u64 board, const Dir d) {
    u64 res = board;
    move_board(res, d);
    return res;
}

void init_tables() {
    for (u16 line = 0; line < (1 << 12); line++) {
        rewards[line] = 0;

        left_0[line] = 0;
        left_1[line] = 0;
        left_2[line] = 0;
        up_0[line] = 0;
        up_1[line] = 0;
        up_2[line] = 0;
        right_0[line] = 0;
        right_1[line] = 0;
        right_2[line] = 0;
        down_0[line] = 0;
        down_1[line] = 0;
        down_2[line] = 0;
    }
    for (u16 line = 0; line < (1 << 12); line++) {
        u8 cells[3];
        u16 rev_line = 0;
        for (u8 i = 0; i < 3; i++) {
            u16 cell = (line >> (i * 4)) & 0xFu;
            cells[i] = cell;
            rev_line |= cell << ((2 - i) * 4);
        }
        u8 to = 0;
        bool merged = false;
        for (u8 from = 0; from < 3; from++) {
            if (cells[from] == 0) { continue; }
            if (to > 0 && cells[to - 1] == cells[from] && !merged) {
                cells[to - 1]++;
                cells[from] = 0;
                merged = true;
                rewards[line] += 1 << cells[to - 1];
            } else {
                if (to != from) {
                    cells[to] = cells[from];
                    cells[from] = 0;
                }
                to++;
                merged = false;
            }
        }
        u16 moved = 0;
        u16 rev_moved = 0;
        for (u8 i = 0; i < 3; i++) {
            u8 cell = cells[i];
            moved |= cell << (i * 4);
            rev_moved |= cell << ((2 - i) * 4);
        }
        left_0[line] = line ^ moved;
        up_0[line] = pdep(line ^ moved, 0x00F00F00Full);
        right_0[rev_line] = rev_line ^ rev_moved;
        down_0[rev_line] = pdep(rev_line ^ rev_moved, 0x00F00F00Full);
    }

    for (u16 line = 0; line < (1 << 12); line++) {
        left_1[line] = left_0[line] << 12;
        left_2[line] = left_1[line] << 12;
        up_1[line] = up_0[line] << 4;
        up_2[line] = up_1[line] << 4;
        right_1[line] = right_0[line] << 12;
        right_2[line] = right_1[line] << 12;
        down_1[line] = down_0[line] << 4;
        down_2[line] = down_1[line] << 4;
    }
}