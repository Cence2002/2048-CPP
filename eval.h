#pragma once

#include "tuple.h"

template<u8 N>
inline float eval_board(const u64 board) {
    float sum = 0;
    for (const auto &b: get_transformations<N>(board)) {
        for (const auto &t: tuples) {
            sum += t.weights[pext(b, t.mask)];
        }
    }
    return sum;
}

template<u8 N>
inline float update_weights(const u64 board, const float gradient) {
    float sum = 0;
    for (const auto &b: get_transformations<N>(board)) {
        for (auto &t: tuples) {
            sum += (t.weights[pext(b, t.mask)] += gradient);
        }
    }
    return sum;
}

struct dera {
    Dir d;
    float eval;
    u32 reward;
    u64 afterstate;
};

template<u8 N>
dera eval_moves(u64 state) {
    dera best = {None, 0, 0, 0};
    for (const Dir d: Dirs) {
        u64 afterstate = moved_board<N>(state, d);
        if (afterstate == state) { continue; }
        u32 reward = get_reward<N>(state, d);
        float eval = float(reward) + eval_board<N>(afterstate);
        if (best.d == None || eval > best.eval) {
            best = {d, eval, reward, afterstate};
        }
    }
    return best;
}
