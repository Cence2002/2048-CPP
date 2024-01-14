#pragma once

#include "board.h"

r_t learning_rate = 0;

struct Tuple {
    const string name;
    const u64 mask;
    vector<r_t> weights;

    Tuple(const u8 N, const string name, const u64 mask) : name(name), mask(mask) {
        weights.resize(E(N * 4), 0);
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

NTuple tuples_4_stage_1 = {
        Tuple(6, "FFF0FFF", 0xFFF0FFFull),
        Tuple(6, "FF00FF00FF0", 0xFF00FF00FF0ull),
        Tuple(6, "FFFFFF", 0xFFFFFFull),
        Tuple(6, "FFFFFF0000", 0xFFFFFF0000ull),
        Tuple(6, "FFFFFF0", 0xFFFFFF0ull),
        Tuple(6, "F000FFFFF", 0xF000FFFFFull),
        Tuple(6, "F0FFFF000F", 0xF0FFFF000Full),
        Tuple(6, "F0FFFFF00", 0xF0FFFFF00ull),
        Tuple(6, "F00FFFFF", 0xF00FFFFFull),
        Tuple(6, "FFFF0F0F", 0xFFFF0F0Full),
        Tuple(6, "F0F0F0FFF", 0xF0F0F0FFFull),
        Tuple(6, "FFFFFF000", 0xFFFFFF000ull),
};

NTuple tuples_4_stage_2 = {
        Tuple(6, "FFF0FFF", 0xFFF0FFFull),
        Tuple(6, "FF00FF00FF0", 0xFF00FF00FF0ull),
        Tuple(6, "FFFFFF", 0xFFFFFFull),
        Tuple(6, "FFFFFF0000", 0xFFFFFF0000ull),
        Tuple(6, "FFFFFF0", 0xFFFFFF0ull),
        Tuple(6, "F000FFFFF", 0xF000FFFFFull),
        Tuple(6, "F0FFFF000F", 0xF0FFFF000Full),
        Tuple(6, "F0FFFFF00", 0xF0FFFFF00ull),
        Tuple(6, "F00FFFFF", 0xF00FFFFFull),
        Tuple(6, "FFFF0F0F", 0xFFFF0F0Full),
        Tuple(6, "F0F0F0FFF", 0xF0F0F0FFFull),
        Tuple(6, "FFFFFF000", 0xFFFFFF000ull),
};

void save_packed_weights(const string &id, const NTuple &tuples) {
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

void load_packed_weights(const string &id, const NTuple &tuples) {
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
        file.read((char *) &size, sizeof(size));
        assert(size == t.weights.size() * sizeof(t.weights[0]));
        file.read((char *) &t.weights[0], streamsize(size));
    }
    file.close();
    cout << "Loading " << filename << " finished: " << time_since(start) / 1e6 << " s" << endl;
}
