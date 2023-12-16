#pragma once

#include <iostream>
#include <cstdint>
#include <cmath>
#include <array>
#include <immintrin.h>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include <unordered_map>

#define DEBUG 0

using namespace std;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using c_t = uint8_t; // cell type
using l_t = uint16_t; // line type
using b_t = uint64_t; // board type

using r_t = float; // real type
using s_t = uint32_t; // score type

enum Dir : u8 {
    None = 0,
    Left = 1,
    Up = 2,
    Right = 3,
    Down = 4,
};

struct Eval {
    Dir dir;
    r_t eval;
    s_t reward;
    b_t afterstate;
};

constexpr Dir DIRS[4] = {Left, Up, Right, Down};
constexpr pair<u8, r_t> SHIFTS[2] = {{0, 0.9},
                                     {1, 0.1}};

constexpr u64 E(const u8 n) { return u64(1) << n; }

u32 random(const u32 n) { return rand() % n; }

auto time_now() {
    return chrono::high_resolution_clock::now();
}

u64 time_since(chrono::high_resolution_clock::time_point start) {
    return chrono::duration_cast<chrono::microseconds>(time_now() - start).count();
}

inline u8 popcnt(const u64 x) { return __builtin_popcountll(x); }

inline u64 pext(const u64 x, const u64 m) { return _pext_u64(x, m); }

inline u64 pdep(const u64 x, const u64 m) { return _pdep_u64(x, m); }

u64 power(u64 base, u8 exp) {
    u64 res = 1;
    while (exp > 0) {
        if (exp & 1) {
            res *= base;
        }
        exp >>= 1;
        base *= base;
    }
    return res;
}

void print_cell(const c_t cell) {
    if (cell == 0) {
        cout << "-";
    } else if (cell < 10) {
        cout << to_string(cell);
    } else {
        cout << char('A' + (cell - 10));
    }
}

template<u8 N>
void print_board(const b_t board) {
    for (u8 y = 0; y < N; ++y) {
        for (u8 x = 0; x < N; ++x) {
            print_cell((board >> (y * (N * 4) + x * 4)) & 0xFu);
            cout << " ";
        }
        cout << endl;
    }
    cout << endl;
}

const string indent = "    ";

struct run_stats_t {
    u64 fill_board_counter = 0;
    u64 reward_board_counter = 0;
    u64 move_board_counter = 0;

    u64 eval_board_counter = 0;
    u64 update_weights_counter = 0;
    u64 eval_moves_counter = 0;

    static run_stats_t diff(run_stats_t end, run_stats_t start) {
        run_stats_t result;
        result.fill_board_counter = end.fill_board_counter - start.fill_board_counter;
        result.reward_board_counter = end.reward_board_counter - start.reward_board_counter;
        result.move_board_counter = end.move_board_counter - start.move_board_counter;
        result.eval_board_counter = end.eval_board_counter - start.eval_board_counter;
        result.update_weights_counter = end.update_weights_counter - start.update_weights_counter;
        result.eval_moves_counter = end.eval_moves_counter - start.eval_moves_counter;
        return result;
    }

    void append_stats(run_stats_t stats) {
        fill_board_counter += stats.fill_board_counter;
        reward_board_counter += stats.reward_board_counter;
        move_board_counter += stats.move_board_counter;
        eval_board_counter += stats.eval_board_counter;
        update_weights_counter += stats.update_weights_counter;
        eval_moves_counter += stats.eval_moves_counter;
    }

    void print_board_operation_stats(r_t elapsed) const {
        cout << indent << "Fill board:     \t" << r_t(fill_board_counter) / elapsed << " / us" << endl;
        cout << indent << "Reward board:   \t" << r_t(reward_board_counter) / elapsed << " / us" << endl;
        cout << indent << "Move board:     \t" << r_t(move_board_counter) / elapsed << " / us" << endl;
    }

    void print_eval_operation_stats(r_t elapsed) const {
        cout << indent << "Eval board:     \t" << r_t(eval_board_counter) / elapsed << " / us" << endl;
        cout << indent << "Update weights: \t" << r_t(update_weights_counter) / elapsed << " / us" << endl;
        cout << indent << "Eval moves:     \t" << r_t(eval_moves_counter) / elapsed << " / us" << endl;
    }
} run_stats;

struct game_stats_t {
    u64 game_counter = 0;
    u64 score_counter = 0;
    u64 moves_counter = 0;

    u32 max_moves = 0;
    s_t max_score = 0;
    u32 max_cell = 0;

    u64 score_A_counter = 0;
    u64 score_B_counter = 0;
    u64 score_C_counter = 0;
    u64 score_D_counter = 0;
    u64 score_E_counter = 0;
    u64 score_F_counter = 0;
    u64 score_G_counter = 0;
    u64 score_H_counter = 0;

    static u32 get_max_cell(b_t board) {
        u32 result = 0;
        while (board) {
            result = max(result, u32(board & 0xFu));
            board >>= 4;
        }
        return result;
    }

    void update_board_stats(b_t board, s_t score, u32 moves) {
        ++game_counter;
        score_counter += score;
        moves_counter += moves;
        max_moves = max(max_moves, moves);
        max_score = max(max_score, score);

        u32 highest_cell = get_max_cell(board);
        max_cell = max(max_cell, highest_cell);

        if (highest_cell < 10) { return; }
        ++score_A_counter;
        if (highest_cell == 10) { return; }
        ++score_B_counter;
        if (highest_cell == 11) { return; }
        ++score_C_counter;
        if (highest_cell == 12) { return; }
        ++score_D_counter;
        if (highest_cell == 13) { return; }
        ++score_E_counter;
        if (highest_cell == 14) { return; }
        ++score_F_counter;
        if (highest_cell == 15) { return; }
        ++score_G_counter;
        if (highest_cell == 16) { return; }
        ++score_H_counter;
    }

    void append_stats(game_stats_t stats) {
        game_counter += stats.game_counter;
        score_counter += stats.score_counter;
        moves_counter += stats.moves_counter;
        max_moves = max(max_moves, stats.max_moves);
        max_score = max(max_score, stats.max_score);
        max_cell = max(max_cell, stats.max_cell);
        score_A_counter += stats.score_A_counter;
        score_B_counter += stats.score_B_counter;
        score_C_counter += stats.score_C_counter;
        score_D_counter += stats.score_D_counter;
        score_E_counter += stats.score_E_counter;
        score_F_counter += stats.score_F_counter;
        score_G_counter += stats.score_G_counter;
        score_H_counter += stats.score_H_counter;
    }

    void print_average_game_stats() const {
        cout << indent << "Average score   \t" << r_t(score_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Average moves   \t" << r_t(moves_counter) / r_t(game_counter) << " / game" << endl;
    }

    void print_max_game_stats() const {
        cout << indent << "Max score:      \t" << max_score << endl;
        cout << indent << "Max moves:      \t" << max_moves << endl;
        cout << indent << "Max cell:       \t" << max_cell << endl;
    }

    void print_score_cell_stats() const {
        cout << indent << "Score A:        \t" << r_t(score_A_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Score B:        \t" << r_t(score_B_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Score C:        \t" << r_t(score_C_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Score D:        \t" << r_t(score_D_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Score E:        \t" << r_t(score_E_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Score F:        \t" << r_t(score_F_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Score G:        \t" << r_t(score_G_counter) / r_t(game_counter) << " / game" << endl;
        cout << indent << "Score H:        \t" << r_t(score_H_counter) / r_t(game_counter) << " / game" << endl;
    }
} training_stats, testing_stats;

u64 cnt = 0;
