#pragma once

#include "eval.h"

const r_t tuple_init = 1e5;
r_t learning_rate = 0;

template<u8 N>
void training_episode() {
    vector<r_t> evals;
    vector<s_t> rewards;
    vector<b_t> afterstates;
    s_t score = 0;

    b_t board = 0;
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

    r_t scaled_learning_rate = learning_rate;
    if constexpr (N == 4) {
        scaled_learning_rate /= 8 * tuples_4.size();
    } else {
        scaled_learning_rate /= 8 * tuples_3.size();
    }

    float target = 0;
    for (u32 t = moves - 1; t < moves; --t) {
        const r_t error = target - eval_board<N>(afterstates[t]);
        target = r_t(rewards[t]) + update_weights<N>(afterstates[t], error * scaled_learning_rate);
    }

    training_stats.update_board_stats(board, score, moves);
}

template<u8 N>
s_t testing_episode() {
    b_t board = 0;
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

    testing_stats.update_board_stats(board, score, moves);

    return score;
}

/*template<u8 N>
float learn(u32 training_episodes, u32 testing_episodes) {
    cnt = 0;
    auto start = time_now();
    for (u32 i = 0; i < training_episodes; ++i) {
        training_episode<N>();
    }
    cout << "Training: " << training_episodes << endl;
    cout << "cnt = " << cnt << endl;
    cout << "time = " << time_since(start) << endl;
    cout << "MMPS = " << float(cnt) / time_since(start) << endl;
    cout << endl;

    cnt = 0;
    start = time_now();
    float sum = 0;
    for (u32 i = 0; i < testing_episodes; ++i) {
        sum += testing_episode<N>();
    }
    cout << "Testing: " << testing_episodes << endl;
    cout << "cnt = " << cnt << endl;
    cout << "time = " << time_since(start) << endl;
    cout << "MMPS = " << float(cnt) / time_since(start) << endl;
    cout << endl;
    return sum / testing_episodes;
}*/

template<u8 N>
void run_training_episodes(u32 games) {
    srand(42);
    cout << "Training started (" << games << " games)" << endl;

    run_stats = {};
    training_stats = {};
    auto start = time_now();
    for (u32 i = 0; i < games; i++) { training_episode<N>(); }
    r_t elapsed = r_t(time_since(start));

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

    cout << "Training finished (" << r_t(time_since(start)) / 1e6 << " s)" << endl;
}

template<u8 N>
void run_testing_episodes(u32 games) {
    srand(42);
    cout << "Testing started (" << games << " games)" << endl;

    run_stats = {};
    testing_stats = {};
    auto start = time_now();
    for (u32 i = 0; i < games; i++) { testing_episode<N>(); }
    r_t elapsed = r_t(time_since(start));

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

    cout << "Testing finished (" << r_t(time_since(start)) / 1e6 << " s)" << endl;
}

template<u8 N>
void run_learning(u32 episodes, u32 training_games, u32 testing_games) {
    cout << "Learning started (" << episodes << " episodes, LR=" << learning_rate << ")" << endl;

    run_stats_t all_run_stats = {};
    game_stats_t all_training_stats = {};
    game_stats_t all_testing_stats = {};

    run_testing_episodes<N>(testing_games);
    cout << endl;

    auto start = time_now();
    for (u32 i = 0; i < episodes; i++) {
        cout << "Episode " << i + 1 << endl;

        run_training_episodes<N>(training_games);
        all_training_stats.append_stats(training_stats);
        all_run_stats.append_stats(run_stats);

        run_testing_episodes<N>(testing_games);
        all_run_stats.append_stats(run_stats);
        all_testing_stats.append_stats(testing_stats);

        cout << endl;
    }
    r_t elapsed = r_t(time_since(start));

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