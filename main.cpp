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

template<int N>
struct Line {
    array<u8, N> cells;

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
            cout << cells[i] << " ";
        }
        cout << endl;
    }
};

template<int N>
struct Board {
    array<Line<N>, N> rows;

    Board() {
        rows.fill(Line<N>());
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

    void fill() {
        u8 empty = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; ++j) {
                if (rows[i][j] == 0) { empty++; }
            }
        }
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

    void print() {
        for (int i = 0; i < N; i++) {
            rows[i].print();
        }
        cout << endl;
    }
};

template<int N>
struct Game {
    Board<N> board;
    u32 score;

    Game() : board(Board<N>()), score(0) {
        board.fill();
        board.fill();
    }

    Game(const Board<N> &board, u32 score) : board(board), score(score) {}

    Game(const Game &game) : board(game.board), score(game.score) {}

    Game &operator=(const Game &game) {
        board = game.board;
        score = game.score;
        return *this;
    }

    bool move(Direction dir) {
        Board<N> afterstate = board.moved(dir);
        if (afterstate == board) { return false; }
        score += board.reward(dir);
        board = afterstate.filled();
        return true;
    }

    Game<N> moved(Direction dir) {
        Game<N> res(*this);
        res.move(dir);
        return res;
    }

    bool is_over() {
        for (int i = 0; i < 4; i++) {
            if (board.moved(Direction(i)) != board) { return false; }
        }
        return true;
    }

    void print() {
        board.print();
        cout << score << endl;
    }
};

template<int N>
u32 play_random_game() {
    Game<N> game;
    while (!game.is_over()) {
        game.move(Direction(random(4) + 1));
    }
    return game.score;
}

int main() {
    srand(42);

    auto start_time = time_now();
    u32 N = 10000;
    float avg_score = 0;
    for (int i = 0; i < N; i++) {
        avg_score += play_random_game<4>();
    }
    cout << "Average score: " << avg_score / N << endl;
    cout << "Time: " << time_since(start_time) / 1e6 << endl;

    return 0;
}
