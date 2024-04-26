#pragma once

#include "board_all.h"

inline r_t learning_rate = 0;

struct Tuple {
    const string name;

    const u64 mask;
    const u64 size = E(popcnt(mask));
    vector<r_t> weights;

    Tuple(const string name, const u64 mask) : name(name), mask(mask) {}

    void init() {
        weights.resize(size, 0);
    }

    inline r_t &operator[](const u32 index) {
        return weights[index];
    }

    inline const r_t &operator[](const u32 index) const {
        return weights[index];
    }

    inline void update_const(const u32 index, const r_t gradient) {
        weights[index] += gradient * learning_rate;
    }
};

using NTuple = array<Tuple, 12>;

inline NTuple tuples_4_stage_1 = {
        Tuple("FFF0FFF", 0xFFF0FFFull),
        Tuple("FF00FF00FF0", 0xFF00FF00FF0ull),
        Tuple("FFFFFF", 0xFFFFFFull),
        Tuple("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple("FFFFFF0", 0xFFFFFF0ull),
        Tuple("F000FFFFF", 0xF000FFFFFull),
        Tuple("F0FFFF000F", 0xF0FFFF000Full),
        Tuple("F0FFFFF00", 0xF0FFFFF00ull),
        Tuple("F00FFFFF", 0xF00FFFFFull),
        Tuple("FFFF0F0F", 0xFFFF0F0Full),
        Tuple("F0F0F0FFF", 0xF0F0F0FFFull),
        Tuple("FFFFFF000", 0xFFFFFF000ull),
};

inline NTuple tuples_4_stage_2 = {
        Tuple("FFF0FFF", 0xFFF0FFFull),
        Tuple("FF00FF00FF0", 0xFF00FF00FF0ull),
        Tuple("FFFFFF", 0xFFFFFFull),
        Tuple("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple("FFFFFF0", 0xFFFFFF0ull),
        Tuple("F000FFFFF", 0xF000FFFFFull),
        Tuple("F0FFFF000F", 0xF0FFFF000Full),
        Tuple("F0FFFFF00", 0xF0FFFFF00ull),
        Tuple("F00FFFFF", 0xF00FFFFFull),
        Tuple("FFFF0F0F", 0xFFFF0F0Full),
        Tuple("F0F0F0FFF", 0xF0F0F0FFFull),
        Tuple("FFFFFF000", 0xFFFFFF000ull),
};

inline void save_packed_weights(const string &id, const NTuple &tuples) {
    const string filename = "weights-" + id + ".bin";
    auto start = time_now();
    ofstream file("../weights_backups/" + filename, ios::binary);
    if (!file.is_open()) {
        cout << "Error opening: " << filename << endl;
        return;
    }
    for (const auto &t: tuples) {
        size_t size = t.weights.size() * sizeof(t.weights[0]);
        file.write(t.name.c_str(), streamsize(t.name.size()));
        file.write((const char *) &size, sizeof(size));
        file.write((const char *) &t.weights[0], streamsize(size));
    }
    file.close();
    cout << "Saving " << filename << " finished: " << time_since(start) / 1e6 << " s" << endl;
}

inline void load_packed_weights(const string &id, NTuple &tuples) {
    const string filename = "weights-" + id + ".bin";
    auto start = time_now();
    ifstream file("../weights_backups/" + filename, ios::binary);
    if (!file.is_open()) {
        cout << "Error opening: " << filename << endl;
        return;
    }
    for (auto &t: tuples) {
        size_t size;
        string name_buffer(t.name.size(), '\0');
        file.read(&name_buffer[0], streamsize(t.name.size()));
        assert(name_buffer == t.name);
        t.init();
        file.read((char *) &size, sizeof(size));
        assert(size == t.weights.size() * sizeof(t.weights[0]));
        file.read((char *) &t.weights[0], streamsize(size));
    }
    file.close();
    cout << "Loading " << filename << " finished: " << time_since(start) / 1e6 << " s" << endl;
}
