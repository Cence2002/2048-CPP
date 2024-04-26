#pragma once

#include "../board.h"
#include "general_board.h"

template<int N, int BASE>
vector<array<u8, N>> generate_lines() {
    vector<array<u8, N>> res;
    array<u8, N> line;
    for (u64 mask = 0; mask < power(BASE, N); mask++) {
        u64 m = mask;
        for (int i = 0; i < N; i++) {
            line[i] = m % BASE;
            m /= BASE;
        }
        res.push_back(line);
    }
    return res;
}

template<int N, int BASE>
void test_line_hash(u32 (*hash)(const array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        u32 expected = Line<N>(line).hash(BASE);
        u32 res = hash(line);
        assert(res == expected);
    }
    cout << "test_line_hash<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_line_left_move(array<u8, N> (*left_move)(const array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        array<u8, N> expected = Line<N>(line).left_moved().to_array();
        array<u8, N> res = left_move(line);
        assert(res == expected);
    }
    cout << "test_line_left_move<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_line_right_move(array<u8, N> (*right_move)(const array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        array<u8, N> expected = Line<N>(line).right_moved().to_array();
        array<u8, N> res = right_move(line);
        assert(res == expected);
    }
    cout << "test_line_right_move<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_line_reward(u32 (*reward)(const array<u8, N>)) {
    for (auto &line: generate_lines<N, BASE>()) {
        u32 expected = Line<N>(line).reward();
        u32 res = reward(line);
        assert(res == expected);
    }
    cout << "test_line_reward<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
vector<array<array<u8, N>, N>> generate_boards(int n = 100000) {
    vector<array<array<u8, N>, N>> res;
    for (int t = 0; t < n; t++) {
        array<array<u8, N>, N> board;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; ++j) {
                if (random(2) == 0) {
                    board[i][j] = 0;
                    continue;
                }
                board[i][j] = random(BASE - 2) + 1;
            }
        }
        res.push_back(board);
    }
    return res;
}

template<int N, int BASE>
void test_board_move(array<array<u8, N>, N> (*move)(const array<array<u8, N>, N>, Dir)) {
    for (auto &board: generate_boards<N, BASE>()) {
        for (auto d: DIRS) {
            array<array<u8, N>, N> expected = Board<N>(board).moved(d).to_array();
            array<array<u8, N>, N> res = move(board, d);
            assert(res == expected);
        }
    }
    cout << "test_board_move<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_board_reward(u32 (*reward)(const array<array<u8, N>, N>, Dir)) {
    for (auto &board: generate_boards<N, BASE>()) {
        for (auto d: DIRS) {
            u32 expected = Board<N>(board).reward(Dir(d));
            u32 res = reward(board, Dir(d));
            assert(res == expected);
        }
    }
    cout << "test_board_reward<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_board_count_empty(u8 (*count_empty)(const array<array<u8, N>, N>)) {
    for (auto &board: generate_boards<N, BASE>()) {
        u8 expected = Board<N>(board).count_empty();
        u8 res = count_empty(board);
        assert(res == expected);
    }
    cout << "test_board_count_empty<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_board_empty_mask(u16 (*empty_mask)(const array<array<u8, N>, N>)) {
    for (auto &board: generate_boards<N, BASE>()) {
        u16 expected = Board<N>(board).empty_mask();
        u16 res = empty_mask(board);
        assert(res == expected);
    }
    cout << "test_board_empty_mask<" << N << ", " << BASE << "> passed" << endl;
}

template<int N, int BASE>
void test_board_tuple_hash(u32 (*tuple_hash)(const array<array<u8, N>, N>, u16)) {
    for (auto &board: generate_boards<N, BASE>(100)) {
        for (u32 mask = 0; mask < power(2, N * N); mask++) {
            if (popcnt(mask) > 6) { continue; }
            u32 expected = Board<N>(board).tuple_hash(mask, BASE);
            u32 res = tuple_hash(board, mask);
            assert(res == expected);
        }
    }
    cout << "test_board_tuple_hash<" << N << ", " << BASE << "> passed" << endl;
}

double chi_squared_test(vector<pair<double, double>> &data) {
    double res = 0;
    for (auto [observed, expected]: data) {
        double rel_freq = (observed - expected) * (observed - expected) / expected;
        res += rel_freq;
    }
    return res;
}

double incomplete_gamma(double s, double x) {
    double sum = 0;
    double term = pow(x, s) * exp(-x) / s;
    double threshold = min(term, 1e-20);
    bool converged = false;
    for (int k = 1; k <= 200; k++) {
        if (term == 0) break;
        sum += term;
        term *= x / (s + double(k));
        if (term < threshold) {
            converged = true;
            break;
        }
    }
    if (!converged) {
        return tgamma(s);
    }
    return sum;
}

double p_value_from_chi_squared(double x, double k) {
    return 1 - incomplete_gamma(k / 2, x / 2) / tgamma(k / 2);
}

double p_from_z(double z_score) {
    return erfc(z_score / sqrt(2)) / 2;
}

double z_from_p(double p_value) {
    double lo = -20;
    double hi = 20;
    while (hi - lo > 1e-20) {
        double mid = (lo + hi) / 2;
        double p = p_from_z(mid);
        if (p > p_value) {
            if (lo == mid) break;
            lo = mid;
        } else {
            if (hi == mid) break;
            hi = mid;
        }
    }
    return (lo + hi) / 2;
}

double total_p_value(vector<double> &p_values) {
    double z_sum = 0;
    for (double p: p_values) {
        double z = z_from_p(p);
        z_sum += z;
    }
    double z_avg = z_sum / sqrt(double(p_values.size()));
    return p_from_z(z_avg);
}

template<int N, int BASE>
void test_board_fill(array<array<u8, N>, N> (*fill)(const array<array<u8, N>, N>)) {
    vector<double> p_values;
    for (array<array<u8, N>, N> board: generate_boards<N, BASE>(100)) {
        if (Board<N>(board).count_empty() == 0) { continue; }
        u8 empty = Board<N>(board).count_empty();
        vector<pair<double, double>> data(empty * 2);

        int k = 100000;
        for (int i = 0; i < empty; i++) {
            data[i].second = double(k) * (0.9 / double(empty));
            data[empty + i].second = double(k) * (0.1 / double(empty));
        }
        for (int t = 0; t < k; t++) {
            array<array<u8, N>, N> filled = fill(board);
            bool found = false;
            u8 index = 0;
            u8 value = 0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (board[i][j] != 0) {
                        assert(filled[i][j] == board[i][j]);
                        continue;
                    }
                    if (found) {
                        assert(filled[i][j] == 0);
                        continue;
                    }
                    if (filled[i][j] == 0) {
                        index++;
                        continue;
                    }
                    assert(filled[i][j] == 1 || filled[i][j] == 2);
                    found = true;
                    value = filled[i][j];
                }
            }
            assert(found);
            if (value == 1) {
                data[index].first++;
            } else {
                data[empty + index].first++;
            }
        }
        double chi_squared = chi_squared_test(data);
        int df = empty * 2 - 1;
        double p_value = p_value_from_chi_squared(chi_squared, double(df));
        p_values.push_back(p_value);
    }
    sort(p_values.begin(), p_values.end());
    double total_p = total_p_value(p_values);
    //cout << "p = " << total_p << endl;
    assert(total_p > 0.01);
    cout << "test_board_fill<" << N << ", " << BASE << "> passed: p = " << total_p << endl;
}

void run_tests() {
    /*if (false) {
        {
            test_line_hash<4, 16>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.hash(16);
            });
            test_line_hash<4, 18>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.hash(18);
            });
            test_line_hash<3, 16>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.hash(16);
            });
            test_line_hash<3, 11>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.hash(11);
            });
        }

        {
            test_line_left_move<4, 16>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.left_moved().to_array();
            });
            test_line_left_move<4, 18>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.left_moved().to_array();
            });
            test_line_left_move<3, 16>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.left_moved().to_array();
            });
            test_line_left_move<3, 11>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.left_moved().to_array();
            });
        }

        {
            test_line_right_move<4, 16>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.right_moved().to_array();
            });
            test_line_right_move<4, 18>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.right_moved().to_array();
            });
            test_line_right_move<3, 16>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.right_moved().to_array();
            });
            test_line_right_move<3, 11>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.right_moved().to_array();
            });
        }

        {
            test_line_reward<4, 16>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.reward();
            });
            test_line_reward<4, 18>([](array<u8, 4> line) {
                Line<4> l(line);
                return l.reward();
            });
            test_line_reward<3, 16>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.reward();
            });
            test_line_reward<3, 11>([](array<u8, 3> line) {
                Line<3> l(line);
                return l.reward();
            });
        }

        {
            test_board_move<4, 16>([](array<array<u8, 4>, 4> board, Dir dir) {
                Board<4> b(board);
                return b.moved(dir).to_array();
            });
            test_board_move<4, 18>([](array<array<u8, 4>, 4> board, Dir dir) {
                Board<4> b(board);
                return b.moved(dir).to_array();
            });
            test_board_move<3, 16>([](array<array<u8, 3>, 3> board, Dir dir) {
                Board<3> b(board);
                return b.moved(dir).to_array();
            });
            test_board_move<3, 11>([](array<array<u8, 3>, 3> board, Dir dir) {
                Board<3> b(board);
                return b.moved(dir).to_array();
            });
        }

        {
            test_board_reward<4, 16>([](array<array<u8, 4>, 4> board, Dir dir) {
                Board<4> b(board);
                return b.reward(dir);
            });
            test_board_reward<4, 18>([](array<array<u8, 4>, 4> board, Dir dir) {
                Board<4> b(board);
                return b.reward(dir);
            });
            test_board_reward<3, 16>([](array<array<u8, 3>, 3> board, Dir dir) {
                Board<3> b(board);
                return b.reward(dir);
            });
            test_board_reward<3, 11>([](array<array<u8, 3>, 3> board, Dir dir) {
                Board<3> b(board);
                return b.reward(dir);
            });
        }

        {
            test_board_count_empty<4, 16>([](array<array<u8, 4>, 4> board) {
                Board<4> b(board);
                return b.count_empty();
            });
            test_board_count_empty<4, 18>([](array<array<u8, 4>, 4> board) {
                Board<4> b(board);
                return b.count_empty();
            });
            test_board_count_empty<3, 16>([](array<array<u8, 3>, 3> board) {
                Board<3> b(board);
                return b.count_empty();
            });
            test_board_count_empty<3, 11>([](array<array<u8, 3>, 3> board) {
                Board<3> b(board);
                return b.count_empty();
            });
        }

        {
            test_board_empty_mask<4, 16>([](array<array<u8, 4>, 4> board) {
                Board<4> b(board);
                return b.empty_mask();
            });
            test_board_empty_mask<4, 18>([](array<array<u8, 4>, 4> board) {
                Board<4> b(board);
                return b.empty_mask();
            });
            test_board_empty_mask<3, 16>([](array<array<u8, 3>, 3> board) {
                Board<3> b(board);
                return b.empty_mask();
            });
            test_board_empty_mask<3, 11>([](array<array<u8, 3>, 3> board) {
                Board<3> b(board);
                return b.empty_mask();
            });
        }

        {
            test_board_tuple_hash<4, 16>([](array<array<u8, 4>, 4> board, u16 mask) {
                Board<4> b(board);
                return b.tuple_hash(mask, 16);
            });
            test_board_tuple_hash<4, 18>([](array<array<u8, 4>, 4> board, u16 mask) {
                Board<4> b(board);
                return b.tuple_hash(mask, 18);
            });
            test_board_tuple_hash<3, 16>([](array<array<u8, 3>, 3> board, u16 mask) {
                Board<3> b(board);
                return b.tuple_hash(mask, 16);
            });
            test_board_tuple_hash<3, 11>([](array<array<u8, 3>, 3> board, u16 mask) {
                Board<3> b(board);
                return b.tuple_hash(mask, 11);
            });
        }

        {
            test_board_fill<4, 16>([](array<array<u8, 4>, 4> board) {
                Board<4> b(board);
                return b.filled().to_array();
            });
            test_board_fill<4, 18>([](array<array<u8, 4>, 4> board) {
                Board<4> b(board);
                return b.filled().to_array();
            });
            test_board_fill<3, 16>([](array<array<u8, 3>, 3> board) {
                Board<3> b(board);
                return b.filled().to_array();
            });
            test_board_fill<3, 11>([](array<array<u8, 3>, 3> board) {
                Board<3> b(board);
                return b.filled().to_array();
            });
        }
    }
    if (true) {
        test_board_move<3, 11>([](const array<array<u8, 3>, 3> board, Dir d) {
            u64 b = from_array(board);
            move_board(b, d);
            return to_array(b);
        });
        test_board_reward<3, 11>([](const array<array<u8, 3>, 3> board, Dir d) {
            u64 b = from_array(board);
            return u32(get_reward(b, d));
        });
        test_board_count_empty<3, 11>([](const array<array<u8, 3>, 3> board) {
            u64 b = from_array(board);
            return count_empty(b);
        });
        test_board_fill<3, 11>([](const array<array<u8, 3>, 3> board) {
            u64 b = from_array(board);
            fill_board(b);
            return to_array(b);
        });
    }*/
    if (true) {
        test_board_move<4, 16>([](const array<array<u8, 4>, 4> board, Dir d) {
            u64 b = from_matrix(board);
            move_board(b, d);
            return to_matrix(b);
        });
        test_board_reward<4, 16>([](const array<array<u8, 4>, 4> board, Dir d) {
            u64 b = from_matrix(board);
            return get_reward(b, d);
        });
        test_board_count_empty<4, 16>([](const array<array<u8, 4>, 4> board) {
            u64 b = from_matrix(board);
            return count_empty(b);
        });
        test_board_fill<4, 16>([](const array<array<u8, 4>, 4> board) {
            u64 b = from_matrix(board);
            fill_board(b);
            return to_matrix(b);
        });
    }
    if (true) {
        test_line_left_move<4, 15>([](array<u8, 4> line) {
            u16 l = from_array(line);
            return to_array(l ^ left_0_4[l]);
        });
        test_line_right_move<4, 15>([](array<u8, 4> line) {
            u16 l = from_array(line);
            return to_array(l ^ right_0_4[l]);
        });
    }
}