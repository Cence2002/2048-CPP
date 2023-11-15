#include "assets.h"
#include "testing.h"

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

#include "testing.h"

void run_tests() {
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
        test_board_move<4, 16>([](array<array<u8, 4>, 4> board, Direction dir) {
            Board<4> b(board);
            return b.moved(dir).to_array();
        });
        test_board_move<4, 18>([](array<array<u8, 4>, 4> board, Direction dir) {
            Board<4> b(board);
            return b.moved(dir).to_array();
        });
        test_board_move<3, 16>([](array<array<u8, 3>, 3> board, Direction dir) {
            Board<3> b(board);
            return b.moved(dir).to_array();
        });
        test_board_move<3, 11>([](array<array<u8, 3>, 3> board, Direction dir) {
            Board<3> b(board);
            return b.moved(dir).to_array();
        });
    }

    {
        test_board_reward<4, 16>([](array<array<u8, 4>, 4> board, Direction dir) {
            Board<4> b(board);
            return b.reward(dir);
        });
        test_board_reward<4, 18>([](array<array<u8, 4>, 4> board, Direction dir) {
            Board<4> b(board);
            return b.reward(dir);
        });
        test_board_reward<3, 16>([](array<array<u8, 3>, 3> board, Direction dir) {
            Board<3> b(board);
            return b.reward(dir);
        });
        test_board_reward<3, 11>([](array<array<u8, 3>, 3> board, Direction dir) {
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

int main() {
    srand(43);

    /*auto start_time = time_now();
    u32 N = 1000;
    float avg_score = 0;
    for (int i = 0; i < N; i++) {
        avg_score += play_random_game<4>();
    }
    cout << "Average score: " << avg_score / N << endl;
    cout << "Time: " << time_since(start_time) / 1e6 << endl;*/

    run_tests();


    return 0;
}
