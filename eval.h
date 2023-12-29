#pragma once

#include "tuple.h"

#define FIXNEGATIVE 0
#define DOWNGRADE 0

template<u8 N>
inline r_t eval_board(const u64 board) {
    ++run_stats.eval_board_counter;
    r_t sum = 0;
    for (const auto &b: get_transformations<N>(board)) {
        if constexpr (N == 4) {
            //TODO replace
            //for (const auto &t: tuples_4) { sum += t.weights[pext(b, t.mask)]; }
            for (u8 i = 0; i < tuples_size_4; ++i) { sum += tuples_4[i].weights[pext(b, tuples_4[i].mask)]; }
        } else {
            //TODO replace
            //for (const auto &t: tuples_3) { sum += t.weights[pext(b, t.mask)]; }
            for (u8 i = 0; i < tuples_size_3; ++i) { sum += tuples_3[i].weights[pext(b, tuples_3[i].mask)]; }
        }
    }
    return sum;
}

template<u8 N>
inline r_t update_weights(const u64 board, const r_t gradient) {
    ++run_stats.update_weights_counter;
    r_t sum = 0;
    for (const auto &b: get_transformations<N>(board)) {
        if constexpr (N == 4) {
            //TODO replace
            //for (auto &t: tuples_4) { sum += (t.weights[pext(b, t.mask)] += gradient); }
            //for (auto &t: tuples_4) { t.weights[pext(b, t.mask)] += gradient; }
            //for (u8 i = 0; i < tuples_size_4; ++i) { tuples_4[i].weights[pext(b, tuples_4[i].mask)] += gradient; }
            for (u8 i = 0; i < tuples_size_4; ++i) {
                sum += (tuples_4[i].weights[pext(b, tuples_4[i].mask)] += gradient);
            }
        } else {
            //TODO replace
            //for (auto &t: tuples_3) { sum += (t.weights[pext(b, t.mask)] += gradient); }
            //for (auto &t: tuples_3) { t.weights[pext(b, t.mask)] += gradient; }
            for (u8 i = 0; i < tuples_size_3; ++i) {
                sum += (tuples_3[i].weights[pext(b, tuples_3[i].mask)] += gradient);
            }
        }
    }
    return sum;
}

template<u8 N>
inline Eval eval_moves(const u64 state) {
    ++run_stats.eval_moves_counter;
    Eval best = {None, 0, 0, 0};
    for (const Dir dir: DIRS) {
        u64 afterstate = moved_board<N>(state, dir);
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
r_t eval_afterstate(u64 afterstate, u8 max_depth, r_t max_prob, u64 &max_states);

template<u8 N>
Eval eval_state(const u64 state, const u8 max_depth, const r_t max_prob, u64 &max_states) {
    if (max_states) { --max_states; }
    Eval best = {None, 0, 0, 0};
    for (const Dir dir: DIRS) {
        u64 afterstate = moved_board<N>(state, dir);
        if (afterstate == state) { continue; }
        const s_t reward = get_reward<N>(state, dir);
        r_t eval = reward + eval_afterstate<N>(afterstate, max_depth, max_prob, max_states);
        if (best.dir == None || eval > best.eval) {
            best = {dir, eval, reward, afterstate};
        }
    }
    //TODO maybe turn on
    //if (best.dir == None) { best.eval = eval_board<N>(state) - 1e7; }
    if (FIXNEGATIVE) { best.eval += 1; }
    return best;
}

template<u8 N>
r_t eval_afterstate(const u64 afterstate, const u8 max_depth, const r_t max_prob, const u64 &max_states) {
    if (max_depth == 0 || max_prob < 1 || max_states == 0) {
        if (FIXNEGATIVE) {
            return max(0, eval_board<N>(afterstate));
        } else {
            return eval_board<N>(afterstate);
        }
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
    u64 empty = empty_mask<N>(afterstate);
    const u8 count = popcnt(empty);
    u64 mask = 1;
    r_t sum = 0;
    for (u8 i = 0; i < count;) {
        if (empty & mask) {
            for (const auto &[shift, prob]: SHIFTS) {
                sum += prob * eval_state<N>(
                        afterstate | (mask << shift),
                        max_depth - 1,
                        max_prob * prob,
                        max_states).eval;
            }
            ++i;
        }
        mask <<= 4;
    }
    return sum / r_t(count);
}

template<u8 N>
Eval limited_depth_prob_player(u64 board, u8 depth, r_t prob) {
    u64 states = numeric_limits<u64>::max();
    return eval_state<N>(board, depth, prob, states);
}

template<u8 N>
Eval limited_states_player(u64 board, u64 states) {
    Eval best = {None, 0, 0, 0};
    for (u8 depth = 0; depth < 100; ++depth) {
        Eval eval = eval_state<N>(board, depth, r_t(E(depth + 4)), states);
        if (states == 0) { break; }
        best = eval;
    }
    return best;
}

template<u8 N>
u64 downgraded(u64 board, const u8 threshold = 15) {
    u32 used = 0;
    u8 highest = 0;
    for (u8 i = 0; i < N * N; ++i) {
        u8 cell = (board >> (i * 4)) & 0xFu;
        used |= u32(1) << cell;
        if (cell > highest) { highest = cell; }
    }
    if (highest < threshold) { return board; }
    u8 missing = 0;
    for (u8 i = 1; i < highest; ++i) {
        if ((used & (u32(1) << i)) == 0) {
            missing = i;
        }
    }
    //TODO maybe try (missing < 5) or other values
    if (missing < 5) { return board; }
    for (u8 i = 0; i < N * N; ++i) {
        u8 cell = (board >> (i * 4)) & 0xFu;
        if (cell > missing) {
            board &= ~(u64(0xF) << (i * 4));
            board |= u64(cell - 1) << (i * 4);
        }
    }
    return board;
}

template<u8 N>
Eval evaluate(u64 board) {
    if (DOWNGRADE) { board = downgraded<4>(board); }
    return limited_states_player<N>(board, 1000);
}
