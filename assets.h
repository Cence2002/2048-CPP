#pragma once

#include <bits/stdc++.h>

using namespace std;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

enum Direction : u8 {
    NONE = 0,
    LEFT = 1,
    UP = 2,
    RIGHT = 3,
    DOWN = 4,
};

constexpr auto DIRS = {LEFT, UP, RIGHT, DOWN};

auto time_now() { return chrono::high_resolution_clock::now(); }

u64 time_since(chrono::high_resolution_clock::time_point start) {
    return chrono::duration_cast<chrono::microseconds>(time_now() - start).count();
}

u8 random(u8 n) {
    return rand() % n;
}

u64 power(u64 base, u8 exp) {
    u64 res = 1;
    while (exp > 0) {
        if (exp & 1) {
            res *= base;
        }
        exp >>= 1;
        base *= base;
    }
    return res;
}
