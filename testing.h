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
    array<array<bool, N>, N> empty_mask() {
        array<array<bool, N>, N> res;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; ++j) {
                res[i][j] = rows[i][j] == 0;
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
        u16 mask = 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N - 1; ++j) {
                if (tuple_mask & mask) {
                    res *= base;
                    res += rows[i][j];
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

template<int N, int BASE>
vector<array<u8, N>> generate_lines() {
    vector<array<u8, N>> res;
    array<u8, N> line;
    for (int mask = 0; mask < power(BASE, N); mask++) {
        int m = mask;
        for (int i = 0; i < N; i++) {
            line[i] = m % BASE;
            m /= BASE;
        }
        res.push_back(line);
    }
    return res;
}

template<int N, int BASE>
void test_line_hash(u32 (*hash)(array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        u32 expected = Line<N>(line).hash(BASE);
        u32 res = hash(line);
        assert(res == expected);
    }
    cout << "test_line_hash<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_line_left_move(array<u8, N> (*left_move)(array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        array<u8, N> expected = Line<N>(line).left_moved().to_array();
        array<u8, N> res = left_move(line);
        assert(res == expected);
    }
    cout << "test_line_left_move<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_line_right_move(array<u8, N> (*right_move)(array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        array<u8, N> expected = Line<N>(line).right_moved().to_array();
        array<u8, N> res = right_move(line);
        assert(res == expected);
    }
    cout << "test_line_right_move<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_line_reward(u32 (*reward)(array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        u32 expected = Line<N>(line).reward();
        u32 res = reward(line);
        assert(res == expected);
    }
    cout << "test_line_reward<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
vector<array<array<u8, N>, N>> generate_boards(int n = 10000) {
    vector<array<array<u8, N>, N>> res;
    for (int t = 0; t < n; t++) {
        array<array<u8, N>, N> board;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; ++j) {
                bool empty = random(2);
                if (empty) {
                    board[i][j] = 0;
                } else {
                    board[i][j] = random(BASE);
                }
            }
        }
        res.push_back(board);
    }
    return res;
}

template<int N, int BASE>
void test_board_move(array<array<u8, N>, N> (*move)(array<array<u8, N>, N>, Direction)) {
    for (auto &board: generate_boards<N, BASE>()) {
        for (auto dir: DIRS) {
            array<array<u8, N>, N> expected = Board<N>(board).moved(Direction(dir)).to_array();
            array<array<u8, N>, N> res = move(board, Direction(dir));
            assert(res == expected);
        }
    }
    cout << "test_board_move<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_board_reward(u32 (*reward)(array<array<u8, N>, N>, Direction)) {
    for (auto &board: generate_boards<N, BASE>()) {
        for (auto dir: DIRS) {
            u32 expected = Board<N>(board).reward(Direction(dir));
            u32 res = reward(board, Direction(dir));
            assert(res == expected);
        }
    }
    cout << "test_board_reward<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_board_count_empty(u8 (*count_empty)(array<array<u8, N>, N>)) {
    for (auto &board: generate_boards<N, BASE>()) {
        u8 expected = Board<N>(board).count_empty();
        u8 res = count_empty(board);
        assert(res == expected);
    }
    cout << "test_board_count_empty<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_board_empty_mask(array<array<bool, N>, N> (*empty_mask)(array<array<u8, N>, N>)) {
    for (auto &board: generate_boards<N, BASE>()) {
        array<array<bool, N>, N> expected = Board<N>(board).empty_mask();
        array<array<bool, N>, N> res = empty_mask(board);
        assert(res == expected);
    }
    cout << "test_board_empty_mask<" << N << ", " << BASE << "> passed" << endl;
}

float chi_squared_test(vector<pair<float, float>> &data) {
    float res = 0;
    for (auto [observed, expected]: data) {
        res += (observed - expected) * (observed - expected) / expected;
    }
    return res;
}

float incomplete_gamma(float s, float x) {
    float sum = 0;
    float term = pow(x, s) * exp(-x) / s;
    for (int t = 1; t <= 1000; t++) {
        sum += term;
        term *= x / (s + float(t));
        if (term < 1e-9f) { break; }
    }
    return sum;
}

float p_value_from_chi_squared(float x, float k) {
    return 1.0f - incomplete_gamma(k / 2, x / 2) / tgamma(k / 2);
}

float p_from_z(float z_score) {
    return 0.5f * erfcf(z_score / sqrt(2.0f));
}

float z_from_p(float p_value) {
    //use binary search
    float lo = -10;
    float hi = 10;
    while (hi - lo > 1e-6f) {
        float mid = (lo + hi) / 2;
        float p = p_from_z(mid);
        if (p > p_value) {
            lo = mid;
        } else {
            hi = mid;
        }
    }
    return (lo + hi) / 2;
}

float total_p_value(vector<float> &p_values) {
    float z_sum = 0;
    for (float p: p_values) { z_sum += z_from_p(p); }
    float z_avg = z_sum / sqrt(float(p_values.size()));
    return p_from_z(z_avg);
}

template<int N, int BASE>
void test_board_fill(array<array<u8, N>, N> (*fill)(array<array<u8, N>, N>)) {
    vector<float> p_values;
    for (array<array<u8, N>, N> board: generate_boards<N, BASE>(10)) {
        u8 empty = Board<N>(board).count_empty();
        vector<pair<float, float>> data(empty * 2);

        int k = 1000000;
        for (int i = 0; i < empty; i++) {
            data[i].second = float(k) * (0.9f / float(empty));
            data[empty + i].second = float(k) * (0.1f / float(empty));
        }
        for (int t = 0; t < k; t++) {
            array<array<u8, N>, N> filled = fill(board);
            u8 ind = 0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (board[i][j] != 0) { continue; }
                    if (filled[i][j] == 0) {
                        ind++;
                    } else if (filled[i][j] == 1) {
                        data[ind].first++;
                        ind++;
                    } else if (filled[i][j] == 2) {
                        data[empty + ind].first++;
                        ind++;
                    } else {
                        assert(false);
                    }
                }
            }
        }
        float chi_squared = chi_squared_test(data);
        int df = empty * 2 - 1;
        float p_value = p_value_from_chi_squared(chi_squared, float(df));
        p_values.push_back(p_value);

        //Board<N>(board).print();
        //cout << "p = " << p_value << endl;
        //for (auto [observed, expected]: data) {
        //    cout << observed << " " << expected << endl;
        //}
    }
    sort(p_values.begin(), p_values.end());
    //for (float p: p_values) { cout << p << " "; }
    //cout << endl;
    float total_p = total_p_value(p_values);
    assert(total_p > 0.05f);
    cout << "test_board_fill<" << N << ", " << BASE << "> passed: p = " << total_p << endl;
}
