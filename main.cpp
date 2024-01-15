#include "testing.h"
#include "endgame_bruteforce.h"
#include "algorithm.h"
#include "stats.h"

void init() {
    auto start = time_now();

    init_rng(SEED);
    init_zeroes();
    init_moves_0();
    init_moves_123();

    cout << "Init time: " << time_since(start) / 1e6 << " s" << endl << endl;
}

s_t play_random_game() {
    u64 board = 0;
    s_t score = 0;
    fill_board(board);
    fill_board(board);
    while (!game_over(board)) {
        Dir d = Dir(random(4) + 1);
        u64 afterstate = moved_board(board, d);
        if (afterstate == board) { continue; }
        score += get_reward(board, d);
        board = afterstate;
        fill_board(board);
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

void perf_test(u64 n) {
    auto start = time_now();
    r_t sum = 0;
    for (u32 i = 0; i < n; ++i) {
        sum += r_t(play_random_game());
    }
    cout << "Average score: " << sum / r_t(n) << endl;
    cout << "Time: " << time_since(start) / 1e6 << endl << endl;
}

void perf_test_general(u64 n) {
    auto start = time_now();
    r_t sum = 0;
    for (u32 i = 0; i < n; ++i) {
        sum += r_t(play_random_game_general<4>());
    }
    cout << "Average score: " << sum / r_t(n) << endl;
    cout << "Time: " << time_since(start) / 1e6 << endl << endl;
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

void run() {
    init();

    //run_tests();
    //cout << endl;
    //perf_test(10000);
    //perf_test_general(10000);

    //load_packed_weights("final");
    //const u32 threads = thread::hardware_concurrency();
    //run_testing_episodes(1000, threads);

    /*Endgame eg(0xFFFFFFF700000000ull);
    u64 b = 0x101012007FFFFFFFull;
    eg.init_goals(10);
    eg.bruteforce_states();
    u8 b_i = eg.transform_index(b);
    print_board(0xFFFFFFF700000000ull);
    print_board(b);
    cout << (int) b_i << endl;
    cout << eg.general_state_eval(b, b_i) << endl;*/

    /*r_t avg = 0;
    for (u32 i = 0; i < 10; ++i) {
        avg += play_game();
    }
    cout << "Average score: " << avg / 10 << endl;*/

    /*auto start = time_now();
    Endgame eg(0xFFFFFFF700000000ull);
    eg.init_goals(threads);
    cout << "Init time: " << time_since(start) / 1e6 << endl;
    start = time_now();
    eg.eval_all_states();
    cout << "Eval time: " << time_since(start) / 1e6 << endl;
    eg.play_game();*/

    load_packed_weights("stage1", tuples_4_stage_1);
    load_packed_weights("stage2", tuples_4_stage_2);
}

Endgame endgame(0xFFFFFF8000000000ull);

void run2() {
    init();
    //load_packed_weights("stage1", tuples_4_stage_1);
    //load_packed_weights("stage2", tuples_4_stage_2);

    //print largest weight in tuples_4_stage_2
    /*r_t max_weight = 0;
    r_t min_weight = 1e6;
    for (const auto &tuple: tuples_4_stage_2) {
        for (const auto &weight: tuple.weights) {
            max_weight = max(max_weight, weight);
            min_weight = min(min_weight, weight);
        }
    }
    cout << max_weight << endl;
    cout << min_weight << endl;*/

    //endgame.load_packed_evals("8-9-prob");

    /*run_algorithm_episodes(100, 10, [](const u64 board, NTuple &tuples) {
        return eval_state(board, tuples).dir;
        //return expectimax_limited_states(board, 100, 0.01, tuples).dir;
        //return expectimax_limited_states(board, 500, 0.01, tuples).dir;
    });*/

    /*run_algorithm_episodes(10000, 10, [](const u64 board, NTuple &tuples) {
        Eval eval = endgame.eval_board(board);
        if (eval.dir != None && eval.eval > 0.01) {
            return eval.dir;
        }
        return eval_state(board, tuples).dir;
        //return eval_state(board, tuples).dir;
        //return expectimax_limited_states(board, 100, 0.01, tuples).dir;
        //return expectimax_limited_states(board, 500, 0.01, tuples).dir;
    });*/

    /*auto start = time_now();
    Endgame eg(0xFFFFFF8000000000ull);
    cout << endl;

    if (0) {
        eg.init_goal_states();
        cout << endl;
        cnt = 0;
        eg.bruteforce_states();
        cout << cnt << endl;
        cout << endl;
        eg.save_packed_evals("8-9-eval");
        cout << endl;
    } else {
        eg.load_packed_evals("8-9-prob");
        cout << endl;
    }
    //eg.print_evals();
    //cout << endl;
    cout << eg.get_state_eval(0x1220100008FFFFFFull) << endl;
    cout << eg.get_afterstate_eval(0x1220100008FFFFFFull) << endl;
    //cout << add_weights(0xEBCDA98700000001ull, tuples_4_stage_2) << endl;
    //cout << expectimax_limited_states(0xEBCDA98700000001ull, 1000000, 0.01, tuples_4_stage_2).eval << endl;*/

    /*cnt = 0;
    u64 total_score = 0;
    for (u32 i = 0; i < 100000; ++i) {
        u64 b = 0xEBCDA98700000001ull;
        u32 score = 0;
        while (true) {
            Dir d = eg.best_dir(b);
            score += get_reward(b, d);
            u64 b2 = moved_board(b, d);
            if (b2 == b) { break; }
            b = b2;
            fill_board(b);
            if (eg.is_goal_state_2(b)) {
                ++cnt;
                break;
            }
        }
        total_score += score;
    }
    cout << r_t(cnt) / 100000 << endl;
    cout << r_t(total_score) / 100000 << endl;*/


    //cout << get_newtiles_score(0, 10, 32) << endl;
    //cout << get_newtiles_score(0, 6, 1024) << endl;
    //cout << get_newtiles_score(65344, 6, 3) << endl;
    //cout << endl;
    //print_newtile_score_for_all_additions(6);
    // [\d]+\s[\d]*\.[\d]*\n
    // ->[\d]+: [\d.]*

    //print_all_prob_score_stuff();

    // space_perf_test(40);

    //cout << (int) calculate_space(0b011101100000000ull, 8) << endl;

    for (u8 G = 6; G <= 11; ++G) {
        for (u8 space = 0; space <= 13; ++space) {
            r_t GB = power(G + 1, space) / 1e9 * 4;
            u32 cases = print_bruteforce_stats(G, space);
            if (cases > 0) {
                cout << "G=" << u32(G) << " space=" << u32(space) << " GB=" << GB << endl;
                cout << endl;
            }
        }
    }
    //calculate_stage_starts(8, 9);
}

int main() {
    srand(42);
    if (REDIRECT) {
        ofstream file("../output.log", ios_base::out | ios_base::trunc);
        streambuf *consoleBuffer = cout.rdbuf();
        streambuf *fileBuffer = file.rdbuf();
        cout.rdbuf(fileBuffer);

        //run();
        run2();

        cout.rdbuf(consoleBuffer);
        file.close();
    } else {
        //run();
        run2();
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

