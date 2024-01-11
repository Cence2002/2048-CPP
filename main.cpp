#include "testing.h"
#include "algorithm.h"
#include "endgame_bruteforce.h"

void init() {
    auto start = time_now();

    init_rng(SEED);
    init_zeroes();
    init_moves_0();
    init_moves_123();

    cout << "Init time: " << time_since(start) / 1e6 << endl << endl;
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

template<u8 N>
void run() {
    //load_all_weights("1216-21-10-12");
    //run_testing_episodes(1000);

    //interactive_learn(1000, 100);
    //for (u32 i = 0; i < 2; ++i) { fixed_learn(0.1, 10, 10000, 1000); }

    //IMPLEMENT run_learning(10, 10000, 1000, 0.75);
    //fixed_learn(0.75, 10, 10000, 1000);

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
    print_board(tem2);
    cout << endgame_temp.prob_state(tem2) << endl;
    cout << (int) endgame_temp.best_dir(tem2) << endl;
    print_board(moved_board(tem2, endgame_temp.best_dir(tem2)));*/

    /*
    //replace 6 with 7 or 8 to get example probabilities
    u64 board = 0xFFFFFF8000000000ull;
    //board = 0xFFF0FFF0F7000000ull;
    board = 0xFFFFFF8000000000ull;
    print_board(board);
    Endgame endgame(board);
    endgame.init_goals();
    vector<u8> packed = endgame.pack_dirs();

    u64 b = 0xFFFFFF8000000002ull;
    while (true) {
        print_board(b);
        Dir d = endgame.unpack_dir(packed, endgame.to_hash(b));
        if (endgame.is_goal_state(b)) {
            cout << "GOAL" << endl;
            break;
        }

        u64 b2 = moved_board(b, d);
        if (b2 == b) {
            cout << "GAME OVER" << endl;
            break;
        }
        b = b2;
        fill_board(b);
        //wait(500000);
    }
    */
}

void run2() {
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

    run_algorithm_episodes(10, 10, [](const u64 board, NTuple &tuples) {
        /*if (++cnt > cnt_2) {
            cnt_2 += 2000;
            //cout << "Deci games: " << cnt / 2000 << endl;
        }*/
        //print_board(board);
        return expectimax_limited_states(board, 10000, 0.01, tuples).dir;
        //const u8 depth = 3;
        //return expectimax_limited_depth_prob(board, depth, 1 / get_min_prob(depth, 0.05), tuples).dir;
        //return eval_state(board, tuples).dir;
    });
    /*
        - - 1 6
        1 3 3 8
        - 2 5 B
        2 1 3 C
     copy this board
     */

    u64 board = 0x00161338025B213Cull;
    /*
        - 1 8 9
        - 1 7 B
        1 2 6 C
        2 3 4 D
     copy this board
     */
    board = 0x0189017B126C234Dull;
    /*print_board(board);
    for (u8 depth = 0; depth <= 5; ++depth) {
        cnt = 0;
        cnt_adds = 0;
        cnt_state = 0;
        cnt_afterstate = 0;
        cnt_probs.clear();
        auto start = time_now();
        r_t used_prob = 1 / get_min_prob(depth, 0.05);
        cout << expectimax_limited_depth_prob(board, depth, used_prob, tuples_4_stage_1).eval << endl;
        cout << "Depth: " << (int) depth << endl;
        cout << "Time: " << time_since(start) / 1e6 << endl;
        cout << "States: " << cnt_state << " (" << cnt_state / time_since(start) * 1e6 << " states/s)" << endl;
        cout << "Afterstates: " << cnt_afterstate << " (" << cnt_afterstate / time_since(start) * 1e6 << " afterstates/s)" << endl;
        cout << "Total: " << cnt_adds << " (" << cnt_adds / time_since(start) * 1e6 << " states/s)" << endl;
        cout << "Small prob: " << cnt << endl;
        cout << "Max prob: " << used_prob << endl;
        cout << "Depths:" << endl;
        vector<pair<u8, r_t>> all_depths;
        for (const auto &[depth, cnt]: cnt_depths) {
            all_depths.emplace_back(depth, cnt);
        }
        sort(all_depths.begin(), all_depths.end(), [](const auto &a, const auto &b) {
            return a.first > b.first;
        });
        for (const auto &p: all_depths) {
            cout << (int) p.first << " " << p.second << endl;
        }
        cout << "Probs:" << endl;
        vector<pair<r_t, u64>> all_probs;
        for (const auto &[prob, cnt]: cnt_probs) {
            all_probs.emplace_back(prob, cnt);
        }
        sort(all_probs.begin(), all_probs.end(), [](const auto &a, const auto &b) {
            return a.first > b.first;
        });
        for (const auto &p: all_probs) {
            cout << used_prob / p.first << " " << p.second << endl;
        }
        cout << endl << endl;
    }*/

    //run_algorithm_episodes(10, 10, [](const u64 board, NTuple &tuples) {
    //    return expectimax_limited_states(board, 100, tuples).dir;
    //});

    /*vector<pair<u32, r_t>> differences;
    for (u32 i = 0; i < tuples_4_stage_1[0].weights.size(); ++i) {
        differences.emplace_back(i, abs(tuples_4_stage_1[0][i] - tuples_4_stage_2[0][i]));
    }
    sort(differences.begin(), differences.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
    for (u32 i = 0; i < 100; ++i) {
        cout << hex << differences[i].first << dec << " " << differences[i].second << endl;
    }*/

    //auto occurences = count_occurrences(15, 100);
    //auto sorted = print_mask_probs(occurences);
    //for (const auto &p: sorted) {
    //    cout << hex << p.first << dec << endl;
    //    print_board_probs(occurences[p.first]);
    //}
}

template<u8 N>
void run3() {
    init();
    load_packed_weights("final", tuples_4_stage_1);

    //run_tests();
    //perf_test(10000);
    //perf_test_general(10000);

    const u32 threads = thread::hardware_concurrency();
    cout << "Number of cores: " << threads << endl;

    //run_algorithm_episodes(1000, 10, [](const u64 board) {
    //    return eval_state(board).dir;
    //});

    //run_algorithm_episodes(20, 10, [](const u64 board) {
    //    return expectimax_limited_states(board, 10).dir;
    //});

    //run_algorithm_episodes(1, 0, [](const u64 board) {
    //    print_board(board);
    //    return expectimax_limited_depth_prob(board, 4, 1e10).dir;
    //});

    //run_algorithm_episodes(1200, 10, [](const u64 board) {
    //    return expectimax_limited_states(board, 250).dir;
    //});

    //run_algorithm_episodes(100, 10, [](const u64 board) {
    //    return expectimax_limited_depth_prob(board, 3, 1e10).dir;
    //});

    //const u64 board = 0x2021000000000000ull;
    //print_board(board);
    //print_dir(expectimax_limited_states(board, 100).dir);
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
        //run3();

        cout.rdbuf(consoleBuffer);
        file.close();
    } else {
        //run();
        run2();
        //run3();
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

