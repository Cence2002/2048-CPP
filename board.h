#pragma once

#include "assets.h"

constexpr u16 from_array(const array<u8, 4> &cells) {
    u16 line = 0;
    for (u8 i = 0; i < 4; i++) {
        line |= u16(cells[i]) << (i * 4);
    }
    return line;
}

constexpr array<u8, 4> to_array(const u16 line) {
    array<u8, 4> cells{};
    for (u8 i = 0; i < 4; i++) {
        cells[i] = (line >> (i * 4)) & 0xFu;
    }
    return cells;
}

constexpr u64 from_matrix(const array<array<u8, 4>, 4> &cells) {
    u64 board = 0;
    for (u8 y = 0; y < 4; y++) {
        for (u8 x = 0; x < 4; x++) {
            board |= u64(cells[y][x]) << (y * 16 + x * 4);
        }
    }
    return board;
}

constexpr array<array<u8, 4>, 4> to_matrix(const u64 board) {
    array<array<u8, 4>, 4> cells{};
    for (u8 y = 0; y < 4; y++) {
        for (u8 x = 0; x < 4; x++) {
            cells[y][x] = (board >> (y * 16 + x * 4)) & 0xFu;
        }
    }
    return cells;
}

constexpr inline u8 get_cell(const u64 board, const u8 i) {
    return (board >> (i * 4)) & 0xFu;
}

constexpr inline void set_cell(u16 &line, const u8 i, const u8 cell) {
    line &= ~(0xFu << (i * 4));
    line |= u16(cell) << (i * 4);
}

constexpr inline void set_cell(u64 &board, const u8 i, const u8 cell) {
    board &= ~(0xFull << (i * 4));
    board |= u64(cell) << (i * 4);
}

template<u8 y>
constexpr inline u16 get_row(const u64 board) {
    return (board >> (y * 16)) & 0xFFFFu;
}

template<u8 x>
constexpr inline u16 get_col(const u64 board) {
    return pext(board, 0x000F000F000F000Full << (x * 4));
}

constexpr inline u64 empty_mask(u64 board) {
    board |= board >> 2;
    board |= board >> 1;
    return ~board & 0x1111111111111111ull;
}

inline u8 count_empty(const u64 board) {
    return popcnt(empty_mask(board));
}

inline void fill_board(u64 &board) {
    //++run_stats.fill_board_counter;
    u64 empty = empty_mask(board);
    u8 pos = random(popcnt(empty));
    u64 mask = 1;
    while (!(empty & mask) || pos-- != 0) { mask <<= 4; }
    board |= mask << !random(10);
}

s_t rewards_4[E(16)];

inline s_t get_reward(const u64 board, const Dir d) {
    //++run_stats.reward_board_counter;
    if (d & 1) {
        return rewards_4[get_row<0>(board)] +
               rewards_4[get_row<1>(board)] +
               rewards_4[get_row<2>(board)] +
               rewards_4[get_row<3>(board)];
    } else {
        return rewards_4[get_col<0>(board)] +
               rewards_4[get_col<1>(board)] +
               rewards_4[get_col<2>(board)] +
               rewards_4[get_col<3>(board)];
    }
}

u64 left_0_4[E(16)];
u64 left_1_4[E(16)];
u64 left_2_4[E(16)];
u64 left_3_4[E(16)];
u64 up_0_4[E(16)];
u64 up_1_4[E(16)];
u64 up_2_4[E(16)];
u64 up_3_4[E(16)];
u64 right_0_4[E(16)];
u64 right_1_4[E(16)];
u64 right_2_4[E(16)];
u64 right_3_4[E(16)];
u64 down_0_4[E(16)];
u64 down_1_4[E(16)];
u64 down_2_4[E(16)];
u64 down_3_4[E(16)];

constexpr inline void move_board(u64 &board, const Dir d) {
    //++run_stats.move_board_counter;
    switch (d) {
        case Left:
            board ^= left_0_4[get_row<0>(board)] ^
                     left_1_4[get_row<1>(board)] ^
                     left_2_4[get_row<2>(board)] ^
                     left_3_4[get_row<3>(board)];
            break;
        case Up:
            board ^= up_0_4[get_col<0>(board)] ^
                     up_1_4[get_col<1>(board)] ^
                     up_2_4[get_col<2>(board)] ^
                     up_3_4[get_col<3>(board)];
            break;
        case Right:
            board ^= right_0_4[get_row<0>(board)] ^
                     right_1_4[get_row<1>(board)] ^
                     right_2_4[get_row<2>(board)] ^
                     right_3_4[get_row<3>(board)];
            break;
        case Down:
            board ^= down_0_4[get_col<0>(board)] ^
                     down_1_4[get_col<1>(board)] ^
                     down_2_4[get_col<2>(board)] ^
                     down_3_4[get_col<3>(board)];
            break;
        default:
            break;
    }
}

constexpr inline u64 moved_board(const u64 board, const Dir d) {
    //++run_stats.move_board_counter;
    switch (d) {
        case Left:
            return board ^
                   left_0_4[get_row<0>(board)] ^
                   left_1_4[get_row<1>(board)] ^
                   left_2_4[get_row<2>(board)] ^
                   left_3_4[get_row<3>(board)];
        case Up:
            return board ^
                   up_0_4[get_col<0>(board)] ^
                   up_1_4[get_col<1>(board)] ^
                   up_2_4[get_col<2>(board)] ^
                   up_3_4[get_col<3>(board)];
        case Right:
            return board ^
                   right_0_4[get_row<0>(board)] ^
                   right_1_4[get_row<1>(board)] ^
                   right_2_4[get_row<2>(board)] ^
                   right_3_4[get_row<3>(board)];
        case Down:
            return board ^
                   down_0_4[get_col<0>(board)] ^
                   down_1_4[get_col<1>(board)] ^
                   down_2_4[get_col<2>(board)] ^
                   down_3_4[get_col<3>(board)];
        default:
            return board;
    }
}

constexpr bool game_over(const u64 board) {
    for (const Dir dir: DIRS) {
        if (moved_board(board, dir) != board) {
            return false;
        }
    }
    return true;
}

inline u32 sum_cells(u64 board) {
    u32 sum = 0;
    while (board) {
        sum += E(board & 0xFu);
        board >>= 4;
    }
    return sum;
}

inline u64 reverse_rows(const u64 board) {
    return ((board & 0x000F000F000F000Full) << 12) |
           ((board & 0x00F000F000F000F0ull) << 04) |
           ((board & 0x0F000F000F000F00ull) >> 04) |
           ((board & 0xF000F000F000F000ull) >> 12);
}

inline u64 reverse_cols(const u64 board) {
    return ((board & 0x000000000000FFFFull) << 48) |
           ((board & 0x00000000FFFF0000ull) << 16) |
           ((board & 0x0000FFFF00000000ull) >> 16) |
           ((board & 0xFFFF000000000000ull) >> 48);
}

inline u64 transpose(const u64 board) {
    return (pext(board, 0x000F000F000F000Full) << 00) |
           (pext(board, 0x00F000F000F000F0ull) << 16) |
           (pext(board, 0x0F000F000F000F00ull) << 32) |
           (pext(board, 0xF000F000F000F000ull) << 48);
}

inline array<u64, 8> get_transformations(const u64 board) {
    const u64 transposed = transpose(board);
    const u64 reversed_rows = reverse_rows(board);
    const u64 reversed_rows_transposed = reverse_rows(transposed);
    return {
            board, reversed_rows,
            transposed, reversed_rows_transposed,
            reverse_cols(board), reverse_cols(reversed_rows),
            reverse_cols(transposed), reverse_cols(reversed_rows_transposed)
    };
}

void init_zeroes() {
    for (u32 line = 0; line < E(16); line++) {
        rewards_4[line] = 0;

        left_0_4[line] = 0;
        left_1_4[line] = 0;
        left_2_4[line] = 0;
        left_3_4[line] = 0;
        up_0_4[line] = 0;
        up_1_4[line] = 0;
        up_2_4[line] = 0;
        up_3_4[line] = 0;
        right_0_4[line] = 0;
        right_1_4[line] = 0;
        right_2_4[line] = 0;
        right_3_4[line] = 0;
        down_0_4[line] = 0;
        down_1_4[line] = 0;
        down_2_4[line] = 0;
        down_3_4[line] = 0;
    }
}

void init_moves_0() {
    for (u32 line = 0; line < E(16); line++) {
        u8 cells[4];
        u16 rev_line = 0;
        s_t reward = 0;
        for (u8 i = 0; i < 4; i++) {
            const u8 cell = get_cell(line, i);
            cells[i] = cell;
            set_cell(rev_line, 3 - i, cell);
        }
        u8 to = 0;
        bool merged = false;
        for (u8 from = 0; from < 4; from++) {
            if (cells[from] == 0) { continue; }
            if (to > 0 && cells[from] == cells[to - 1] && !merged && cells[from] != 0xFu) {
                cells[to - 1]++;
                cells[from] = 0;
                merged = true;
                reward += E(cells[to - 1]);
            } else {
                if (to != from) {
                    cells[to] = cells[from];
                    cells[from] = 0;
                }
                to++;
                merged = false;
            }
        }
        rewards_4[line] = reward;

        u16 moved = 0;
        u16 rev_moved = 0;
        for (u8 i = 0; i < 4; i++) {
            u8 cell = cells[i];
            set_cell(moved, i, cell);
            set_cell(rev_moved, 3 - i, cell);
        }
        left_0_4[line] = line ^ moved;
        up_0_4[line] = pdep(line ^ moved, 0x000F000F000F000Full);
        right_0_4[rev_line] = rev_line ^ rev_moved;
        down_0_4[rev_line] = pdep(rev_line ^ rev_moved, 0x000F000F000F000Full);
    }
}

void init_moves_123() {
    for (u32 line = 0; line < E(16); line++) {
        left_1_4[line] = left_0_4[line] << 16;
        left_2_4[line] = left_1_4[line] << 16;
        left_3_4[line] = left_2_4[line] << 16;
        up_1_4[line] = up_0_4[line] << 4;
        up_2_4[line] = up_1_4[line] << 4;
        up_3_4[line] = up_2_4[line] << 4;
        right_1_4[line] = right_0_4[line] << 16;
        right_2_4[line] = right_1_4[line] << 16;
        right_3_4[line] = right_2_4[line] << 16;
        down_1_4[line] = down_0_4[line] << 4;
        down_2_4[line] = down_1_4[line] << 4;
        down_3_4[line] = down_2_4[line] << 4;
    }
}
