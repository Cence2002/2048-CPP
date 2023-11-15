#pragma once

#include "assets.h"

template<int N>
class Line {
private:
    // C0 C1 C2 C3
    array<u8, N> cells;

public:
    Line() {
        cells.fill(0);
    }

    Line(const array<u8, N> cells) : cells(cells) {}

    Line(const Line &line) : cells(line.cells) {}

    Line &operator=(const Line &line) {
        cells = line.cells;
        return *this;
    }

    u8 &operator[](const u8 i) {
        return cells[i];
    }

    bool operator==(const Line &line) const {
        return cells == line.cells;
    }

    bool operator!=(const Line &line) const {
        return cells != line.cells;
    }

    array<u8, N> to_array() {
        return cells;
    }

    // C3 C2 C1 C0
    u32 hash(u8 base) {
        u32 res = 0;
        for (int i = 0; i < N; i++) {
            res += cells[i] * power(base, i);
        }
        return res;
    }

    Line<N> reversed() {
        Line<N> res;
        for (int i = 0; i < N; i++) {
            res[i] = cells[N - i - 1];
        }
        return res;
    }

    // <= C0 C1 C2 C3 <=
    void left_move() {
        u8 to = 0;
        bool merged = false;
        for (int from = 0; from < N; from++) {
            if (cells[from] == 0) { continue; }
            if (to > 0 && cells[to - 1] == cells[from] && !merged) {
                merged = true;
                cells[to - 1] += 1;
                cells[from] = 0;
                continue;
            }
            if (to < from) {
                cells[to] = cells[from];
                cells[from] = 0;
            }
            to++;
            merged = false;
        }
    }

    Line<N> left_moved() {
        Line<N> res(*this);
        res.left_move();
        return res;
    }

    // => C0 C1 C2 C3 =>
    void right_move() {
        Line<N> res = reversed();
        res.left_move();
        cells = res.reversed().cells;
    }

    Line<N> right_moved() {
        Line<N> res(*this);
        res.right_move();
        return res;
    }

    // both left and right
    u32 reward() {
        u32 res = 0;
        bool merged = false;
        u8 prev = 0;
        for (int i = 0; i < N; i++) {
            if (cells[i] == 0) { continue; }
            if (prev == cells[i] && !merged) {
                merged = true;
                res += power(2, cells[i] + 1);
                continue;
            }
            prev = cells[i];
            merged = false;
        }
        return res;
    }

    void print() {
        for (int i = 0; i < N; i++) {
            if (cells[i] == 0) {
                cout << "-";
            } else if (cells[i] < 10) {
                cout << int(cells[i]);
            } else {
                cout << char('A' + cells[i] - 10);
            }
            cout << " ";
        }
        cout << endl;
    }
};

template<int N>
class Board {
private:
    // R0 R1 R2 R3
    array<Line<N>, N> rows;

public:
    Board() {
        rows.fill(Line<N>());
    }

    Board(const array<array<u8, N>, N> &rows) {
        for (int i = 0; i < N; i++) {
            this->rows[i] = Line<N>(rows[i]);
        }
    }

    Board(const array<Line<N>, N> &rows) : rows(rows) {}

    Board(const Board &board) : rows(board.rows) {}

    Board &operator=(const Board &board) {
        rows = board.rows;
        return *this;
    }

    Line<N> &operator[](const u8 i) {
        return rows[i];
    }

    bool operator==(const Board &board) const {
        return rows == board.rows;
    }

    bool operator!=(const Board &board) const {
        return rows != board.rows;
    }

    array<array<u8, N>, N> to_array() {
        array<array<u8, N>, N> res;
        for (int i = 0; i < N; i++) {
            res[i] = rows[i].to_array();
        }
        return res;
    }

    void transpose() {
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; ++j) {
                swap(rows[i][j], rows[j][i]);
            }
        }
    }

    void move(Direction dir) {
        switch (dir) {
            case LEFT:
                for (int i = 0; i < N; i++) {
                    rows[i].left_move();
                }
                break;
            case UP:
                transpose();
                for (int i = 0; i < N; i++) {
                    rows[i].left_move();
                }
                transpose();
                break;
            case RIGHT:
                for (int i = 0; i < N; i++) {
                    rows[i].right_move();
                }
                break;
            case DOWN:
                transpose();
                for (int i = 0; i < N; i++) {
                    rows[i].right_move();
                }
                transpose();
                break;
            default:
                break;
        }
    }

    Board<N> moved(Direction dir) {
        Board<N> res(*this);
        res.move(dir);
        return res;
    }

    u32 reward(Direction dir) {
        u32 res = 0;
        switch (dir) {
            case LEFT:
            case RIGHT:
                for (int i = 0; i < N; i++) {
                    res += rows[i].reward();
                }
                break;
            case UP:
            case DOWN:
                transpose();
                for (int i = 0; i < N; i++) {
                    res += rows[i].reward();
                }
                transpose();
                break;
            default:
                break;
        }
        return res;
    }

    u8 count_empty() {
        u8 res = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; ++j) {
                if (rows[i][j] == 0) { res++; }
            }
        }
        return res;
    }

    // 00 01 02 03
    // 10 11 12 13
    // 20 21 22 23
    // 30 31 32 33
    u16 empty_mask() {
        u16 res = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; ++j) {
                if (rows[i][j] == 0) {
                    res |= 1 << (i * N + j);
                }
            }
        }
        return res;
    }

    void fill() {
        u8 empty = count_empty();
        if (empty == 0) { return; }
        u8 pos = random(empty);
        u8 val = random(10) == 0 ? 2 : 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; ++j) {
                if (rows[i][j] > 0) { continue; }
                if (pos == 0) {
                    rows[i][j] = val;
                    return;
                }
                pos--;
            }
        }
    }

    Board<N> filled() {
        Board<N> res(*this);
        res.fill();
        return res;
    }

    u32 tuple_hash(u16 tuple_mask, u8 base) {
        u32 res = 0;
        u8 index = 0;
        u16 mask = 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N - 1; ++j) {
                if (tuple_mask & mask) {
                    res += rows[i][j] * power(base, index);
                    index++;
                }
                mask <<= 1;
            }
        }
        return res;
    }

    void print() {
        for (int i = 0; i < N; i++) {
            rows[i].print();
        }
    }
};
