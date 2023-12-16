#pragma once

#include "board.h"

template<u32 N>
struct Tuple {
    const string name;
    b_t mask;
    array<float, E(N * 4)> weights{};

    Tuple(string name, const b_t mask) : name(std::move(name)), mask(mask) {
        weights.fill(0);
    }

    inline float &operator[](const u32 index) {
        return weights[index];
    }
};

//tuples_4_yeh
array<Tuple<6>, 4> tuples_4 = {
        Tuple<6>("FFFFFF", 0xFFFFFFull),
        Tuple<6>("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple<6>("FFF0FFF", 0xFFF0FFFull),
        Tuple<6>("FFF0FFF0000", 0xFFF0FFF0000ull),
};

array<Tuple<4>, 2> tuples_3 = {
        Tuple<4>("FFFF", 0xFFFFull),
        Tuple<4>("F0FFF", 0xF0FFFull),
};