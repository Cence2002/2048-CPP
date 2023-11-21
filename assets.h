#pragma once

#include <bits/stdc++.h>
#include <immintrin.h>

using namespace std;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

enum Dir : u8 {
    None = 0,
    Left = 1,
    Up = 2,
    Right = 3,
    Down = 4,
};

constexpr auto Dirs = {Left, Up, Right, Down};

auto time_now() {
    return chrono::high_resolution_clock::now();
}

u64 time_since(chrono::high_resolution_clock::time_point start) {
    return chrono::duration_cast<chrono::microseconds>(time_now() - start).count();
}

u32 random(const u32 n) { return rand() % n; }

constexpr u64 E(const u8 n) { return u64(1) << n; }

inline u8 popcnt(const u64 x) { return __builtin_popcountll(x); }

inline u64 pext(const u64 x, const u64 m) { return _pext_u64(x, m); }

inline u64 pdep(const u64 x, const u64 m) { return _pdep_u64(x, m); }

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

u64 cnt = 0;
