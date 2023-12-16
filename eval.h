#pragma once

#include "tuple.h"

template<u8 N>
inline r_t eval_board(const b_t board) {
    ++run_stats.eval_board_counter;
    r_t sum = 0;
    for (const auto &b: get_transformations<N>(board)) {
        if constexpr (N == 4) {
            for (const auto &t: tuples_4) { sum += t.weights[pext(b, t.mask)]; }
        } else {
            for (const auto &t: tuples_3) { sum += t.weights[pext(b, t.mask)]; }
        }
    }
    return sum;
}

template<u8 N>
inline r_t update_weights(const b_t board, const r_t gradient) {
    ++run_stats.update_weights_counter;
    r_t sum = 0;
    for (const auto &b: get_transformations<N>(board)) {
        if constexpr (N == 4) {
            for (auto &t: tuples_4) { sum += (t.weights[pext(b, t.mask)] += gradient); }
        } else {
            for (auto &t: tuples_3) { sum += (t.weights[pext(b, t.mask)] += gradient); }
        }
    }
    return sum;
}

template<u8 N>
inline Eval eval_moves(const b_t state) {
    ++run_stats.eval_moves_counter;
    Eval best = {None, 0, 0, 0};
    for (const Dir dir: DIRS) {
        b_t afterstate = moved_board<N>(state, dir);
        if (afterstate == state) { continue; }
        s_t reward = get_reward<N>(state, dir);
        r_t eval = r_t(reward) + eval_board<N>(afterstate);
        if (best.dir == None || eval > best.eval) {
            best = {dir, eval, reward, afterstate};
        }
    }
    return best;
}

template<u8 N>
r_t eval_afterstate(b_t afterstate, u8 max_depth, r_t max_prob, u64 &max_states);

template<u8 N>
Eval eval_state(const b_t state, const u8 max_depth, const r_t max_prob, u64 &max_states) {
    if (max_states) { --max_states; }
    Eval best = {None, 0, 0, 0};
    for (const Dir dir: DIRS) {
        b_t afterstate = moved_board<N>(state, dir);
        if (afterstate == state) { continue; }
        s_t reward = get_reward<N>(state, dir);
        r_t eval = reward + eval_afterstate<N>(afterstate, max_depth, max_prob, max_states);
        if (best.dir == None || eval > best.eval) {
            best = {dir, eval, reward, afterstate};
        }
    }
    //TODO maybe turn on
    //if (best.dir == None) { best.eval = eval_board<N>(state) - 1e7; }
    return best;
}

template<u8 N>
r_t eval_afterstate(const b_t afterstate, const u8 max_depth, const r_t max_prob, const u64 &max_states) {
    if (max_depth == 0 || max_prob < 1 || max_states == 0) {
        return eval_board<N>(afterstate);
    }
    /*const u8 empty = count_empty<N>(afterstate);
    max_prob /= r_t(empty);
    r_t avg_eval = 0;
    b_t mask = 0xFu;
    b_t val = 0x1u;
    for (u8 i = 0; i < N * N; ++i) {
        if ((afterstate & mask) == 0) {
            avg_eval += r_t(0.9) * eval_state<N>(
                    afterstate | val, max_depth, max_prob * r_t(0.9), max_moves).e;
            avg_eval += r_t(0.1) * eval_state<N>(
                    afterstate | (val << 1), max_depth, max_prob * r_t(0.1), max_moves).e;
        }
        mask <<= 4;
        val <<= 4;
    }
    return avg_eval / r_t(empty);*/
    b_t empty = empty_mask<N>(afterstate);
    u8 count = popcnt(empty);
    b_t mask = 1;
    r_t sum = 0;
    while (count) {
        if (empty & mask) {
            for (const auto &[shift, prob]: SHIFTS) {
                sum += prob * eval_state<N>(
                        afterstate | (mask << shift),
                        max_depth - 1,
                        max_prob * prob,
                        max_states).eval;
            }
            --count;
        }
        mask <<= 4;
    }
    return sum / r_t(popcnt(empty));
}

template<u8 N>
Eval limited_depth_prob_player(b_t board, u8 depth, r_t prob) {
    u64 states = numeric_limits<u64>::max();
    return eval_state<N>(board, depth, prob, states);
}

template<u8 N>
Eval limited_states_player(b_t board, u64 states) {
    Eval best = {None, 0, 0, 0};
    for (u8 depth = 0; depth < 100; ++depth) {
        Eval eval = eval_state<N>(board, depth, r_t(E(depth + 4)), states);
        if (states == 0) { break; }
        best = eval;
    }
    return best;
}
