#pragma once

#include "general_board.h"


u64 get_size(array<array<u8, 4>, 4> &B) {
    u64 size = 1;
    for (u8 i = 0; i < 4; ++i) {
        for (u8 j = 0; j < 4; ++j) {
            if (B[i][j] > 17) { continue; }
            size *= B[i][j];
        }
    }
    return size;
}

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

template<array<array<u8, 4>, 4> &B, u8 G>
class Endgame {
private:
    array<r_t, get_size(B)> probs{};

    Endgame() {
    }
};

/*class Endgame {
public:
    array<r_t, B> probs{};

    Endgame() {
        weights.fill(0);
        MIN
    }

    inline r_t &operator[](const u32 index) {
        return weights[index];
    }
};*/