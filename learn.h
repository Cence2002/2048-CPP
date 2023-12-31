#pragma once

#include "eval.h"

template<u8 N>
Game_stat training_episode() {
    vector<r_t> evals;
    vector<s_t> rewards;
    vector<u64> afterstates;
    s_t score = 0;

    u64 board = 0;
    fill_board<N>(board);
    fill_board<N>(board);
    while (true) {
        const auto [dir, eval, reward, afterstate]
                = eval_moves<N>(board);
        if (dir == None) { break; }
        evals.push_back(eval);
        rewards.push_back(reward);
        afterstates.push_back(afterstate);
        score += reward;

        board = afterstate;
        fill_board<N>(board);
    }
    const u32 moves = evals.size();

    r_t scaled_learning_rate = learning_rate / 8;
    if constexpr (N == 4) {
        scaled_learning_rate /= r_t(tuples_size_4);
    } else {
        scaled_learning_rate /= r_t(tuples_size_3);
    }

    r_t target = 0;
    for (u32 t = moves - 1; t < moves; --t) {
        const r_t error = target - eval_board<N>(afterstates[t]);
        target = r_t(rewards[t]) + update_weights<N>(afterstates[t], error * scaled_learning_rate);

        //const r_t error = target - evals[t];
        //update_weights<N>(afterstates[t], error * scaled_learning_rate);
        //target = r_t(rewards[t]) + evals[t];
    }

    //training_stats.update_board_stats(board, score, moves);

    return {board, score, moves};
}

template<u8 N>
Game_stat testing_episode() {
    u64 board = 0;
    s_t score = 0;
    u32 moves = 0;

    fill_board<N>(board);
    fill_board<N>(board);
    while (true) {
        const auto [dir, eval, reward, afterstate]
                = eval_moves<N>(board);
        if (dir == None) { break; }
        score += reward;
        ++moves;

        board = afterstate;
        fill_board<N>(board);
    }

    //testing_stats.update_board_stats(board, score, moves);

    return {board, score, moves};
}

template<u8 N>
vector<Game_stat> run_training_episodes(u32 games, u8 threads) {
    srand(42);
    cout << "Training started (" << games << " games)" << endl;

    run_stats = {};
    vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(training_episode<N>());
        }
    } else {
        vector<thread> all_threads;
        const u32 threads_games = games / threads;
        vector<vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats]() {
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(training_episode<N>());
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
    training_stats = {};
    for (const auto &game_stats: games_stats) {
        training_stats.update_board_stats(game_stats.board, game_stats.score, game_stats.moves);
    }
    r_t elapsed = time_since(start);

    if (DEBUG) {
        run_stats.print_board_operation_stats(elapsed);
        run_stats.print_eval_operation_stats(elapsed);

        training_stats.print_average_game_stats();
        training_stats.print_max_game_stats();
        training_stats.print_score_cell_stats();
    } else {
        cout << indent << "Move per us:    \t" << r_t(run_stats.move_board_counter) / elapsed << endl;
        cout << indent << "Eval per us:    \t" << r_t(run_stats.eval_board_counter) / elapsed << endl;
        cout << indent << "Update per us:  \t" << r_t(run_stats.update_weights_counter) / elapsed << endl;
        cout << indent << "Ms per game:    \t" << (elapsed / 1e3) / r_t(training_stats.game_counter) << endl;
    }

    cout << "Training finished (" << time_since(start) / 1e6 << " s)" << endl;

    return games_stats;
}

template<u8 N>
vector<Game_stat> run_testing_episodes(u32 games, u8 threads) {
    srand(42);
    cout << "Testing started (" << games << " games)" << endl;

    run_stats = {};
    vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(testing_episode<N>());
        }
    } else {
        vector<thread> all_threads;
        const u32 threads_games = games / threads;
        vector<vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats]() {
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(testing_episode<N>());
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

    if (DEBUG) {
        run_stats.print_board_operation_stats(elapsed);
        run_stats.print_eval_operation_stats(elapsed);

        testing_stats.print_average_game_stats();
        testing_stats.print_max_game_stats();
        testing_stats.print_score_cell_stats();
    } else {
        cout << indent << "Move per us:    \t" << r_t(run_stats.move_board_counter) / elapsed << endl;
        cout << indent << "Eval per us:    \t" << r_t(run_stats.eval_board_counter) / elapsed << endl;
        cout << indent << "Ms per game:    \t" << (elapsed / 1e3) / r_t(testing_stats.game_counter) << endl;
        cout << indent << "Move per game:  \t" << r_t(testing_stats.moves_counter) / r_t(testing_stats.game_counter) << endl;
        cout << indent << "Score per game: \t" << r_t(testing_stats.score_counter) / r_t(testing_stats.game_counter) << endl;
    }

    cout << "Testing finished (" << time_since(start) / 1e6 << " s)" << endl;

    return games_stats;
}

template<u8 N>
void run_learning(u32 episodes, u32 training_games, u32 testing_games, u8 threads) {
    cout << "Learning started (" << episodes << " episodes, LR=" << learning_rate << ")" << endl;

    run_stats_t all_run_stats = {};
    game_stats_t all_training_stats = {};
    game_stats_t all_testing_stats = {};

    run_testing_episodes<N>(testing_games, threads);
    cout << endl;

    auto start = time_now();
    for (u32 i = 0; i < episodes; i++) {
        cout << "Episode " << i + 1 << endl;

        run_training_episodes<N>(training_games, threads);
        all_training_stats.append_stats(training_stats);
        all_run_stats.append_stats(run_stats);

        run_testing_episodes<N>(testing_games, threads);
        all_run_stats.append_stats(run_stats);
        all_testing_stats.append_stats(testing_stats);

        cout << endl;
    }
    r_t elapsed = time_since(start);

    if (DEBUG) {
        /*cout << "Overall run stats per s" << endl;
        cout << indent << "Fill board:     \t" << r_t(all_run_stats.fill_board_counter) / elapsed << endl;
        cout << indent << "Reward board:   \t" << r_t(all_run_stats.reward_board_counter) / elapsed << endl;
        cout << indent << "Move board:     \t" << r_t(all_run_stats.move_board_counter) / elapsed << endl;
        cout << indent << "Eval board:     \t" << r_t(all_run_stats.eval_board_counter) / elapsed << endl;
        cout << indent << "Update weights: \t" << r_t(all_run_stats.update_weights_counter) / elapsed << endl;
        cout << indent << "Eval moves:     \t" << r_t(all_run_stats.eval_moves_counter) / elapsed << endl;

        cout << "Overall training stats per game (" << all_training_stats.game_counter << " games)" << endl;
        cout << indent << "Score per game: \t" << r_t(all_training_stats.score_counter) / r_t(all_training_stats.game_counter) << endl;
        cout << indent << "Moves per game: \t" << r_t(all_training_stats.moves_counter) / r_t(all_training_stats.game_counter) << endl;

        cout << "Overall testing stats per game (" << all_testing_stats.game_counter << " games)" << endl;
        cout << indent << "Score per game: \t" << r_t(all_testing_stats.score_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Moves per game: \t" << r_t(all_testing_stats.moves_counter) / r_t(all_testing_stats.game_counter) << endl;

        cout << indent << "Max moves:      \t" << all_testing_stats.max_moves << endl;
        cout << indent << "Max score:      \t" << all_testing_stats.max_score << endl;
        cout << indent << "Max cell:       \t" << all_testing_stats.max_cell << endl;
        cout << indent << "Score A:        \t" << r_t(all_testing_stats.score_A_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score B:        \t" << r_t(all_testing_stats.score_B_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score C:        \t" << r_t(all_testing_stats.score_C_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score D:        \t" << r_t(all_testing_stats.score_D_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score E:        \t" << r_t(all_testing_stats.score_E_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score F:        \t" << r_t(all_testing_stats.score_F_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score G:        \t" << r_t(all_testing_stats.score_G_counter) / r_t(all_testing_stats.game_counter) << endl;
        cout << indent << "Score H:        \t" << r_t(all_testing_stats.score_H_counter) / r_t(all_testing_stats.game_counter) << endl;*/

        cout << "Overall run stats (" << elapsed << " us)" << endl;
        all_run_stats.print_board_operation_stats(elapsed);
        all_run_stats.print_eval_operation_stats(elapsed);

        cout << "Overall training stats (" << all_training_stats.game_counter << " games)" << endl;
        all_training_stats.print_average_game_stats();
        all_training_stats.print_max_game_stats();
        all_training_stats.print_score_cell_stats();

        cout << "Overall testing stats (" << all_testing_stats.game_counter << " games)" << endl;
        all_testing_stats.print_average_game_stats();
        all_testing_stats.print_max_game_stats();
        all_testing_stats.print_score_cell_stats();
    }

    cout << "Learning finished (" << elapsed / 1e6 << " s)" << endl;
    cout << endl;
}
