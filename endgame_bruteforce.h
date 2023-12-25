#pragma once

//#include "general_board.h"
#include "board.h"

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

constexpr u8 get_G(u64 board) {
    u8 G = 0;
    for (u8 i = 0; i < 16; ++i) {
        u8 cell = board & 0xFu;
        if (cell == 0xFu) { continue; }
        G = max(G, cell);
        board >>= 4;
    }
    return G;
}

constexpr u64 get_size(const u64 board) {
    return power(get_G(board), count_empty<4>(board));
}

//take a board with some large tiles and some small tiles
//given mask is B
//ignore tiles that are not empty in B (base)
//only return hash if, on all empty tiles in B, the tile is small enough
//otherwise, return 0
template<u64 B>
u64 get_hash(u64 board) {
    u64 hash = 0;
    u8 G = get_G(B);
    u64 empty = empty_mask<4>(B);
    for (u8 i = 0; i < 16; ++i) {
        if (empty & 0xFu) {
            u8 cell = board & 0xFu;
            if (cell >= G) { return 0; }
            hash = hash * G + cell;
        }
        board >>= 4;
        empty >>= 4;
    }
    return hash;
}

//B is the base board for this endgame
//it contains 16 tiles, each 4 bits
//values can be:
// 0: small tile (< G)
// F: large tile (>= G)
// 2: goal tile (== G)
// goal tile is the one we want to merge with new tiles
// we want to maximize the probability of reaching the goal tile
// building a goal tile next to the original goal tile is a win
template<u64 B>
class Endgame {
private:
    static constexpr u8 G = get_G(B);
    array<r_t, get_size(B)> probs{};

    //select boards from where we can definitely win
    //build a goal tile next to the original goal tile
    //set the probability of reaching the goal tile to 1
    void init_goals() {
        u64 empty = empty_mask<4>(B);
        u64 mask = 1;
        for (u8 i = 0; i < 16; ++i) {
            if (empty & 0xFu) {
                u64 board = B | mask;
                u64 hash = get_hash<B>(board);
                if (hash) {
                    probs[hash] = 1;
                }
            }
            mask <<= 4;
            empty >>= 4;
        }
    }

public:
    Endgame() {
        probs.fill(-1);
        init_goals();
    }
};

// E D C B
// 2 4 9 A
// 1 2 2 1
// 2 5 1 0
// ==>
// 1 1 1 1
// 0 0 9 1
// 0 0 0 0
// 0 0 0 0

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