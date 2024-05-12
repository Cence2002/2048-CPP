#pragma once

#include "eval.h"

Game_stat training_episode(NTuple &tuples) {
    std::vector<r_t> evals;
    std::vector<s_t> rewards;
    std::vector<b_t> afterstates;

    b_t board = 0;
    s_t score = 0;

    board.spawn();
    board.spawn();
    while (true) {
        const auto [dir, eval, reward, afterstate] = eval_state(board, tuples);
        if (dir == None) { break; }
        evals.push_back(eval);
        rewards.push_back(reward);
        afterstates.push_back(afterstate);
        score += reward;

        board = afterstate;
        board.spawn();
    }
    const u32 moves = evals.size();

    r_t scaled_learning_rate = learning_rate / 8;
    scaled_learning_rate /= r_t(tuples.size());

    r_t target = 0;
    for (u32 t = moves - 1; t < moves; --t) {
        const r_t error = target - add_weights(afterstates[t], tuples);
        target = r_t(rewards[t]) + update_weights(afterstates[t], error * scaled_learning_rate, tuples);
    }

    return {board, score, moves};
}

Game_stat testing_episode(const NTuple &tuples) {
    b_t board = 0;
    s_t score = 0;
    u32 moves = 0;

    board.spawn();
    board.spawn();
    while (true) {
        const auto [dir, eval, reward, afterstate]
                = eval_state(board, tuples);
        if (dir == None) { break; }
        score += reward;
        ++moves;

        board = afterstate;
        board.spawn();
    }

    return {board, score, moves};
}

std::vector<Game_stat> run_training_episodes(u32 games, u8 threads, NTuple &tuples) {
    srand(42);
    std::cout << "Training started (" << games << " games)" << std::endl;

    run_stats = {};
    std::vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(training_episode(tuples));
        }
    } else {
        std::vector<std::thread> all_threads;
        const u32 threads_games = games / threads;
        std::vector<std::vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats, &tuples]() {
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(training_episode(tuples));
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
        training_stats.update_board_stats(game_stats.board.get_bits(), game_stats.score, game_stats.moves);
    }
    r_t elapsed = time_since(start);

    if (DEBUG_PRINT) {
        run_stats.print_board_operation_stats(elapsed);
        run_stats.print_eval_operation_stats(elapsed);

        training_stats.print_average_game_stats();
        training_stats.print_max_game_stats();
        training_stats.print_score_cell_stats();
    } else {
        //std::cout << indent << "Move per us:    \t" << r_t(run_stats.move_board_counter) / elapsed << std::endl;
        //std::cout << indent << "Eval per us:    \t" << r_t(run_stats.eval_board_counter) / elapsed << std::endl;
        //std::cout << indent << "Update per us:  \t" << r_t(run_stats.update_weights_counter) / elapsed << std::endl;
        std::cout << indent << "Ms per game:    \t" << (elapsed / 1e3) / r_t(training_stats.game_counter) << std::endl;
        std::cout << indent << "Move per game:  \t" << r_t(training_stats.moves_counter) / r_t(training_stats.game_counter) << std::endl;
    }

    std::cout << "Training finished (" << time_since(start) / 1e6 << " s)" << std::endl;

    return games_stats;
}

std::vector<Game_stat> run_testing_episodes(u32 games, u8 threads, const NTuple &tuples) {
    srand(42);
    std::cout << "Testing started (" << games << " games)" << std::endl;

    run_stats = {};
    std::vector<Game_stat> games_stats;
    auto start = time_now();
    if (threads == 0) {
        for (u32 i = 0; i < games; i++) {
            games_stats.push_back(testing_episode(tuples));
        }
    } else {
        std::vector<std::thread> all_threads;
        const u32 threads_games = games / threads;
        std::vector<std::vector<Game_stat>> threads_stats(threads);
        for (u8 t = 0; t < threads; ++t) {
            all_threads.emplace_back([t, threads_games, &threads_stats, &tuples]() {
                for (u32 i = 0; i < threads_games; ++i) {
                    threads_stats[t].push_back(testing_episode(tuples));
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

    if (DEBUG_PRINT) {
        run_stats.print_board_operation_stats(elapsed);
        run_stats.print_eval_operation_stats(elapsed);

        testing_stats.print_average_game_stats();
        testing_stats.print_max_game_stats();
        testing_stats.print_score_cell_stats();
    } else {
        testing_stats.print_score_cell_stats();
        //std::cout << indent << "Move per us:    \t" << r_t(run_stats.move_board_counter) / elapsed << std::endl;
        //std::cout << indent << "Eval per us:    \t" << r_t(run_stats.eval_board_counter) / elapsed << std::endl;
        std::cout << indent << "Ms per game:    \t" << (elapsed / 1e3) / r_t(testing_stats.game_counter) << std::endl;
        std::cout << indent << "Move per game:  \t" << r_t(testing_stats.moves_counter) / r_t(testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score per game: \t" << r_t(testing_stats.score_counter) / r_t(testing_stats.game_counter) << std::endl;
    }

    std::cout << "Testing finished (" << time_since(start) / 1e6 << " s)" << std::endl;

    return games_stats;
}

void run_learning(u32 episodes, u32 training_games, u32 testing_games, u8 threads, NTuple &tuples) {
    std::cout << "Learning started (" << episodes << " episodes, LR=" << learning_rate << ")" << std::endl;

    run_stats_t all_run_stats = {};
    game_stats_t all_training_stats = {};
    game_stats_t all_testing_stats = {};

    run_testing_episodes(testing_games, threads, tuples);
    std::cout << std::endl;

    auto start = time_now();
    for (u32 i = 0; i < episodes; i++) {
        std::cout << "Episode " << i + 1 << std::endl;

        run_training_episodes(training_games, threads, tuples);
        all_training_stats.append_stats(training_stats);
        all_run_stats.append_stats(run_stats);

        run_testing_episodes(testing_games, threads, tuples);
        all_run_stats.append_stats(run_stats);
        all_testing_stats.append_stats(testing_stats);

        std::cout << std::endl;
    }
    r_t elapsed = time_since(start);

    if (DEBUG_PRINT) {
        /*std::cout << "Overall run stats per s" << std::endl;
        std::cout << indent << "Fill board:     \t" << r_t(all_run_stats.fill_board_counter) / elapsed << std::endl;
        std::cout << indent << "Reward board:   \t" << r_t(all_run_stats.reward_board_counter) / elapsed << std::endl;
        std::cout << indent << "Move board:     \t" << r_t(all_run_stats.move_board_counter) / elapsed << std::endl;
        std::cout << indent << "Eval board:     \t" << r_t(all_run_stats.eval_board_counter) / elapsed << std::endl;
        std::cout << indent << "Update weights: \t" << r_t(all_run_stats.update_weights_counter) / elapsed << std::endl;
        std::cout << indent << "Eval moves:     \t" << r_t(all_run_stats.eval_moves_counter) / elapsed << std::endl;

        std::cout << "Overall training stats per game (" << all_training_stats.game_counter << " games)" << std::endl;
        std::cout << indent << "Score per game: \t" << r_t(all_training_stats.score_counter) / r_t(all_training_stats.game_counter) << std::endl;
        std::cout << indent << "Moves per game: \t" << r_t(all_training_stats.moves_counter) / r_t(all_training_stats.game_counter) << std::endl;

        std::cout << "Overall testing stats per game (" << all_testing_stats.game_counter << " games)" << std::endl;
        std::cout << indent << "Score per game: \t" << r_t(all_testing_stats.score_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Moves per game: \t" << r_t(all_testing_stats.moves_counter) / r_t(all_testing_stats.game_counter) << std::endl;

        std::cout << indent << "Max moves:      \t" << all_testing_stats.max_moves << std::endl;
        std::cout << indent << "Max score:      \t" << all_testing_stats.max_score << std::endl;
        std::cout << indent << "Max cell:       \t" << all_testing_stats.max_cell << std::endl;
        std::cout << indent << "Score A:        \t" << r_t(all_testing_stats.score_A_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score B:        \t" << r_t(all_testing_stats.score_B_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score C:        \t" << r_t(all_testing_stats.score_C_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score D:        \t" << r_t(all_testing_stats.score_D_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score E:        \t" << r_t(all_testing_stats.score_E_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score F:        \t" << r_t(all_testing_stats.score_F_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score G:        \t" << r_t(all_testing_stats.score_G_counter) / r_t(all_testing_stats.game_counter) << std::endl;
        std::cout << indent << "Score H:        \t" << r_t(all_testing_stats.score_H_counter) / r_t(all_testing_stats.game_counter) << std::endl;*/

        std::cout << "Overall run stats (" << elapsed << " us)" << std::endl;
        all_run_stats.print_board_operation_stats(elapsed);
        all_run_stats.print_eval_operation_stats(elapsed);

        std::cout << "Overall training stats (" << all_training_stats.game_counter << " games)" << std::endl;
        all_training_stats.print_average_game_stats();
        all_training_stats.print_max_game_stats();
        all_training_stats.print_score_cell_stats();

        std::cout << "Overall testing stats (" << all_testing_stats.game_counter << " games)" << std::endl;
        all_testing_stats.print_average_game_stats();
        all_testing_stats.print_max_game_stats();
        all_testing_stats.print_score_cell_stats();
    }

    std::cout << "Learning finished (" << elapsed / 1e6 << " s)" << std::endl;
    std::cout << std::endl;
}

void fixed_learn(r_t LR, u32 episodes, u32 training_games, u32 testing_games, u8 threads, NTuple &tuples) {
    learning_rate = LR;
    run_learning(episodes, training_games, testing_games, threads, tuples);
    std::string ts_str = get_time_str();
    std::cout << "Timestamp: " << ts_str << std::endl;
    save_packed_weights(ts_str, tuples);
    std::cout << std::endl;
}
