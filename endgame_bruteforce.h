#pragma once

#include "board.h"
#include "eval.h"

/*
 brute force all possible endgame positions
 for a given board with some large and some small tiles
 maximize the probability of reaching the smallest large tile (G)
 for all possible configurations of small tiles (< G)
 where the large tiles are fixed in place

 first, find each "winning" configuration (position)
 then, for each possible configuration of small tiles
 find the probability of reaching any winning configuration
*/


// E D C B
// 6 8 9 A
// 0 2 2 1
// 2 4 1 0
// ==>
// F F F F
// 6 F F F
// 0 0 0 0
// 0 0 0 0


//take a board with some large tiles and some small tiles
//given mask is B
//ignore tiles that are not empty in B (base)
//only return hash if, on all empty tiles in B, the tile is small enough
//otherwise, return 0

//B is the base board for this endgame
//it contains 16 tiles, each 4 bits
//values can be:
// 0: small tile (< G)
// F: large tile (>= G)
// 2: goal tile (== G)
// goal tile is the one we want to merge with new tiles
// we want to maximize the probability of reaching the goal tile
// building a goal tile next to the original goal tile is a win




//TODO
//instead of 2-bit directions, store more information

//is a goal reachable certainly enough (>20%)
//if no, rely on tuple network
//if yes, what's the best move (2 bits)

//TODO
//maybe simply store the tuple value of goal positions
//that will incentivize reaching better goal positions

class Endgame {
private:
    const u64 B;
    const u8 G;
    const u64 size;
    vector<r_t> probs;

public:
    explicit Endgame(const u64 base) : B(base), G(get_G(B)), size(get_size(G, B)) {
        cout << size << endl;
        probs.resize(size, -1);
    }

    static u8 get_G(const u64 B) {
        u8 G = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 cell = (B >> (i * 4)) & 0xFu;
            if (cell == 0xFu) { continue; }
            G = max(G, cell);
        }
        return G;
    }

    static u64 get_size(const u8 G, const u64 B) {
        return power(G + 1, count_empty<4>(B));
    }

    u64 to_hash(const u64 board) const {
        u64 hash = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (board >> (i * 4)) & 0xFu;
            if (base == 0) {
                if (cell > G) { return 0; }
                hash = hash * (G + 1) + cell;
            } else {
                if (cell != base) { return 0; }
            }
        }
        return hash;
    }

    u64 from_hash(u64 hash) const {
        u64 board = 0;
        array<u8, 16> cells{};
        u8 index = 0;
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            if (base == 0) {
                cells[index++] = hash % (G + 1);
                hash /= G + 1;
            }
        }
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (base == 0) ? cells[--index] : base;
            board |= u64(cell) << (i * 4);
        }
        return board;
    }

    bool is_goal(const u64 board) const {
        for (u8 i = 0; i < 16; ++i) {
            const u8 base = (B >> (i * 4)) & 0xFu;
            const u8 cell = (board >> (i * 4)) & 0xFu;
            if (base == 0xFu && cell != 0xFu) { return false; }
            if (base == G && cell != G + 1) { return false; }
        }
        return true;
    }

    bool is_goal_state(const u64 state) const {
        for (const Dir dir: DIRS) {
            const u64 afterstate = moved_board<4>(state, dir);
            if (afterstate == state) { continue; }
            if (is_goal(afterstate)) { return true; }
        }
        return false;
    }

    r_t eval_goal_state(const u64 state) const {
        return expectimax_limited_states<4>(state, 100).eval;
    }

    void init_goals(u32 threads) {
        //TODO replace with n-tuple evaluation with not-too-large depth
        if (threads == 0) {
            for (u64 hash = 0; hash < size; ++hash) {
                const u64 board = from_hash(hash);
                if (is_goal_state(board)) {
                    probs[hash] = 1;
                    probs[hash] = eval_goal_state(board);
                }
            }
        } else {
            vector<thread> all_threads;
            for (u32 t = 0; t < threads; ++t) {
                all_threads.emplace_back([this, t, threads]() {
                    for (u64 hash = t; hash < size; hash += threads) {
                        const u64 board = from_hash(hash);
                        if (is_goal_state(board)) {
                            probs[hash] = 1;
                            probs[hash] = eval_goal_state(board);
                        }
                    }
                });
            }
            for (auto &thread: all_threads) {
                thread.join();
            }
        }
    }

    r_t eval_state(const u64 state) {
        const u64 hash = to_hash(state);
        if (hash == 0) { return 0; }
        if (probs[hash] != -1) { return probs[hash]; }
        r_t max_prob = 0;
        for (const Dir dir: DIRS) {
            const u64 afterstate = moved_board<4>(state, dir);
            if (afterstate == state) { continue; }
            r_t reward = r_t(get_reward<4>(state, dir));
            //TODO remove
            //reward *= 0;
            const r_t eval = reward + eval_afterstate(afterstate);
            max_prob = max(max_prob, eval);
        }
        probs[hash] = max_prob;
        return max_prob;
    }

    r_t eval_afterstate(const u64 afterstate) {
        if (to_hash(afterstate) == 0) { return 0; }
        u64 empty = empty_mask<4>(afterstate);
        const u8 count = popcnt(empty);
        u64 mask = 1;
        r_t sum = 0;
        for (u8 i = 0; i < count;) {
            if (empty & mask) {
                for (const auto &[shift, prob]: SHIFTS) {
                    sum += prob * eval_state(afterstate | (mask << shift));
                }
                ++i;
            }
            mask <<= 4;
        }
        return sum / r_t(count);
    }

    void eval_all_states() {
        for (u64 hash = 0; hash < size; ++hash) {
            const u64 board = from_hash(hash);
            if (to_hash(board) == 0) { continue; }
            eval_state(board);
        }
    }

    Dir best_dir(u64 board) {
        Dir best_dir = None;
        r_t best_prob = 0;
        for (const Dir dir: DIRS) {
            u64 afterstate = moved_board<4>(board, dir);
            if (afterstate == board) { continue; }
            r_t eval = eval_afterstate(afterstate);
            if (best_dir == None || eval > best_prob) {
                best_dir = dir;
                best_prob = eval;
            }
        }
        return best_dir;
    }

    void play_game() {
        u64 board = B;
        fill_board<4>(board);
        print_board<4>(board);
        while (!is_goal_state(board)) {
            Dir dir = best_dir(board);
            if (dir == None) { break; }
            board = moved_board<4>(board, dir);
            fill_board<4>(board);
            print_board<4>(board);
        }
        cout << "done" << endl;
    }

    vector<u8> pack_dirs() {
        vector<u8> packed(size / 4 + 1, 0);
        for (u64 hash = 0; hash < size; ++hash) {
            u8 dir = best_dir(from_hash(hash));
            if (dir != 0) { --dir; }
            packed[hash / 4] |= dir << ((hash % 4) * 2);
        }
        return packed;
    }

    Dir unpack_dir(const vector<u8> &packed, const u64 hash) {
        u8 byte = packed[hash / 4];
        u8 bits = (byte >> ((hash % 4) * 2)) & 0b11u;
        return Dir(bits + 1);
    }
};




/*
TO BUILD	1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16
SPACE	1	8e-9	1e-8	2e-8	2e-8	2e-8	3e-8	3e-8	4e-8	4e-8	4e-8	5e-8	5e-8	6e-8	6e-8	6e-8	7e-8
		2	2e-8	4e-8	6e-8	1e-7	1e-7	2e-7	3e-7	3e-7	4e-7	5e-7	6e-7	7e-7	8e-7	9e-7	1e-6	1e-6
		3	3e-8	1e-7	3e-7	5e-7	9e-7	1e-6	2e-6	3e-6	4e-6	5e-6	7e-6	9e-6	1e-5	1e-5	2e-5	2e-5
		4	6e-8	3e-7	1e-6	3e-6	5e-6	1e-5	2e-5	3e-5	4e-5	6e-5	8e-5	1e-4	2e-4	2e-4	3e-4	3e-4
		5	1e-7	1e-6	4e-6	1e-5	3e-5	7e-5	1e-4	2e-4	4e-4	6e-4	1e-3	1e-3	2e-3	3e-3	4e-3	6e-3
		6	3e-7	3e-6	2e-5	6e-5	2e-4	5e-4	1e-3	2e-3	4e-3	7e-3	1e-2	2e-2	3e-2	5e-2	7e-2	1e-1
		7	5e-7	9e-6	7e-5	3e-4	1e-3	3e-3	8e-3	2e-2	4e-2	8e-2	1e-1	3e-1	4e-1	7e-1	1e+0	2e+0
		8	1e-6	3e-5	3e-4	2e-3	7e-3	2e-2	7e-2	2e-1	4e-1	9e-1	2e+0	3e+0	6e+0	1e+1	2e+1	3e+1
		9	2e-6	8e-5	1e-3	8e-3	4e-2	2e-1	5e-1	2e+0	4e+0	9e+0	2e+1	4e+1	8e+1	2e+2	3e+2	5e+2
		10	4e-6	2e-4	4e-3	4e-2	2e-1	1e+0	4e+0	1e+1	4e+1	1e+2	2e+2	6e+2	1e+3	2e+3	4e+3	8e+3
		11	8e-6	7e-4	2e-2	2e-1	1e+0	8e+0	3e+1	1e+2	4e+2	1e+3	3e+3	7e+3	2e+4	3e+4	7e+4	1e+5
		12	2e-5	2e-3	7e-2	1e+0	9e+0	6e+1	3e+2	1e+3	4e+3	1e+4	4e+4	9e+4	2e+5	5e+5	1e+6	2e+6
		13	3e-5	6e-3	3e-1	5e+0	5e+1	4e+2	2e+3	1e+4	4e+4	1e+5	4e+5	1e+6	3e+6	8e+6	2e+7	4e+7
		14	7e-5	2e-2	1e+0	2e+1	3e+2	3e+3	2e+4	9e+4	4e+5	2e+6	5e+6	2e+7	4e+7	1e+8	3e+8	7e+8
		15	1e-4	6e-2	4e+0	1e+2	2e+3	2e+4	1e+5	8e+5	4e+6	2e+7	6e+7	2e+8	6e+8	2e+9	5e+9	1e+10
		16	3e-4	2e-1	2e+1	6e+2	1e+4	1e+5	1e+6	7e+6	4e+7	2e+8	7e+8	3e+9	9e+9	3e+10	7e+10	2e+11
*/

/*
TO BUILD	1		2		3		4		5		6		7		8		9		10		11		12		13		14		15		16
SPACE	1	5e-10	8e-10	1e-9	1e-9	2e-9	2e-9	2e-9	2e-9	3e-9	3e-9	3e-9	3e-9	4e-9	4e-9	4e-9	4e-9
		2	1e-9	2e-9	4e-9	6e-9	9e-9	1e-8	2e-8	2e-8	3e-8	3e-8	4e-8	4e-8	5e-8	6e-8	6e-8	7e-8
		3	2e-9	7e-9	2e-8	3e-8	5e-8	9e-8	1e-7	2e-7	3e-7	3e-7	4e-7	5e-7	7e-7	8e-7	1e-6	1e-6
		4	4e-9	2e-8	6e-8	2e-7	3e-7	6e-7	1e-6	2e-6	3e-6	4e-6	5e-6	7e-6	1e-5	1e-5	2e-5	2e-5
		5	8e-9	6e-8	3e-7	8e-7	2e-6	4e-6	8e-6	1e-5	3e-5	4e-5	6e-5	9e-5	1e-4	2e-4	3e-4	4e-4
		6	2e-8	2e-7	1e-6	4e-6	1e-5	3e-5	7e-5	1e-4	3e-4	4e-4	7e-4	1e-3	2e-3	3e-3	4e-3	6e-3
		7	3e-8	5e-7	4e-6	2e-5	7e-5	2e-4	5e-4	1e-3	3e-3	5e-3	9e-3	2e-2	3e-2	4e-2	7e-2	1e-1
		8	6e-8	2e-6	2e-5	1e-4	4e-4	1e-3	4e-3	1e-2	3e-2	5e-2	1e-1	2e-1	4e-1	6e-1	1e+0	2e+0
		9	1e-7	5e-6	7e-5	5e-4	3e-3	1e-2	3e-2	1e-1	3e-1	6e-1	1e+0	3e+0	5e+0	1e+1	2e+1	3e+1
		10	3e-7	1e-5	3e-4	2e-3	2e-2	7e-2	3e-1	9e-1	3e+0	6e+0	2e+1	3e+1	7e+1	1e+2	3e+2	5e+2
		11	5e-7	4e-5	1e-3	1e-2	9e-2	5e-1	2e+0	8e+0	3e+1	7e+1	2e+2	4e+2	1e+3	2e+3	4e+3	9e+3
		12	1e-6	1e-4	4e-3	6e-2	5e-1	3e+0	2e+1	7e+1	3e+2	8e+2	2e+3	6e+3	1e+4	3e+4	7e+4	1e+5
		13	2e-6	4e-4	2e-2	3e-1	3e+0	2e+1	1e+2	6e+2	3e+3	9e+3	3e+4	8e+4	2e+5	5e+5	1e+6	2e+6
		14	4e-6	1e-3	7e-2	2e+0	2e+1	2e+2	1e+3	6e+3	3e+4	9e+4	3e+5	1e+6	3e+6	7e+6	2e+7	4e+7
		15	8e-6	4e-3	3e-1	8e+0	1e+2	1e+3	9e+3	5e+4	3e+5	1e+6	4e+6	1e+7	4e+7	1e+8	3e+8	7e+8
		16	2e-5	1e-2	1e+0	4e+1	7e+2	8e+3	7e+4	5e+5	3e+6	1e+7	5e+7	2e+8	5e+8	2e+9	5e+9	1e+10
*/