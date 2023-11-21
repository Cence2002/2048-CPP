#pragma once

#include "eval.h"

float learning_rate = 0.1;

template<u8 N>
void training_episode() {
    vector<float> evals;
    vector<u32> rewards;
    vector<u64> afterstates;
    u32 score = 0;

    u64 board = 0;
    fill_board<N>(board);
    fill_board<N>(board);
    while (true) {
        const auto [d, eval, reward, afterstate]
                = eval_moves<N>(board);
        if (d == None) { break; }
        evals.push_back(eval);
        rewards.push_back(reward);
        afterstates.push_back(afterstate);
        score += reward;
        ++cnt;

        board = afterstate;
        fill_board<N>(board);
    }
    const u32 moves = evals.size();


    const float scaled_learning_rate = learning_rate / (8 * tuples.size());

    float target = 0;
    for (u32 t = moves - 1; t < moves; --t) {
        const float error = target - eval_board<N>(afterstates[t]);
        target = float(rewards[t]) + update_weights<N>(afterstates[t], error * scaled_learning_rate);
    }
}

template<u8 N>
float testing_episode() {
    u32 score = 0;

    u64 board = 0;
    fill_board<N>(board);
    fill_board<N>(board);
    while (true) {
        const auto [d, eval, reward, afterstate]
                = eval_moves<N>(board);
        if (d == None) { break; }
        score += reward;
        ++cnt;

        board = afterstate;
        fill_board<N>(board);
    }

    return score;
}

template<u8 N>
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
}
