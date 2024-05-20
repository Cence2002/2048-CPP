#pragma once

#include "tuple.h"

inline r_t add_weights(const b_t board, const NTuple &tuples) {
    //++run_stats.eval_board_counter;
    r_t sum = 0;
    for (const auto &b: board.get_transformations()) {
        for (const auto &t: tuples) { sum += t[pext(b.get_bits(), t.mask)]; }
    }
    return sum;
}

inline r_t update_weights(const b_t board, const r_t gradient, NTuple &tuples) {
    //++run_stats.update_weights_counter;
    r_t sum = 0;
    for (const auto &b: board.get_transformations()) {
        for (auto &t: tuples) { sum += (t[pext(b.get_bits(), t.mask)] += gradient); }
    }
    return sum;
}

inline Eval eval_state(const b_t state, const NTuple &tuples) {
    //++run_stats.eval_moves_counter;
    Eval best{};
    for (const Dir dir: DIRS) {
        const b_t afterstate = state.slid(dir);
        if (afterstate == state) { continue; }
        const s_t reward = state.get_reward(dir);
        const r_t eval = r_t(reward) + add_weights(afterstate, tuples);
        if (best.dir == None || eval > best.eval) {
            best = {dir, eval, reward, afterstate};
        }
    }
    return best;
}

inline u64 cnt_state, cnt_afterstate;

inline r_t expectimax_afterstate(b_t afterstate, u8 depth, r_t max_prob, u64 &evals, const NTuple &tuples);

inline Eval expectimax_state(const b_t state, const u8 depth, const r_t max_prob, u64 &evals, const NTuple &tuples) {
    Eval best{};
    if (evals == 0) { return best; }
    cnt_state++;
    for (const Dir dir: DIRS) {
        const b_t afterstate = state.slid(dir);
        if (afterstate == state) { continue; }
        r_t eval = expectimax_afterstate(afterstate, depth, max_prob, evals, tuples);
        if (evals == 0) { return best; }
        const s_t reward = state.get_reward(dir);
        eval += r_t(reward);
        if (best.dir == None || eval > best.eval) {
            best = {dir, eval, reward, afterstate};
        }
    }
    best.eval += 1;
    return best;
}

inline r_t expectimax_afterstate(const b_t afterstate, u8 depth, const r_t max_prob, u64 &evals, const NTuple &tuples) {
    if (evals == 0) { return 0; }
    if (depth == 0 || max_prob < 1) {
        --evals;
        return std::max(r_t(0), add_weights(afterstate, tuples));
    }
    cnt_afterstate++;
    --depth;
    const u64 empty_mask = afterstate.empty_mask();
    const u8 empty_count = b_t::empty_count(empty_mask);
    u64 mask = 1;
    r_t sum = 0;
    r_t significance = 0;
    for (u8 i = 0; i < empty_count; mask <<= 4) {
        if (empty_mask & mask) {
            for (const auto &[shift, prob]: SHIFTS) {
                if (prob * max_prob < 1) {
                    significance += prob;
                    continue;
                }
                sum += prob * expectimax_state(afterstate | (mask << shift),
                                               depth,
                                               max_prob * prob,
                                               evals,
                                               tuples).eval;
                if (evals == 0) { return 0; }
            }
            ++i;
        }
    }
    if (significance > 0) {
        sum += significance * add_weights(afterstate, tuples);
    }
    return sum * RECIPROCALS[empty_count];
}

inline Eval expectimax_limited_depth_prob(const b_t board, const u8 depth, const r_t prob, const NTuple &tuples) {
    u64 evals = std::numeric_limits<u64>::max();
    return expectimax_state(board, depth, r_t(1) / prob, evals, tuples);
}

inline r_t get_prob(const u8 cnt1, const u8 cnt2) {
    return pow(r_t(0.9), r_t(cnt1)) * pow(r_t(0.1), r_t(cnt2));
}

inline r_t get_ratio(const u64 cnt1, const u64 cnt2) {
    return binomial(cnt1 + cnt2, cnt1) * get_prob(cnt1, cnt2);
}

inline r_t get_min_prob(const u8 depth, const r_t min_ratio) {
    u8 cnt1 = 0;
    u8 cnt2 = depth;
    while (cnt1 < depth && cnt2 > 0 && get_ratio(cnt1, cnt2) < min_ratio) {
        ++cnt1;
        --cnt2;
    }
    r_t max_prob = get_prob(cnt1, cnt2);
    return max_prob * r_t(0.5);
}

//threshold = throw away at most that portion of probabilities in total
inline r_t get_max_prob(const u8 depth, r_t threshold) {
    u8 cnt1 = 0;
    u8 cnt2 = depth;
    while (cnt1 < depth && cnt2 > 0) {
        const r_t ratio = get_ratio(cnt1, cnt2);
        if (ratio > threshold) { break; }
        threshold -= ratio;
        ++cnt1;
        --cnt2;
    }
    return r_t(2) / get_prob(cnt1, cnt2);
}

inline Eval expectimax_limited_evals(const b_t board, u64 evals, const r_t threshold, const NTuple &tuples) {
    Eval best{};
    for (u8 depth = 0; depth < 64; ++depth) {
        const u64 evals_backup = evals;
        const Eval eval = expectimax_state(board, depth, get_max_prob(depth, threshold), evals, tuples);
        if (evals == 0) { break; }
        best = eval;
    }
    return best;
}

//threshold = smallest min below which it won't downgrade
inline b_t downgraded(b_t board, const u8 threshold = 15) {
    u32 used = 0;
    u8 highest = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = board.get_cell(i);
        used |= u32(1) << cell;
        if (cell > highest) { highest = cell; }
    }
    if (highest < threshold) { return board; }
    u8 missing = 0;
    for (u8 i = 1; i < highest; ++i) {
        if (((used >> i) & 1) == 0) {
            missing = i;
        }
    }
    if (missing < 7) { return board; }
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = board.get_cell(i);
        if (cell > missing) {
            board.set_cell(i, cell - 1);
        }
    }
    return board;
}

//threshold = smallest number that can be modified
inline b_t upgraded_old(const b_t board, const u8 threshold) {
    std::array<u8, 16> counts{};
    counts.fill(0);
    for (u8 i = 0; i < 16; ++i) {
        ++counts[board.get_cell(i)];
    }
    u8 max_double = 0;
    for (u8 i = 1; i < 16; ++i) {
        if (counts[i] > 1) {
            max_double = i;
        }
    }
    if (counts[15] == 0 || max_double > 10) { return board; }
    std::array<u8, 16> new_cells{};
    for (u8 i = 0; i < 16; ++i) {
        if (i < threshold) {
            new_cells[i] = i;
            continue;
        }
        new_cells[i] = new_cells[i - 1];
        if (counts[i] > 0) { ++new_cells[i]; }
    }
    b_t upgraded = 0;
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = board.get_cell(i);
        upgraded.set_cell(i, new_cells[cell]);
    }
    return upgraded;
}

inline b_t upgraded(const b_t board) {
    std::array<u8, 16> counts{};
    for (u8 i = 0; i < 16; ++i) {
        ++counts[board.get_cell(i)];
    }
    if (counts[15] == 0 || counts[14] > 0) { return board; }
    u8 max_double = 0;
    for (u8 i = 1; i < 16; ++i) {
        if (counts[i] > 1) {
            max_double = i;
        }
    }
    if (max_double > 10) { return board; }
    b_t upgraded = board;
    for (u8 i = 0; i < 16; ++i) {
        const u8 cell = board.get_cell(i);
        if (cell < 15) { continue; }
        upgraded.set_cell(i, 14);
    }
    return upgraded;
}
