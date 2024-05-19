#pragma once

#include "eval.h"

u32 game_counter = 0;

inline Game_stat algorithm_episode(Dir (*algorithm)(const b_t)) {
    b_t board = 0;
    s_t score = 0;
    u32 moves = 0;

    board.spawn();
    board.spawn();
    while (true) {
        const Dir dir = algorithm(board);
        if (dir == None) { break; }
        score += board.get_reward(dir);
        ++moves;

        board.slide(dir);
        board.spawn();
        //if (random(10) == 0)std::cout << board.to_string() << std::endl;
    }
    std::ostringstream oss;
    oss << "Game " << ++game_counter << " => score=" << score << " moves=" << moves << " board=" << std::hex << board.get_bits() << std::dec << std::endl;
    std::cout << oss.str();

    //std::cout << board.to_string() << std::endl;

    return {board, score, moves};
}

inline std::vector<Game_stat> run_algorithm_episodes(u32 games, u8 threads, Dir (*algorithm)(const b_t)) {
    srand(42);

    run_stats = {};
    std::vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(algorithm_episode(algorithm));
        }
    } else {
        std::vector<std::thread> all_threads;
        const u32 threads_games = games / threads;
        std::vector<std::vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats, algorithm]() {
                init_rng();
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(algorithm_episode(algorithm));
                }
            });
        }
        for (auto &thread: all_threads) {
            thread.join();
        }
        for (const auto &thread_stats: threads_stats) {
            games_stats.insert(games_stats.end(), thread_stats.begin(), thread_stats.end());
        }
    }
    testing_stats = {};
    for (const auto &game_stats: games_stats) {
        testing_stats.update_board_stats(game_stats.board.get_bits(), game_stats.score, game_stats.moves);
    }
    r_t elapsed = time_since(start);

    testing_stats.print_average_game_stats();
    testing_stats.print_max_game_stats();
    testing_stats.print_score_cell_stats();
    //cout << indent << "S per game:    \t" << (elapsed / 1e6) / r_t(testing_stats.game_counter) << endl;
    //cout << indent << "Moves per uS:  \t" << r_t(testing_stats.moves_counter) / elapsed << endl;

    return games_stats;
}
