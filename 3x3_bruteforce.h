#pragma once

#include "assets.h"
#include "board.h"

unordered_map<u32, float> state_cache;
unordered_map<u32, float> afterstate_cache;

u32 to_hash(u64 board) {
    u32 hash = 0;
    for (u8 i = 0; i < 9; ++i) {
        hash = (hash * 11) + (board & 0xFu);
        board >>= 4;
    }
    return hash;
}

u32 get_hash(const u64 board) {
    u32 hash = numeric_limits<u32>::max();
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

float get_max_eval(const vector<pair<float, float>> &evals) {
    if (evals.empty()) { return 0; }
    float max_eval = 0;
    for (const auto &[eval, prob]: evals) {
        max_eval = max(max_eval, eval);
    }
    return max_eval;
}

float get_min_eval(const vector<pair<float, float>> &evals) {
    if (evals.empty()) { return 0; }
    float min_eval = numeric_limits<float>::max();
    for (const auto &[eval, prob]: evals) {
        min_eval = min(min_eval, eval);
    }
    return min_eval;
}

float get_exp_eval(const vector<pair<float, float>> &evals) {
    if (evals.empty()) { return 0; }
    float exp_eval = 0;
    for (const auto &[eval, prob]: evals) {
        exp_eval += eval * prob;
    }
    return exp_eval;
}

float get_eval(const vector<pair<float, float>> &evals, u8 mode) {
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

float eval_afterstate(u64 afterstate, u8 depth, u8 move_mode, u8 fill_mode, u8 metric);

float eval_state(u64 state, u8 move_mode, u8 fill_mode, u8 metric) {
    u32 hash = get_hash(state);
    if (state_cache.count(hash)) { return state_cache[hash]; }
    vector<pair<float, float>> evals;
    for (const Dir d: Dirs) {
        u64 afterstate = moved_board<3>(state, d);
        if (afterstate == state) { continue; }
        float curr_eval = eval_afterstate(afterstate, 1, move_mode, fill_mode, metric);
        switch (metric) {
            case 0:
                curr_eval += 1;
                break;
            case 1:
                curr_eval += get_reward<3>(state, d);
                break;
            default:
                curr_eval = highest_tile(afterstate) == metric ? 1 : curr_eval;
                break;
        }
        evals.emplace_back(curr_eval, 1);
    }
    for (auto &[eval, prob]: evals) {
        prob /= float(evals.size());
    }
    float eval = get_eval(evals, move_mode);
    state_cache[hash] = eval;
    return eval;
}

float eval_afterstate(u64 afterstate, u8 depth, u8 move_mode, u8 fill_mode, u8 metric) {
    if (depth == 0) { return eval_state(afterstate, move_mode, fill_mode, metric); }
    u32 hash = get_hash(afterstate);
    if (afterstate_cache.count(hash)) { return afterstate_cache[hash]; }
    vector<pair<float, float>> evals;

    u64 empty = empty_mask<3>(afterstate);
    u8 empty_count = popcnt(empty);
    if (empty_count == 0) { return 0; }
    u64 mask = 1;
    for (u8 i = 0; i < 9; ++i) {
        if (empty & mask) {
            for (const auto &[shift, prob]: {make_pair(0, 0.9), make_pair(1, 0.1)}) {
                float curr_eval = eval_afterstate(
                        afterstate | (mask << shift), depth - 1,
                        move_mode, fill_mode, metric);
                evals.emplace_back(curr_eval, prob);
            }
        }
        mask <<= 4;
    }
    for (auto &[eval, prob]: evals) {
        prob /= float(empty_count);
    }
    float eval = get_eval(evals, fill_mode);
    afterstate_cache[hash] = eval;
    return eval;
}

float eval_game(u8 move_mode, u8 fill_mode, u8 metric) {
    state_cache.clear();
    afterstate_cache.clear();
    return eval_afterstate(0, 2, move_mode, fill_mode, metric);
}