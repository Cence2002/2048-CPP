#pragma once

#include "tuple.h"

#define FIXNEGATIVE 0
#define DOWNGRADE 0

u64 cnt_adds = 0;

inline r_t add_weights(const u64 board, const NTuple &tuples) {
    //++run_stats.eval_board_counter;
    //cnt_adds++;
    r_t sum = 0;
    for (const auto &b: get_transformations(board)) {
        for (const auto &t: tuples) { sum += t[pext(b, t.mask)]; }
    }
    return sum;
}

inline r_t update_weights(const u64 board, const r_t gradient, NTuple &tuples) {
    //++run_stats.update_weights_counter;
    r_t sum = 0;
    for (const auto &b: get_transformations(board)) {
        for (auto &t: tuples) { sum += (t[pext(b, t.mask)] += gradient); }
    }
    return sum;
}

inline Eval eval_state(const u64 state, const NTuple &tuples) {
    //++run_stats.eval_moves_counter;
    Eval best = Eval::None();
    for (const Dir dir: DIRS) {
        u64 afterstate = moved_board(state, dir);
        if (afterstate == state) { continue; }
        s_t reward = get_reward(state, dir);
        r_t eval = r_t(reward) + add_weights(afterstate, tuples);
        if (best.dir == None || eval > best.eval) {
            best = {dir, eval, reward, afterstate};
        }
    }
    return best;
}

u64 cnt_state, cnt_afterstate;

r_t expectimax_afterstate(u64 afterstate, u8 max_depth, r_t max_prob, u64 &max_states, const NTuple &tuples);

Eval expectimax_state(const u64 state, const u8 max_depth, const r_t max_prob, u64 &max_states, const NTuple &tuples) {
    Eval best = Eval::None();
    if (max_states == 0) { return best; }
    --max_states;
    if (max_states == 0) { return best; }
    //cnt_state++;
    for (const Dir dir: DIRS) {
        const u64 afterstate = moved_board(state, dir);
        if (afterstate == state) { continue; }
        r_t eval = expectimax_afterstate(afterstate, max_depth, max_prob, max_states, tuples);
        if (max_states == 0) { return best; }
        const s_t reward = get_reward(state, dir);
        eval += r_t(reward);
        if (best.dir == None || eval > best.eval) {
            best = {dir, eval, reward, afterstate};
        }
    }
    //TODO maybe turn on
    //if (best.dir == None) { best.eval = eval_board<N>(state) - 1e7; }
    if (FIXNEGATIVE) { best.eval += 1; }
    return best;
}

unordered_map<r_t, u64> cnt_probs;
unordered_map<u8, u64> cnt_depths;

//TODO take evaluated afterstates partially if the children have too small probs to reduce the size of the tree and
//detect small probabilities sooner
r_t expectimax_afterstate(const u64 afterstate, const u8 max_depth, const r_t max_prob, u64 &max_states, const NTuple &tuples) {
    if (max_states == 0) { return 0; }
    /*if (cnt_depths.find(max_depth) == cnt_depths.end()) { cnt_depths[max_depth] = 0; }
    cnt_depths[max_depth]++;
    if(max_prob <= 1) {
        cnt++;
    }*/
    if (max_depth == 0 || max_prob <= 1) {
        /*if (cnt_probs.find(max_prob) == cnt_probs.end()) { cnt_probs[max_prob] = 0; }
        cnt_probs[max_prob]++;*/
        if (FIXNEGATIVE) {
            return max(r_t(0), add_weights(afterstate, tuples));
        } else {
            return add_weights(afterstate, tuples);
        }
    }
    const u64 empty = empty_mask(afterstate);
    const u8 empty_count = popcnt(empty);
    u64 mask = 1;
    r_t sum = 0;
    r_t significance = 0;
    //cnt_afterstate++;
    for (u8 i = 0; i < empty_count; mask <<= 4) {
        if (empty & mask) {
            for (const auto &[shift, prob]: SHIFTS) {
                if (prob * max_prob <= 1) {
                    significance += prob;
                    continue;
                }
                sum += prob * expectimax_state(
                        afterstate | (mask << shift),
                        max_depth - 1,
                        max_prob * prob,
                        max_states,
                        tuples).eval;
                if (max_states == 0) { return 0; }
            }
            ++i;
        }
    }
    if (significance > 0) {
        sum += significance * add_weights(afterstate, tuples);
    }
    return sum / r_t(empty_count);
}

Eval expectimax_limited_depth_prob(const u64 board, const u8 depth, const r_t prob, const NTuple &tuples) {
    u64 states = numeric_limits<u64>::max();
    return expectimax_state(board, depth, prob, states, tuples);
}

r_t get_prob(const u8 cnt1, const u8 cnt2) {
    return pow(0.9f, r_t(cnt1)) * pow(0.1f, r_t(cnt2));
}

r_t get_ratio(const u64 cnt1, const u64 cnt2) {
    return binomial(cnt1 + cnt2, cnt1) * get_prob(cnt1, cnt2);
}

r_t get_min_prob(const u8 depth, const r_t min_ratio) {
    u8 cnt1 = 0;
    u8 cnt2 = depth;
    while (cnt1 < depth && cnt2 > 0 && get_ratio(cnt1, cnt2) < min_ratio) {
        ++cnt1;
        --cnt2;
    }
    r_t max_prob = get_prob(cnt1, cnt2);
    //denominator D can be anything such that 1<D<9
    //because next smallest prob is max_prob/9
    return max_prob / 2;
}

Eval expectimax_limited_states(const u64 board, u64 states, const r_t min_ratio, const NTuple &tuples) { // const r_t threshold = 0.05
    Eval best = Eval::None();
    for (u8 depth = 0; depth < 100; ++depth) {
        //TODO replace
        //Eval eval = expectimax_state(board, depth, r_t(E(depth + 4)), states, tuples);
        //Eval eval = expectimax_state(board, depth, 1.0f / get_min_prob(depth, min_ratio), states, tuples);
        const Eval eval = expectimax_state(board, depth, r_t(1) / get_min_prob(depth, min_ratio), states, tuples);
        if (states == 0) { break; }
        best = eval;
    }
    return best;
}

u64 downgraded(u64 board, const u8 threshold = 15) {
    u32 used = 0;
    u8 highest = 0;
    for (u8 i = 0; i < 16; ++i) {
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
    for (u8 i = 0; i < 16; ++i) {
        u8 cell = (board >> (i * 4)) & 0xFu;
        if (cell > missing) {
            board &= ~(u64(0xF) << (i * 4));
            board |= u64(cell - 1) << (i * 4);
        }
    }
    return board;
}
