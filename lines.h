#pragma once

#include "assets.h"

struct line_base {
    line_base() = default;

    virtual void load_array(const std::array<u8, 4> &cells) = 0;

    virtual std::array<u8, 4> get_array() const = 0;

    virtual u8 get_item(u8 i) const = 0;

    virtual void set_item(u8 i, u8 cell) = 0;

    virtual void reverse() = 0;

    virtual u8 empty_count() const = 0;

    virtual s_t get_reward() const = 0;

    virtual void left() = 0;

    virtual void right() = 0;

    std::string to_string() const {
        std::string str;
        for (u8 i = 0; i < 4; i++) {
            str.push_back(cell_str[get_item(i)]);
            str += " ";
        }
        str += "\n";
        return str;
    }

    friend std::ostream &operator<<(std::ostream &os, const line_base &line) {
        os << line.to_string();
        return os;
    }
};

template<typename T>
concept line_concept = std::is_base_of_v<line_base, T>
                       && std::is_default_constructible_v<T>
                       && std::is_copy_constructible_v<T>
                       && std::is_copy_assignable_v<T>
                       && requires() {{ T::init_tables() } -> std::same_as<void>; }
                       && requires(T line, T other) {
    { line == other } -> std::same_as<bool>;
    { line != other } -> std::same_as<bool>;
};

struct l_t_16 : public line_base {
private:
    u16 line;

    static s_t reward_table[E(16)];
    static u16 left_table[E(16)];
    static u16 right_table[E(16)];

public:
    l_t_16() : line(0) {}

    l_t_16(const u16 line) : line(line) {}

    l_t_16(const l_t_16 &line) : line(line.line) {}

    l_t_16 &operator=(const l_t_16 &other) {
        line = other.line;
        return *this;
    }

    bool operator==(const l_t_16 &other) const {
        return line == other.line;
    }

    bool operator!=(const l_t_16 &other) const {
        return line != other.line;
    }

    u16 get_bits() const {
        return line;
    }

    void load_array(const std::array<u8, 4> &cells) override {
        line = 0;
        for (u8 i = 0; i < 4; i++) {
            line |= u16(cells[i] & 0xFu) << (i * 4);
        }
    }

    std::array<u8, 4> get_array() const override {
        std::array<u8, 4> cells{};
        for (u8 i = 0; i < 4; i++) {
            cells[i] = (line >> (i * 4)) & 0xFu;
        }
        return cells;
    }

    u8 get_item(const u8 i) const override {
        return (line >> (i * 4)) & 0xFu;
    }

    void set_item(const u8 i, const u8 cell) override {
        line &= ~(0xFu << (i * 4));
        line |= u16(cell & 0xFu) << (i * 4);
    }

    void reverse() override {
        for (u8 i = 0; i < 2; i++) {
            const u8 temp = get_item(i);
            set_item(i, get_item(3 - i));
            set_item(3 - i, temp);
        }
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 i = 0; i < 4; i++) {
            count += get_item(i) == 0;
        }
        return count;
    }

    s_t get_reward() const override {
        return reward_table[line];
    }

    void left() override {
        line = left_table[line];
    }

    void right() override {
        line = right_table[line];
    }

    static void init_tables() {
        for (u16 bits = E(16) - 1; bits > 0; --bits) {
            std::array<u8, 4> cells{};
            l_t_16 line;
            l_t_16 reversed_line;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = (bits >> (i * 4)) & 0xFu;
                cells[i] = cell;
                line.set_item(i, cell);
                reversed_line.set_item(3 - i, cell);
            }
            s_t reward = 0;
            u8 to = 0;
            bool can_merge = true;
            for (u8 from = 0; from < 4; ++from) {
                if (cells[from] == 0) {
                    continue;
                }
                if (to > 0 && cells[from] == cells[to - 1] && can_merge
                    && cells[from] != 0xFu) {
                    ++cells[to - 1];
                    cells[from] = 0;
                    reward += E(cells[to - 1]);
                    can_merge = false;
                } else {
                    if (to != from) {
                        cells[to] = cells[from];
                        cells[from] = 0;
                    }
                    ++to;
                    can_merge = true;
                }
            }
            reward_table[bits] = reward;

            l_t_16 moved_line = 0;
            l_t_16 moved_reversed_line = 0;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = cells[i];
                moved_line.set_item(i, cell);
                moved_reversed_line.set_item(3 - i, cell);
            }
            left_table[line.get_bits()] = moved_line.get_bits();
            right_table[reversed_line.get_bits()] = moved_reversed_line.get_bits();
        }
    }
};

s_t l_t_16::reward_table[E(16)];
u16 l_t_16::left_table[E(16)];
u16 l_t_16::right_table[E(16)];

struct l_t_32 : public line_base {
private:
    u32 line;

    static s_t reward_table[E(20)];
    static u32 left_table[E(20)];
    static u32 right_table[E(20)];

public:
    l_t_32() : line(0) {}
    l_t_32(const u32 line) : line(line) {}
    l_t_32(const l_t_32 &line) : line(line.line) {}

    l_t_32 &operator=(const l_t_32 &other) {
        line = other.line;
        return *this;
    }

    bool operator==(const l_t_32 &other) const {
        return line == other.line;
    }

    bool operator!=(const l_t_32 &other) const {
        return line != other.line;
    }

    u32 get_bits() const {
        return line;
    }

    void load_array(const std::array<u8, 4> &cells) override {
        line = 0;
        for (u8 i = 0; i < 4; i++) {
            line |= u32(cells[i] & 0x1Fu) << (i * 5);
        }
    }

    std::array<u8, 4> get_array() const override {
        std::array<u8, 4> cells{};
        for (u8 i = 0; i < 4; i++) {
            cells[i] = (line >> (i * 5)) & 0x1Fu;
        }
        return cells;
    }

    u8 get_item(const u8 i) const override {
        return (line >> (i * 5)) & 0x1Fu;
    }

    void set_item(const u8 i, const u8 cell) override {
        line &= ~(0x1Fu << (i * 5));
        line |= u32(cell & 0x1Fu) << (i * 5);
    }

    void reverse() override {
        for (u8 i = 0; i < 2; i++) {
            const u8 temp = get_item(i);
            set_item(i, get_item(3 - i));
            set_item(3 - i, temp);
        }
    }

    u8 empty_count() const override {
        u8 count = 0;
        for (u8 i = 0; i < 4; i++) {
            count += get_item(i) == 0;
        }
        return count;
    }

    s_t get_reward() const override {
        return reward_table[line];
    }

    void left() override {
        line = left_table[line];
    }

    void right() override {
        line = right_table[line];
    }

    static void init_tables() {
        for (u32 bits = E(20) - 1; bits > 0; --bits) {
            std::array<u8, 4> cells{};
            l_t_32 line;
            l_t_32 reversed_line;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = (bits >> (i * 5)) & 0x1Fu;
                cells[i] = cell;
                line.set_item(i, cell);
                reversed_line.set_item(3 - i, cell);
            }
            s_t reward = 0;
            u8 to = 0;
            bool can_merge = true;
            for (u8 from = 0; from < 4; ++from) {
                if (cells[from] == 0) {
                    continue;
                }
                if (to > 0 && cells[from] == cells[to - 1] && can_merge) {
                    ++cells[to - 1];
                    cells[from] = 0;
                    reward += E(cells[to - 1]);
                    can_merge = false;
                } else {
                    if (to != from) {
                        cells[to] = cells[from];
                        cells[from] = 0;
                    }
                    ++to;
                    can_merge = true;
                }
            }
            reward_table[line.get_bits()] = reward;

            l_t_32 moved_line = 0;
            l_t_32 moved_reversed_line = 0;
            for (u8 i = 0; i < 4; i++) {
                const u8 cell = cells[i];
                moved_line.set_item(i, cell);
                moved_reversed_line.set_item(3 - i, cell);
            }
            left_table[line.get_bits()] = moved_line.get_bits();
            right_table[reversed_line.get_bits()] = moved_reversed_line.get_bits();
        }
    }
};

s_t l_t_32::reward_table[E(20)];
u32 l_t_32::left_table[E(20)];
u32 l_t_32::right_table[E(20)];