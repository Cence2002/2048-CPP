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
#include <algorithm>
#include <thread>
#include <random>
#include <mutex>
#include <cassert>

constexpr bool DEBUG_PRINT = true;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using r_t = float;
using s_t = uint32_t;

enum Dir : u8 {
    None = 0,
    Left = 1,
    Up = 2,
    Right = 3,
    Down = 4,
};

constexpr auto DIRS = {Left, Up, Right, Down};

constexpr std::pair<u8, r_t> SHIFTS[2] = {
        {0, 0.9},
        {1, 0.1}
};

constexpr std::array<r_t, 17> RECIPROCALS = {
        r_t(0),
        r_t(1) / 1, r_t(1) / 2, r_t(1) / 3, r_t(1) / 4,
        r_t(1) / 5, r_t(1) / 6, r_t(1) / 7, r_t(1) / 8,
        r_t(1) / 9, r_t(1) / 10, r_t(1) / 11, r_t(1) / 12,
        r_t(1) / 13, r_t(1) / 14, r_t(1) / 15, r_t(1) / 16
};

constexpr u64 E(const u8 n) { return u64(1) << n; }

inline auto time_now() {
    return std::chrono::high_resolution_clock::now();
}
inline const auto execution_start = time_now();

inline r_t time_since(std::chrono::high_resolution_clock::time_point start) {
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_now() - start);
    return r_t(duration.count());
}

inline void wait(const r_t time) {
    std::this_thread::sleep_for(std::chrono::seconds(u32(time)));
}

inline std::mutex init_rng_mutex;
inline thread_local std::mt19937 rng;

inline void init_rng(u32 seed = 0) {
    std::lock_guard<std::mutex> lock(init_rng_mutex);
    if (seed == 0) {
        seed = u32(time_since(execution_start));
    }
    std::cout << "T-" << std::this_thread::get_id() << " seed: " << seed << std::endl;
    rng.seed(seed);
}

inline thread_local std::array<std::uniform_int_distribution<u8>, 20> dists = {
        std::uniform_int_distribution<u8>(0, 0), // should never be used
        std::uniform_int_distribution<u8>(0, 0),
        std::uniform_int_distribution<u8>(0, 1),
        std::uniform_int_distribution<u8>(0, 2),
        std::uniform_int_distribution<u8>(0, 3),
        std::uniform_int_distribution<u8>(0, 4),
        std::uniform_int_distribution<u8>(0, 5),
        std::uniform_int_distribution<u8>(0, 6),
        std::uniform_int_distribution<u8>(0, 7),
        std::uniform_int_distribution<u8>(0, 8),
        std::uniform_int_distribution<u8>(0, 9),
        std::uniform_int_distribution<u8>(0, 10),
        std::uniform_int_distribution<u8>(0, 11),
        std::uniform_int_distribution<u8>(0, 12),
        std::uniform_int_distribution<u8>(0, 13),
        std::uniform_int_distribution<u8>(0, 14),
        std::uniform_int_distribution<u8>(0, 15),
        std::uniform_int_distribution<u8>(0, 16),
        std::uniform_int_distribution<u8>(0, 17),
        std::uniform_int_distribution<u8>(0, 18),
};

inline u8 random(const u8 n) {
    return dists[n](rng);
}

inline u8 popcnt(const u64 x) { return __builtin_popcountll(x); }

inline u64 pext(const u64 x, const u64 m) { return _pext_u64(x, m); }

inline u64 pdep(const u64 x, const u64 m) { return _pdep_u64(x, m); }

inline u64 power(u64 base, u8 exp) {
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

constexpr r_t interpolate(const r_t start, const r_t end, const r_t t) {
    return start + (end - start) * t;
}

constexpr r_t factorial(const u8 n) {
    if (n == 0) { return 1; }
    return r_t(n) * factorial(n - 1);
}

constexpr r_t binomial(u8 n, u8 k) {
    return factorial(n) / (factorial(k) * factorial(n - k));
}

constexpr r_t st_dev(const u64 sum, const u64 sum_squared, const u64 n) {
    return sqrt(r_t(sum_squared) / r_t(n) - r_t(sum) * r_t(sum) / r_t(n * n));
}

constexpr r_t conf_rad(const u64 sum, const u64 sum_squared, const u64 n) {
    constexpr r_t z = 1.95996; // 95%
    //constexpr r_t z = 1.01522; // 69%
    return z * st_dev(sum, sum_squared, n) / sqrt(r_t(n));
}

inline void save_array(const std::string &filename, const char *arr, const size_t size) {
    std::cout << "Saving " << filename << " started" << std::endl;
    auto start = time_now();
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error opening: " << filename << std::endl;
        return;
    }
    file.write(arr, std::streamsize(size));
    file.close();
    std::cout << "Saving " << filename << " finished: " << time_since(start) / 1e6 << " s" << std::endl;
}

inline void load_array(const std::string &filename, char *arr, const size_t size) {
    std::cout << "Loading " << filename << " started" << std::endl;
    auto start = time_now();
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error opening: " << filename << std::endl;
        return;
    }
    file.read(arr, std::streamsize(size));
    file.close();
    std::cout << "Loading " << filename << " finished: " << time_since(start) / 1e6 << " s" << std::endl;
}

inline std::string get_time_str() {
    auto ts = time(nullptr);
    auto local_ts = *localtime(&ts);
    std::ostringstream temp;
    temp << std::put_time(&local_ts, "%m%d_%H%M%S");
    return temp.str();
}


inline void print_dir(const Dir dir) {
    switch (dir) {
        case Left:
            std::cout << "Left";
            break;
        case Up:
            std::cout << "Up";
            break;
        case Right:
            std::cout << "Right";
            break;
        case Down:
            std::cout << "Down";
            break;
        default:
            std::cout << "None";
            break;
    }
}

std::array<char, 20> cell_str = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'
};

inline void print_cell(const u8 cell) {
    if (cell == 0) {
        std::cout << "-";
    } else if (cell < 10) {
        std::cout << std::to_string(cell);
    } else {
        std::cout << char('A' + (cell - 10));
    }
}

inline void print_board(const u64 board) {
    for (u8 y = 0; y < 4; ++y) {
        for (u8 x = 0; x < 4; ++x) {
            print_cell((board >> (y * 16 + x * 4)) & 0xFu);
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

const std::string indent = "    ";

inline struct run_stats_t {
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
        std::cout << indent << "Fill board:     \t" << r_t(fill_board_counter) / elapsed << " / us" << std::endl;
        std::cout << indent << "Reward board:   \t" << r_t(reward_board_counter) / elapsed << " / us" << std::endl;
        std::cout << indent << "Move board:     \t" << r_t(move_board_counter) / elapsed << " / us" << std::endl;
    }

    void print_eval_operation_stats(r_t elapsed) const {
        std::cout << indent << "Eval board:     \t" << r_t(eval_board_counter) / elapsed << " / us" << std::endl;
        std::cout << indent << "Update weights: \t" << r_t(update_weights_counter) / elapsed << " / us" << std::endl;
        std::cout << indent << "Eval moves:     \t" << r_t(eval_moves_counter) / elapsed << " / us" << std::endl;
    }
} run_stats;

inline struct game_stats_t {
    u64 game_counter = 0;
    u64 score_counter = 0;
    u64 score_squared_counter = 0;
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

    static u32 get_max_cell(u64 board) {
        u32 result = 0;
        while (board) {
            result = std::max(result, u32(board & 0xFu));
            board >>= 4;
        }
        return result;
    }

    void update_board_stats(u64 board, s_t score, u32 moves) {
        ++game_counter;
        score_counter += score;
        score_squared_counter += u64(score) * u64(score);
        moves_counter += moves;
        max_moves = std::max(max_moves, moves);
        max_score = std::max(max_score, score);

        u32 highest_cell = get_max_cell(board);
        max_cell = std::max(max_cell, highest_cell);

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
        score_squared_counter += stats.score_squared_counter;
        moves_counter += stats.moves_counter;
        max_moves = std::max(max_moves, stats.max_moves);
        max_score = std::max(max_score, stats.max_score);
        max_cell = std::max(max_cell, stats.max_cell);
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
        std::cout << indent << "Average score   \t" << r_t(score_counter) / r_t(game_counter) << " / game" << std::endl;
        std::cout << indent << "Confidence rad  \t" << conf_rad(score_counter, score_squared_counter, game_counter) << std::endl;
        std::cout << indent << "St dev score    \t" << st_dev(score_counter, score_squared_counter, game_counter) << std::endl;
        std::cout << indent << "Average moves   \t" << r_t(moves_counter) / r_t(game_counter) << " / game" << std::endl;
    }

    void print_max_game_stats() const {
        std::cout << indent << "Max score:      \t" << max_score << std::endl;
        std::cout << indent << "Max moves:      \t" << max_moves << std::endl;
        std::cout << indent << "Max cell:       \t" << max_cell << std::endl;
    }

    void print_score_cell_stats() const {
        std::cout << indent << "Score A:        \t" << r_t(score_A_counter) / r_t(game_counter) << " / game" << std::endl;
        std::cout << indent << "Score B:        \t" << r_t(score_B_counter) / r_t(game_counter) << " / game" << std::endl;
        std::cout << indent << "Score C:        \t" << r_t(score_C_counter) / r_t(game_counter) << " / game" << std::endl;
        std::cout << indent << "Score D:        \t" << r_t(score_D_counter) / r_t(game_counter) << " / game" << std::endl;
        std::cout << indent << "Score E:        \t" << r_t(score_E_counter) / r_t(game_counter) << " / game" << std::endl;
        std::cout << indent << "Score F:        \t" << r_t(score_F_counter) / r_t(game_counter) << " / game" << std::endl;
        //std::cout << indent << "Score G:        \t" << r_t(score_G_counter) / r_t(game_counter) << " / game" << std::endl;
        //std::cout << indent << "Score H:        \t" << r_t(score_H_counter) / r_t(game_counter) << " / game" << std::endl;
    }
} training_stats, testing_stats;
