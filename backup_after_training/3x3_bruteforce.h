#pragma once

#include "board.h"

using h_t = u32;

h_t to_hash(u64 board) {
    h_t hash = 0;
    for (u8 i = 0; i < 9; ++i) {
        hash = (hash * 11) + (board & 0xFu);
        board >>= 4;
    }
    return hash;
}

h_t get_hash(const u64 board) {
    h_t hash = numeric_limits<h_t>::max();
    for (const auto &b: get_transformations<3>(board)) {
        hash = min(hash, to_hash(b));
    }
    return hash;
}

u8 highest_tile(u64 board) {
    u8 highest = 0;
    for (u8 i = 0; i < 9; ++i) {
        highest = max(highest, u8(board & 0xFu));
        board >>= 4;
    }
    return highest;
}

r_t get_max_eval(const vector<pair<r_t, r_t >> &evals) {
    if (evals.empty()) { return 0; }
    r_t max_eval = 0;
    for (const auto &[eval, prob]: evals) {
        max_eval = max(max_eval, eval);
    }
    return max_eval;
}

r_t get_min_eval(const vector<pair<r_t, r_t >> &evals) {
    if (evals.empty()) { return 0; }
    r_t min_eval = numeric_limits<r_t>::max();
    for (const auto &[eval, prob]: evals) {
        min_eval = min(min_eval, eval);
    }
    return min_eval;
}

r_t get_exp_eval(const vector<pair<r_t, r_t >> &evals) {
    if (evals.empty()) { return 0; }
    r_t exp_eval = 0;
    for (const auto &[eval, prob]: evals) {
        exp_eval += eval * prob;
    }
    return exp_eval;
}

r_t get_eval(const vector<pair<r_t, r_t >> &evals, const u8 mode) {
    switch (mode) {
        case 0:
            return get_max_eval(evals);
        case 1:
            return get_min_eval(evals);
        case 2:
            return get_exp_eval(evals);
        default:
            return 0;
    }
}

struct Bruteforce {
    u8 move_mode, fill_mode, metric;

    unordered_map<h_t, r_t> state_cache;
    unordered_map<h_t, r_t> afterstate_cache;

    Bruteforce(u8 move_mode, u8 fill_mode, u8 metric) : move_mode(move_mode), fill_mode(fill_mode), metric(metric) {
        size_t size = 10000000;
        state_cache.reserve(size);
        afterstate_cache.reserve(size);
    }

    ~Bruteforce() {
        state_cache.clear();
        afterstate_cache.clear();
    }

    r_t eval_state(u64 state) {
        h_t hash = get_hash(state);
        if (state_cache.count(hash)) { return state_cache[hash]; }
        vector<pair<r_t, r_t>> evals;
        for (const Dir d: DIRS) {
            u64 afterstate = moved_board<3>(state, d);
            if (afterstate == state) { continue; }
            r_t curr_eval = eval_afterstate(afterstate, 1);
            switch (metric) {
                case 0:
                    curr_eval += 1;
                    break;
                case 1:
                    curr_eval += r_t(get_reward<3>(state, d));
                    break;
                default:
                    curr_eval = highest_tile(afterstate) >= metric ? 1 : curr_eval;
                    break;
            }
            evals.emplace_back(curr_eval, 1);
        }
        for (auto &[eval, prob]: evals) {
            prob /= r_t(evals.size());
        }
        r_t eval = get_eval(evals, move_mode);
        state_cache[hash] = eval;
        return eval;
    }

    r_t eval_afterstate(u64 afterstate, u8 depth) {
        if (depth == 0) { return eval_state(afterstate); }
        h_t hash = get_hash(afterstate);
        if (afterstate_cache.count(hash)) { return afterstate_cache[hash]; }
        vector<pair<r_t, r_t>> evals;

        u64 empty = empty_mask<3>(afterstate);
        u8 empty_count = popcnt(empty);
        if (empty_count == 0) { return 0; }
        u64 mask = 1;
        for (u8 i = 0; i < 9; ++i) {
            if (empty & mask) {
                for (const auto &[shift, prob]: SHIFTS) {
                    r_t curr_eval = eval_afterstate(afterstate | (mask << shift), depth - 1);
                    evals.emplace_back(curr_eval, prob);
                }
            }
            mask <<= 4;
        }
        for (auto &[eval, prob]: evals) {
            prob /= r_t(empty_count);
        }
        r_t eval = get_eval(evals, fill_mode);
        afterstate_cache[hash] = eval;
        return eval;
    }

    r_t eval_game() {
        state_cache.clear();
        afterstate_cache.clear();
        return eval_afterstate(0, 2);
    }
};
