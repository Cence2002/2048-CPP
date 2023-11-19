#pragma once

#include "assets.h"

template<u8 N>
u64 from_array(const array<array<u8, N>, N> &cells) {
    u64 board = 0;
    for (u8 y = 0; y < N; y++) {
        for (u8 x = 0; x < N; x++) {
            board |= u64(cells[y][x]) << (y * (N * 4) + x * 4);
        }
    }
    return board;
}

template<u8 N>
array<array<u8, N>, N> to_array(const u64 board) {
    array<array<u8, N>, N> cells{};
    for (u8 y = 0; y < N; y++) {
        for (u8 x = 0; x < N; x++) {
            cells[y][x] = (board >> (y * (N * 4) + x * 4)) & 0xFu;
        }
    }
    return cells;
}

template<u8 N, u8 y>
inline u16 get_row(const u64 board) {
    if constexpr (N == 4) {
        return (board >> (y * 16)) & 0xFFFFu;
    } else {
        return (board >> (y * 12)) & 0xFFFu;
    }
}

template<u8 N, u8 x>
inline u16 get_col(const u64 board) {
    if constexpr (N == 4) {
        return pext(board, 0x000F000F000F000Full << (x * 4));
    } else {
        return pext(board, 0x00F00F00Full << (x * 4));
    }
}

template<u8 N>
inline u64 empty_mask(u64 board) {
    board |= board >> 2;
    board |= board >> 1;
    if constexpr (N == 4) {
        return ~board & 0x1111111111111111ull;
    } else {
        return ~board & 0x111111111ull;
    }
}

template<u8 N>
inline u8 count_empty(const u64 board) {
    return popcnt(empty_mask<N>(board));
}

template<u8 N>
inline void fill_board(u64 &board) {
    u64 empty = empty_mask<N>(board);
    u8 pos = random(popcnt(empty));
    u64 mask = 1;
    while (!(empty & mask) || pos-- != 0) { mask <<= 4; }
    board |= mask << !random(10);
}

u32 rewards_4[E(16)];
u32 rewards_3[E(12)];

template<u8 N>
inline u32 get_reward(const u64 board, const Dir d) {
    if constexpr (N == 4) {
        if (d & 1) {
            return rewards_4[get_row<4, 0>(board)] +
                   rewards_4[get_row<4, 1>(board)] +
                   rewards_4[get_row<4, 2>(board)] +
                   rewards_4[get_row<4, 3>(board)];
        } else {
            return rewards_4[get_col<4, 0>(board)] +
                   rewards_4[get_col<4, 1>(board)] +
                   rewards_4[get_col<4, 2>(board)] +
                   rewards_4[get_col<4, 3>(board)];
        }
    } else {
        if (d & 1) {
            return rewards_3[get_row<3, 0>(board)] +
                   rewards_3[get_row<3, 1>(board)] +
                   rewards_3[get_row<3, 2>(board)];
        } else {
            return rewards_3[get_col<3, 0>(board)] +
                   rewards_3[get_col<3, 1>(board)] +
                   rewards_3[get_col<3, 2>(board)];
        }
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

u64 left_0_3[E(12)];
u64 left_1_3[E(12)];
u64 left_2_3[E(12)];
u64 up_0_3[E(12)];
u64 up_1_3[E(12)];
u64 up_2_3[E(12)];
u64 right_0_3[E(12)];
u64 right_1_3[E(12)];
u64 right_2_3[E(12)];
u64 down_0_3[E(12)];
u64 down_1_3[E(12)];
u64 down_2_3[E(12)];

template<u8 mode>
inline void move_board(u64 &board, const Dir d) {
    if constexpr (mode == 4) {
        switch (d) {
            case Left:
                board ^= left_0_4[get_row<4, 0>(board)] ^
                         left_1_4[get_row<4, 1>(board)] ^
                         left_2_4[get_row<4, 2>(board)] ^
                         left_3_4[get_row<4, 3>(board)];
                break;
            case Up:
                board ^= up_0_4[get_col<4, 0>(board)] ^
                         up_1_4[get_col<4, 1>(board)] ^
                         up_2_4[get_col<4, 2>(board)] ^
                         up_3_4[get_col<4, 3>(board)];
                break;
            case Right:
                board ^= right_0_4[get_row<4, 0>(board)] ^
                         right_1_4[get_row<4, 1>(board)] ^
                         right_2_4[get_row<4, 2>(board)] ^
                         right_3_4[get_row<4, 3>(board)];
                break;
            case Down:
                board ^= down_0_4[get_col<4, 0>(board)] ^
                         down_1_4[get_col<4, 1>(board)] ^
                         down_2_4[get_col<4, 2>(board)] ^
                         down_3_4[get_col<4, 3>(board)];
                break;
            default:
                break;
        }
    } else {
        switch (d) {
            case Left:
                board ^= left_0_3[get_row<3, 0>(board)] ^
                         left_1_3[get_row<3, 1>(board)] ^
                         left_2_3[get_row<3, 2>(board)];
                break;
            case Up:
                board ^= up_0_3[get_col<3, 0>(board)] ^
                         up_1_3[get_col<3, 1>(board)] ^
                         up_2_3[get_col<3, 2>(board)];
                break;
            case Right:
                board ^= right_0_3[get_row<3, 0>(board)] ^
                         right_1_3[get_row<3, 1>(board)] ^
                         right_2_3[get_row<3, 2>(board)];
                break;
            case Down:
                board ^= down_0_3[get_col<3, 0>(board)] ^
                         down_1_3[get_col<3, 1>(board)] ^
                         down_2_3[get_col<3, 2>(board)];
                break;
            default:
                break;
        }
    }
}

template<u8 N>
inline u64 moved_board(const u64 board, const Dir d) {
    u64 res = board;
    move_board<N>(res, d);
    return res;
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
    for (u32 line = 0; line < E(12); line++) {
        rewards_3[line] = 0;

        left_0_3[line] = 0;
        left_1_3[line] = 0;
        left_2_3[line] = 0;
        up_0_3[line] = 0;
        up_1_3[line] = 0;
        up_2_3[line] = 0;
        right_0_3[line] = 0;
        right_1_3[line] = 0;
        right_2_3[line] = 0;
        down_0_3[line] = 0;
        down_1_3[line] = 0;
        down_2_3[line] = 0;
    }
}

template<u8 N>
void init_moves_0() {
    for (u32 line = 0; line < E(N * 4); line++) {
        u8 cells[3];
        u16 rev_line = 0;
        u32 reward = 0;
        for (u8 i = 0; i < N; i++) {
            u16 cell = (line >> (i * 4)) & 0xFu;
            cells[i] = cell;
            rev_line |= cell << ((N - 1 - i) * 4);
        }
        u8 to = 0;
        bool merged = false;
        for (u8 from = 0; from < N; from++) {
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
        if constexpr (N == 4) {
            rewards_4[line] = reward;
        } else {
            rewards_3[line] = reward;
        }

        u16 moved = 0;
        u16 rev_moved = 0;
        for (u8 i = 0; i < N; i++) {
            u8 cell = cells[i];
            moved |= cell << (i * 4);
            rev_moved |= cell << ((N - 1 - i) * 4);
        }
        if constexpr (N == 4) {
            left_0_4[line] = line ^ moved;
            up_0_4[line] = pdep(line ^ moved, 0x000F000F000F000Full);
            right_0_4[rev_line] = rev_line ^ rev_moved;
            down_0_4[rev_line] = pdep(rev_line ^ rev_moved, 0x000F000F000F000Full);
        } else {
            left_0_3[line] = line ^ moved;
            up_0_3[line] = pdep(line ^ moved, 0x00F00F00Full);
            right_0_3[rev_line] = rev_line ^ rev_moved;
            down_0_3[rev_line] = pdep(rev_line ^ rev_moved, 0x00F00F00Full);
        }
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
    for (u32 line = 0; line < E(12); line++) {
        left_1_3[line] = left_0_3[line] << 12;
        left_2_3[line] = left_1_3[line] << 12;
        up_1_3[line] = up_0_3[line] << 4;
        up_2_3[line] = up_1_3[line] << 4;
        right_1_3[line] = right_0_3[line] << 12;
        right_2_3[line] = right_1_3[line] << 12;
        down_1_3[line] = down_0_3[line] << 4;
        down_2_3[line] = down_1_3[line] << 4;
    }
}

template<u8 N>
inline u64 reverse_rows(const u64 board) {
    if constexpr (N == 4) {
        return ((board & 0x000F000F000F000Full) << 12) |
               ((board & 0x00F000F000F000F0ull) << 04) |
               ((board & 0x0F000F000F000F00ull) >> 04) |
               ((board & 0xF000F000F000F000ull) >> 12);
    } else {
        return ((board & 0x00F00F00Full) << 8) |
               ((board & 0x0F00F00F0ull) << 0) |
               ((board & 0xF00F00F00ull) >> 8);
    }
}

template<u8 N>
inline u64 reverse_cols(const u64 board) {
    if constexpr (N == 4) {
        return ((board & 0x000000000000FFFFull) << 48) |
               ((board & 0x00000000FFFF0000ull) << 16) |
               ((board & 0x0000FFFF00000000ull) >> 16) |
               ((board & 0xFFFF000000000000ull) >> 48);
    } else {
        return ((board & 0x000000FFFull) << 24) |
               ((board & 0x000FFF000ull) << 00) |
               ((board & 0xFFF000000ull) >> 24);
    }
}


template<u8 N>
inline u64 transpose(const u64 board) {
    if constexpr (N == 4) {
        return (pext(board, 0x000F000F000F000Full) << 00) |
               (pext(board, 0x00F000F000F000F0ull) << 16) |
               (pext(board, 0x0F000F000F000F00ull) << 32) |
               (pext(board, 0xF000F000F000F000ull) << 48);
    } else {
        return (pext(board, 0x00F00F00Full) << 00) |
               (pext(board, 0x0F00F00F0ull) << 12) |
               (pext(board, 0xF00F00F00ull) << 24);
    }
}

template<u8 N>
inline array<u64, 8> get_transformations(const u64 board) {
    const u64 transposed = transpose<N>(board);
    const u64 reversed_rows = reverse_rows<N>(board);
    const u64 transposed_reversed_rows = reverse_rows<N>(transposed);
    return {
            board, reversed_rows,
            transposed, transposed_reversed_rows,
            reverse_cols<N>(board), reverse_cols<N>(reversed_rows),
            reverse_cols<N>(transposed), reverse_cols<N>(transposed_reversed_rows)
    };
}