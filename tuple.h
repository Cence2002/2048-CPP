#pragma once

#include "board.h"

template<u32 N>
struct Tuple {
    const string name;
    b_t mask;
    array<r_t, E(N * 4)> weights{};

    Tuple(string name, const b_t mask) : name(std::move(name)), mask(mask) {
        weights.fill(0);
    }

    inline r_t &operator[](const u32 index) {
        return weights[index];
    }
};

array<Tuple<6>, 12> tuples_4_selection = {
        Tuple<6>("mask_0", 0),
        Tuple<6>("mask_1", 0),
        Tuple<6>("mask_2", 0),
        Tuple<6>("mask_3", 0),
        Tuple<6>("mask_4", 0),
        Tuple<6>("mask_5", 0),
        Tuple<6>("mask_6", 0),
        Tuple<6>("mask_7", 0),
        Tuple<6>("mask_8", 0),
        Tuple<6>("mask_9", 0),
        Tuple<6>("mask_10", 0),
        Tuple<6>("mask_11", 0),
};

/*
//tuples_4_yeh
array<Tuple<6>, 4> tuples_4_yeh = {
        Tuple<6>("FFFFFF", 0xFFFFFFull),
        Tuple<6>("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple<6>("FFF0FFF", 0xFFF0FFFull),
        Tuple<6>("FFF0FFF0000", 0xFFF0FFF0000ull),
};

//tuples_4
array<Tuple<6>, 10> tuples_4_10 = {
        Tuple<6>("FFF0FFF", 0xFFF0FFFull),
        Tuple<6>("FF00FF00FF0", 0xFF00FF00FF0ull),
        Tuple<6>("FFFFFF", 0xFFFFFFull),
        Tuple<6>("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple<6>("FFFFFF0", 0xFFFFFF0ull),
        Tuple<6>("F000FFFFF", 0xF000FFFFFull),
        Tuple<6>("F0FFFF000F", 0xF0FFFF000Full),
        Tuple<6>("F0FFFFF00", 0xF0FFFFF00ull),
        Tuple<6>("F00FFFFF", 0xF00FFFFFull),
        Tuple<6>("FFFF0F0F", 0xFFFF0F0Full)
};

//tuples_4_all5
array<Tuple<5>, 33> tuples_4_all5 = {
        Tuple<5>("FFF0F000F00", 0xFFF0F000F00ull),
        Tuple<5>("FFFFF00000", 0xFFFFF00000ull),
        Tuple<5>("FFF00FF", 0xFFF00FFull),
        Tuple<5>("F000F0FFF", 0xF000F0FFFull),
        Tuple<5>("FF00FF000F0", 0xFF00FF000F0ull),
        Tuple<5>("FFF0FF0", 0xFFF0FF0ull),
        Tuple<5>("F0FFF0F00", 0xF0FFF0F00ull),
        Tuple<5>("F00FFF00F0", 0xF00FFF00F0ull),
        Tuple<5>("FFFFF0", 0xFFFFF0ull),
        Tuple<5>("FF0FFF", 0xFF0FFFull),
        Tuple<5>("FFFF00F0", 0xFFFF00F0ull),
        Tuple<5>("FFFFF00", 0xFFFFF00ull),
        Tuple<5>("FFFF000F", 0xFFFF000Full),
        Tuple<5>("FFFFF0000", 0xFFFFF0000ull),
        Tuple<5>("FFF0F0F", 0xFFF0F0Full),
        Tuple<5>("F0FFFF0000", 0xF0FFFF0000ull),
        Tuple<5>("FF00FFF", 0xFF00FFFull),
        Tuple<5>("F00FF000FF", 0xF00FF000FFull),
        Tuple<5>("F0FFFF", 0xF0FFFFull),
        Tuple<5>("F0FFF00F0", 0xF0FFF00F0ull),
        Tuple<5>("F0F0FFF0000", 0xF0F0FFF0000ull),
        Tuple<5>("F0F0FFF", 0xF0F0FFFull),
        Tuple<5>("FFFFF", 0xFFFFFull),
        Tuple<5>("F0FFF0F000", 0xF0FFF0F000ull),
        Tuple<5>("F000F0FFF0", 0xF000F0FFF0ull),
        Tuple<5>("F00FF00FF0", 0xF00FF00FF0ull),
        Tuple<5>("FF0FF00F00", 0xFF0FF00F00ull),
        Tuple<5>("F0FFF000F0", 0xF0FFF000F0ull),
        Tuple<5>("F00FF0FF00", 0xF00FF0FF00ull),
        Tuple<5>("F00FFF0F00", 0xF00FFF0F00ull),
        Tuple<5>("F0FFF000F", 0xF0FFF000Full),
        Tuple<5>("F00FF0FF0", 0xF00FF0FF0ull),
        Tuple<5>("F0FFF00F00", 0xF0FFF00F00ull)
};
*/

array<Tuple<4>, 2> tuples_3_4 = {
        Tuple<4>("FFFF", 0xFFFFull),
        Tuple<4>("F0FFF", 0xF0FFFull),
};

array<Tuple<5>, 2> tuples_3_5 = {
        Tuple<5>("FFFFF", 0xFFFFFull),
        Tuple<5>("F00FFFF", 0xF00FFFFull)
};

u8 num_tuples = 0;
auto tuples_4 = tuples_4_selection;
auto tuples_3 = tuples_3_4;
