#include "testing.h"
#include "learn.h"
#include "endgame_bruteforce.h"
#include "3x3_bruteforce.h"

void init() {
    auto start = time_now();

    init_zeroes();
    init_moves_0<4>();
    init_moves_0<3>();
    init_moves_123();

    for (auto &t: tuples_4) {
        t.weights.fill(tuple_init / (8 * tuples_size_4));
    }
    for (auto &t: tuples_3) {
        t.weights.fill(tuple_init / (8 * tuples_size_3));
    }

    cout << "Init time: " << time_since(start) / 1e6 << endl;
}

/*template<u8 N>
void save_all_weights(const string &ts_str) {
    const string dir = "../weights_backups";
    if constexpr (N == 4) {
        for (u8 i = 0; i < tuples_size_4; ++i) {
            auto &t = tuples_4[i];
            const string filename = "weights_" + t.name + "_" + ts_str + ".bin";
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            save_array(dir + "/" + filename, (char *) &t.weights, size);
        }
    } else {
        for (u8 i = 0; i < tuples_size_3; ++i) {
            auto &t = tuples_3[i];
            const string filename = "weights_" + t.name + "_" + ts_str + ".bin";
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            save_array(dir + "/" + filename, (char *) &t.weights, size);
        }
    }
    cout << endl;
}

template<u8 N>
void load_all_weights(const string &ts_str) {
    const string dir = "../weights_backups";
    if constexpr (N == 4) {
        for (u8 i = 0; i < tuples_size_4; ++i) {
            auto &t = tuples_4[i];
            const string filename = "weights_" + t.name + "_" + ts_str + ".bin";
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            load_array(dir + "/" + filename, (char *) &t.weights, size);
        }
    } else {
        for (u8 i = 0; i < tuples_size_3; ++i) {
            auto &t = tuples_3[i];
            const string filename = "weights_" + t.name + "_" + ts_str + ".bin";
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            load_array(dir + "/" + filename, (char *) &t.weights, size);
        }
    }
    cout << endl;
}*/


template<u8 N>
s_t play_random_game() {
    u64 board = 0;
    s_t score = 0;
    fill_board<N>(board);
    fill_board<N>(board);
    while (!game_over<N>(board)) {
        Dir d = Dir(random(4) + 1);
        u64 afterstate = moved_board<N>(board, d);
        if (afterstate == board) { continue; }
        score += get_reward<N>(board, d);
        board = afterstate;
        fill_board<N>(board);
    }
    return score;
}

template<u8 N>
s_t play_random_game_general() {
    Game<N> game;
    while (!game.is_over()) {
        game.move(Dir(random(4) + 1));
    }
    return game.score;
}

void perf_test(u64 N) {
    auto start = time_now();
    r_t sum = 0;
    for (u32 i = 0; i < N; ++i) {
        sum += r_t(play_random_game<4>());
    }
    cout << "Average score: " << sum / r_t(N) << endl;
    cout << "Time: " << time_since(start) / 1e6 << endl << endl;
}

void perf_test_general(u64 N) {
    auto start = time_now();
    r_t sum = 0;
    for (u32 i = 0; i < N; ++i) {
        sum += r_t(play_random_game_general<4>());
    }
    cout << "Average score: " << sum / r_t(N) << endl;
    cout << "Time: " << time_since(start) / 1e6 << endl << endl;
}

template<u8 N>
s_t play_game() {
    u64 board = 0;
    s_t score = 0;
    fill_board<N>(board);
    fill_board<N>(board);
    while (!game_over<N>(board)) {
        const auto [dir, eval, reward, afterstate] =
                limited_states_player<N>(board, 1000);
        if (dir == None) { break; }
        score += reward;
        board = afterstate;
        fill_board<N>(board);
    }
    return score;
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
        test_board_move<3, 11>([](const array<array<u8, 3>, 3> board, Dir d) {
            u64 b = from_matrix<3>(board);
            move_board<3>(b, d);
            return to_matrix<3>(b);
        });
        test_board_reward<3, 11>([](const array<array<u8, 3>, 3> board, Dir d) {
            u64 b = from_matrix<3>(board);
            return get_reward<3>(b, d);
        });
        test_board_count_empty<3, 11>([](const array<array<u8, 3>, 3> board) {
            u64 b = from_matrix<3>(board);
            return count_empty<3>(b);
        });
        test_board_fill<3, 11>([](const array<array<u8, 3>, 3> board) {
            u64 b = from_matrix<3>(board);
            fill_board<3>(b);
            return to_matrix<3>(b);
        });
    }
    if (true) {
        test_board_move<4, 16>([](const array<array<u8, 4>, 4> board, Dir d) {
            u64 b = from_matrix<4>(board);
            move_board<4>(b, d);
            return to_matrix<4>(b);
        });
        test_board_reward<4, 16>([](const array<array<u8, 4>, 4> board, Dir d) {
            u64 b = from_matrix<4>(board);
            return get_reward<4>(b, d);
        });
        test_board_count_empty<4, 16>([](const array<array<u8, 4>, 4> board) {
            u64 b = from_matrix<4>(board);
            return count_empty<4>(b);
        });
        test_board_fill<4, 16>([](const array<array<u8, 4>, 4> board) {
            u64 b = from_matrix<4>(board);
            fill_board<4>(b);
            return to_matrix<4>(b);
        });
    }
    if (true) {
        test_line_left_move<4, 15>([](array<u8, 4> line) {
            u16 l = from_array<4>(line);
            return to_array<4>(l ^ left_0_4[l]);
        });
        test_line_right_move<4, 15>([](array<u8, 4> line) {
            u16 l = from_array<4>(line);
            return to_array<4>(l ^ right_0_4[l]);
        });
    }
}

template<u8 N>
void fixed_learn(r_t LR, u32 episodes, u32 training_games, u32 testing_games, u8 threads) {
    learning_rate = LR;
    run_learning<N>(episodes, training_games, testing_games, threads);
    string ts_str = get_time_str();
    cout << "Timestamp: " << ts_str << endl;
    save_packed_weights<N>(ts_str);
}

template<u8 N>
void run() {
    //load_all_weights<N>("1216-21-10-12");
    //run_testing_episodes<N>(1000);

    //interactive_learn(1000, 100);
    //for (u32 i = 0; i < 2; ++i) { fixed_learn<N>(0.1, 10, 10000, 1000); }

    //IMPLEMENT run_learning<4>(10, 10000, 1000, 0.75);
    //fixed_learn<N>(0.75, 10, 10000, 1000);

    //constexpr u64 board = 0xFFFFFFF600000000ull;
    //cout << Endgame<board>.get_size() << endl;
    //cout << get_size(board) * 4.0f / 1e6 << endl;
    //cout << (int) get_G(board) << endl;
    //cout << to_hash(board, 0xFFFFFFF612500000ull) << endl;
    //cout << endl;
    //cout << hex << from_hash<board>(to_hash<board>(0xFFFFFFF610000000ull)) << endl;

    //cout << endgame.prob_afterstate(0xFFFFFFF600000000ull) << endl;
    //cout << endgame.prob_state(0xFFF0FFF0F7422222ull) << endl;

    /*u64 temp = 0xFFFFFFF700000000ull;
    Endgame endgame_temp(temp);
    endgame_temp.init_goals();
    u64 tem2 = 0xFFFFFFF734561122ull;
    print_board<4>(tem2);
    cout << endgame_temp.prob_state(tem2) << endl;
    cout << (int) endgame_temp.best_dir(tem2) << endl;
    print_board<4>(moved_board<4>(tem2, endgame_temp.best_dir(tem2)));*/

    /*
    //replace 6 with 7 or 8 to get example probabilities
    u64 board = 0xFFFFFF8000000000ull;
    //board = 0xFFF0FFF0F7000000ull;
    board = 0xFFFFFF8000000000ull;
    print_board<4>(board);
    Endgame endgame(board);
    endgame.init_goals();
    vector<u8> packed = endgame.pack_dirs();

    u64 b = 0xFFFFFF8000000002ull;
    while (true) {
        print_board<4>(b);
        Dir d = endgame.unpack_dir(packed, endgame.to_hash(b));
        if (endgame.is_goal_state(b)) {
            cout << "GOAL" << endl;
            break;
        }

        u64 b2 = moved_board<4>(b, d);
        if (b2 == b) {
            cout << "GAME OVER" << endl;
            break;
        }
        b = b2;
        fill_board<4>(b);
        //wait(500000);
    }
    */
}

template<u8 N>
void run2() {
    init();
    //load_packed_weights<N>("1229_114706");
    const u32 threads = thread::hardware_concurrency();
    cout << "Number of cores: " << threads << endl;
    for (u32 i = 0; i < 1; ++i) {
        fixed_learn<N>(0.1, 10, 100000, 10000, threads);
    }
}

int main() {
    srand(42);

    //run_tests();
    //cout << endl;
    //perf_test(10000);
    //perf_test_general(10000);

    if (REDIRECT) {
        ofstream file("../output.log", std::ios_base::out | std::ios_base::trunc);
        streambuf *consoleBuffer = cout.rdbuf();
        streambuf *fileBuffer = file.rdbuf();
        cout.rdbuf(fileBuffer);

        //run<4>();
        run2<4>();

        cout.rdbuf(consoleBuffer);
        file.close();
    } else {
        //run<4>();
        run2<4>();
    }

    return 0;
}

// To brute force for 3x3:
// goal of player: maximizing or minimizing (or random)
// goal of game: maximizing or minimizing or random (with arbitrary probability for 2 instead of 0.9)
// metrics: score, number of moves, probability of reaching a certain tile (1-10), entropy(?), TODO: extend
// extra: ratio of reachable positions compared to all

// Average sum per moves: n/(0.9*2+0.1*4) =>
// => n/2.2
// Average sum
// Average score for 2^2: (0.45*4+0.1*0)/(0.45+0.1) =>
// => 36/11 (= 3.27)
// Average score for 2^n: f(2)=36/11,f(n)=2*f(n-1)+2^n =>
// => (n-13/11)*2^n

// Score for 2 = 4 => 3.3
// Score for 3 = 8 => 14.5
// Score for 4 = 16 => 45.1
// Score for 5 = 32 => 122.2
// Score for 6 = 64 => 308.4
// Score for 7 = 128 => 744.7
// Score for 8 = 256 => 1745.5
// Score for 9 = 512 => 4002.9
// Score for A = 1024 => 9029.8
// Score for B = 2048 => 20107.6
// Score for C = 4096 => 44311.3
// Score for D = 8192 => 96814.5
// Score for E = 16384 => 210013.1
// Score for F = 32768 => 452794.2
// Score for G = 65536 => 971124.4
// Score for H = 131072 => 2073320.7

// Moves for N = 2^N => 2^N/2.2

