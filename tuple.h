#pragma once

#include "board_all.h"

inline r_t learning_rate = 0;

struct Tuple {
    const std::string name;

    const u64 mask;
    const u64 size = E(popcnt(mask));
    std::vector<r_t> weights;

    Tuple(const std::string name, const u64 mask) : name(name), mask(mask) {}

    void init(r_t value = 0) {
        weights.clear();
        weights.resize(size, value);
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

using NTuple = std::vector<Tuple>;

NTuple ntuple_bence_stage_1 = {
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

NTuple ntuple_bence_stage_2 = {
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

NTuple ntuple_yeh = {
        Tuple("FFFFFF", 0xFFFFFFull),
        Tuple("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple("FFF0FFF", 0xFFF0FFFull),
        Tuple("FFF0FFF0000", 0xFFF0FFF0000ull),
};

NTuple ntuple_all5 = {
        Tuple("FFF0F000F00", 0xFFF0F000F00ull),
        Tuple("FFFFF00000", 0xFFFFF00000ull),
        Tuple("FFF00FF", 0xFFF00FFull),
        Tuple("F000F0FFF", 0xF000F0FFFull),
        Tuple("FF00FF000F0", 0xFF00FF000F0ull),
        Tuple("FFF0FF0", 0xFFF0FF0ull),
        Tuple("F0FFF0F00", 0xF0FFF0F00ull),
        Tuple("F00FFF00F0", 0xF00FFF00F0ull),
        Tuple("FFFFF0", 0xFFFFF0ull),
        Tuple("FF0FFF", 0xFF0FFFull),
        Tuple("FFFF00F0", 0xFFFF00F0ull),
        Tuple("FFFFF00", 0xFFFFF00ull),
        Tuple("FFFF000F", 0xFFFF000Full),
        Tuple("FFFFF0000", 0xFFFFF0000ull),
        Tuple("FFF0F0F", 0xFFF0F0Full),
        Tuple("F0FFFF0000", 0xF0FFFF0000ull),
        Tuple("FF00FFF", 0xFF00FFFull),
        Tuple("F00FF000FF", 0xF00FF000FFull),
        Tuple("F0FFFF", 0xF0FFFFull),
        Tuple("F0FFF00F0", 0xF0FFF00F0ull),
        Tuple("F0F0FFF0000", 0xF0F0FFF0000ull),
        Tuple("F0F0FFF", 0xF0F0FFFull),
        Tuple("FFFFF", 0xFFFFFull),
        Tuple("F0FFF0F000", 0xF0FFF0F000ull),
        Tuple("F000F0FFF0", 0xF000F0FFF0ull),
        Tuple("F00FF00FF0", 0xF00FF00FF0ull),
        Tuple("FF0FF00F00", 0xFF0FF00F00ull),
        Tuple("F0FFF000F0", 0xF0FFF000F0ull),
        Tuple("F00FF0FF00", 0xF00FF0FF00ull),
        Tuple("F00FFF0F00", 0xF00FFF0F00ull),
        Tuple("F0FFF000F", 0xF0FFF000Full),
        Tuple("F00FF0FF0", 0xF00FF0FF0ull),
        Tuple("F0FFF00F00", 0xF0FFF00F00ull)
};

inline void save_packed_weights(const std::string &id, const NTuple &tuples) {
    const std::string filename = "weights-" + id + ".bin";
    auto start = time_now();
    std::ofstream file("../weights_backups/" + filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error opening: " << filename << std::endl;
        return;
    }
    for (const auto &t: tuples) {
        size_t size = t.weights.size() * sizeof(t.weights[0]);
        file.write(t.name.c_str(), std::streamsize(t.name.size()));
        file.write((const char *) &size, sizeof(size));
        file.write((const char *) &t.weights[0], std::streamsize(size));
    }
    file.close();
    std::cout << "Saving " << filename << " finished: " << time_since(start) / 1e6 << " s" << std::endl;
}

inline void load_packed_weights(const std::string &id, NTuple &tuples) {
    const std::string filename = "weights-" + id + ".bin";
    auto start = time_now();
    std::ifstream file("../weights_backups/" + filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error opening: " << filename << std::endl;
        return;
    }
    for (auto &t: tuples) {
        size_t size;
        std::string name_buffer(t.name.size(), '\0');
        file.read(&name_buffer[0], std::streamsize(t.name.size()));
        assert(name_buffer == t.name);
        t.init();
        file.read((char *) &size, sizeof(size));
        assert(size == t.weights.size() * sizeof(t.weights[0]));
        file.read((char *) &t.weights[0], std::streamsize(size));
    }
    file.close();
    std::cout << "Loading " << filename << " finished: " << time_since(start) / 1e6 << " s" << std::endl;
}
