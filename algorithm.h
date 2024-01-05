#pragma once

#include "eval.h"

template<u8 N>
Game_stat algorithm_episode(Dir (*algorithm)(const u64)) {
    u64 board = 0;
    s_t score = 0;
    u32 moves = 0;

    fill_board<N>(board);
    fill_board<N>(board);
    while (true) {
        const Dir dir = algorithm(board);
        if (dir == None) { break; }
        score += get_reward<N>(board, dir);
        ++moves;

        move_board<N>(board, dir);
        fill_board<N>(board);
    }

    return {board, score, moves};
}

template<u8 N>
vector<Game_stat> run_algorithm_episodes(u32 games, u8 threads, Dir (*algorithm)(const u64)) {
    srand(42);
    cout << "Testing started (" << games << " games)" << endl;

    run_stats = {};
    vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(algorithm_episode<N>(algorithm));
        }
    } else {
        vector<thread> all_threads;
        const u32 threads_games = games / threads;
        vector<vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats, algorithm]() {
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(algorithm_episode<N>(algorithm));
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
        testing_stats.update_board_stats(game_stats.board, game_stats.score, game_stats.moves);
    }
    r_t elapsed = time_since(start);

    testing_stats.print_average_game_stats();
    testing_stats.print_max_game_stats();
    testing_stats.print_score_cell_stats();
    cout << indent << "S per game:    \t" << (elapsed / 1e6) / r_t(testing_stats.game_counter) << endl;
    cout << indent << "Ms per move:    \t" << (elapsed / 1e3) / r_t(testing_stats.moves_counter) << endl;

    cout << "Testing finished (" << time_since(start) / 1e6 << " s)" << endl;

    return games_stats;
}