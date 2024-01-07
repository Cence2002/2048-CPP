#pragma once

#include "board.h"

constexpr r_t tuple_init = 2e5;
r_t learning_rate = 0;

template<u32 N>
struct Tuple {
    const string name;
    u64 mask;
    vector<r_t> weights;

    Tuple(string name, const u64 mask) : name(std::move(name)), mask(mask) {
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

    //r_t pow_beta1 = 1;
    //r_t pow_beta2 = 1;
    //const r_t beta_0 = 0.99;
    //const r_t beta_1 = 0.99;
    //const r_t beta_2 = 0.999;
    //const r_t epsilon = 1e-8;

    //array<r_t, E(N * 4)> sum{};
    //array<r_t, E(N * 4)> sum_abs{};
    //array<r_t, E(N * 4)> sum_squares{};
    //array<r_t, E(N * 4)> sum_squares_max{};

    /*inline void update_coherence(const u32 index, const r_t gradient) {
        sum[index] += gradient;
        sum_abs[index] += abs(gradient);
        const r_t delta = safe_div(abs(sum[index]), sum_abs[index]);
        weights[index] += gradient * learning_rate * delta;
    }

    inline void update_coherence_sqrt(const u32 index, const r_t gradient) {
        sum[index] *= beta_0;
        sum_abs[index] *= beta_0;
        sum[index] += gradient;
        sum_abs[index] += abs(gradient);
        const r_t delta = sqrt(safe_div(abs(sum[index]), sum_abs[index]));
        weights[index] += gradient * learning_rate * delta;
    }

    inline void update_adam(const u32 index, const r_t gradient) {
        pow_beta1 *= beta_1;
        pow_beta2 *= beta_2;
        sum[index] = beta_1 * sum[index] + (1 - beta_1) * gradient;
        sum_squares[index] = beta_2 * sum_squares[index] + (1 - beta_2) * gradient * gradient;
        sum_squares_max[index] = max(sum_squares_max[index], sum_squares[index]);
        const r_t m_norm = sum[index] / (1 - pow_beta1);
        const r_t v_norm = sum_squares[index] / (1 - pow_beta2);
        weights[index] += learning_rate * m_norm / (sqrt(v_norm) + epsilon);
    }

    inline void update_momentum(const u32 index, const r_t gradient) {
        pow_beta1 *= beta_1;
        sum[index] = beta_1 * sum[index] + (1 - beta_1) * gradient;
        const r_t m_norm = sum[index] / (1 - pow_beta1);
        weights[index] += learning_rate * m_norm;
    }

    inline void update_rmsprop(const u32 index, const r_t gradient) {
        pow_beta2 *= beta_2;
        sum_squares[index] = beta_2 * sum_squares[index] + (1 - beta_2) * gradient * gradient;
        const r_t v_norm = sum_squares[index] / (1 - pow_beta2);
        weights[index] += learning_rate * gradient / (sqrt(v_norm) + epsilon);
    }*/
};

array<Tuple<6>, 12> tuples_4_bence = {
        Tuple<6>("FFF0FFF", 0xFFF0FFFull),
        Tuple<6>("FF00FF00FF0", 0xFF00FF00FF0ull),
        Tuple<6>("FFFFFF", 0xFFFFFFull),
        Tuple<6>("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple<6>("FFFFFF0", 0xFFFFFF0ull),
        Tuple<6>("F000FFFFF", 0xF000FFFFFull),
        Tuple<6>("F0FFFF000F", 0xF0FFFF000Full),
        Tuple<6>("F0FFFFF00", 0xF0FFFFF00ull),
        Tuple<6>("F00FFFFF", 0xF00FFFFFull),
        Tuple<6>("FFFF0F0F", 0xFFFF0F0Full),
        Tuple<6>("F0F0F0FFF", 0xF0F0F0FFFull),
        Tuple<6>("FFFFFF000", 0xFFFFFF000ull),
};

/*
//tuples_4_yeh
array<Tuple<6>, 4> tuples_4_yeh = {
        Tuple<6>("FFFFFF", 0xFFFFFFull),
        Tuple<6>("FFFFFF0000", 0xFFFFFF0000ull),
        Tuple<6>("FFF0FFF", 0xFFF0FFFull),
        Tuple<6>("FFF0FFF0000", 0xFFF0FFF0000ull),
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


auto tuples_4 = tuples_4_bence;
constexpr u8 tuples_size_4 = tuples_4.size();

auto tuples_3 = tuples_3_4;
constexpr u8 tuples_size_3 = tuples_3.size();

template<u8 N>
void save_packed_weights(const string &id) {
    const string filename = "weights-" + id + ".bin";
    auto start = time_now();
    ofstream file("../weights_backups/" + filename, ios::binary);
    if (!file.is_open()) {
        cout << "Error opening: " << filename << endl;
        return;
    }
    if constexpr (N == 4) {
        for (u8 i = 0; i < tuples_size_4; ++i) {
            auto &t = tuples_4[i];
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            file.write(t.name.c_str(), streamsize(t.name.size()));
            file.write((const char *) &size, sizeof(size));
            file.write((const char *) &t.weights[0], streamsize(size));
        }
    } else {
        for (u8 i = 0; i < tuples_size_3; ++i) {
            auto &t = tuples_3[i];
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            file.write(t.name.c_str(), streamsize(t.name.size()));
            file.write((const char *) &size, sizeof(size));
            file.write((const char *) &t.weights[0], streamsize(size));
        }
    }
    file.close();
    cout << "Saving " << filename << " finished: " << time_since(start) << " us" << endl;
}

template<u8 N>
void load_packed_weights(const string &id) {
    const string filename = "weights-" + id + ".bin";
    auto start = time_now();
    ifstream file("../weights_backups/" + filename, ios::binary);
    if (!file.is_open()) {
        cout << "Error opening: " << filename << endl;
        return;
    }
    if constexpr (N == 4) {
        for (u8 i = 0; i < tuples_size_4; ++i) {
            auto &t = tuples_4[i];
            size_t size;
            string nameBuffer(t.name.size(), '\0');
            file.read(&nameBuffer[0], streamsize(t.name.size()));
            assert(nameBuffer == t.name);
            file.read((char *) &size, sizeof(size));
            assert(size == t.weights.size() * sizeof(t.weights[0]));
            file.read((char *) &t.weights[0], streamsize(size));
        }
    } else {
        for (u8 i = 0; i < tuples_size_3; ++i) {
            auto &t = tuples_3[i];
            size_t size;
            string nameBuffer(t.name.size(), '\0');
            file.read(&nameBuffer[0], streamsize(t.name.size()));
            assert(nameBuffer == t.name);
            file.read((char *) &size, sizeof(size));
            assert(size == t.weights.size() * sizeof(t.weights[0]));
            file.read((char *) &t.weights[0], streamsize(size));
        }
    }
    file.close();
    cout << "Loading " << filename << " finished: " << time_since(start) << " us" << endl;
}
