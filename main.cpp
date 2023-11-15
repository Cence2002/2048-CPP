#include "assets.h"
#include "general_board.h"
#include "testing.h"

template<int N>
u32 play_random_game() {
    Game<N> game;
    while (!game.is_over()) {
        game.move(Direction(random(4) + 1));
    }
    return game.score;
}

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

int main() {
    srand(42);
    run_tests();

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
