#include "endgame_bruteforce.h"
#include "algorithm.h"
//#include "testing.h"
//#include "stats.h"

void init() {
    auto start = time_now();

    init_rng(SEED);
    init_zeroes();
    init_moves_0();
    init_moves_123();

    cout << "Threads: " << thread::hardware_concurrency() << endl;

    cout << "Init time: " << time_since(start) / 1e6 << " s" << endl << endl;
}

/*s_t play_random_game() {
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
}*/

void perf_test_eval(const u64 n) {
    auto start = time_now();
    u64 moves = 0;
    r_t sum = 0;
    for (u32 i = 0; i < n; ++i) {
        u64 board = 0;
        fill_board(board);
        fill_board(board);
        while (true) {
            Eval eval = eval_state(board, tuples_4_stage_1);
            if (eval.dir == None) { break; }
            ++moves;
            sum += eval.reward;
            board = eval.afterstate;
            fill_board(board);
        }
    }
    cout << "Average moves: " << moves / r_t(n) << endl;
    cout << "Average score: " << sum / r_t(n) << endl;
    cout << "Time: " << time_since(start) / 1e6 << endl << endl;
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

    //load_packed_weights("stage1", tuples_4_stage_1);
    //load_packed_weights("stage2", tuples_4_stage_2);
}

r_t get_goal_value(const r_t cumulative_score_gain, const r_t reaching_prob, const r_t scale) {
    return cumulative_score_gain / reaching_prob * scale;
}

void endgameG8S9_prob() {
    /*Endgame endgame({
                            0xFFFFFF8000000000ull,
                            0xFFF0FF8F00000000ull,
                            0x0FFFFF80F0000000ull,
                            0x0FF0FF8FF0000000ull,
                            0xFFFF8FF000000000ull,
                            0xFFF08FFF00000000ull,
                            0x0FFFFFF080000000ull,
                    }); // 0.841903*/
    /*Endgame endgame({
                            0xFFFFFF8000000000ull,
                            0xFFF0FF8F00000000ull,
                            0x0FFFFF80F0000000ull,
                            0x0FF0FF8FF0000000ull,
                            0xFFFF8FF000000000ull,
                            0xFFF08FFF00000000ull,
                    }); // 0.841902*/
    Endgame endgame({
                            0xFFFFFF8000000000ull,
                            0xFFFF8FF000000000ull,
                            0xFFF08FFF00000000ull,
                            0x0FFFFF80F0000000ull,
                            0x0FFFF8FF00000000ull
                    }); // 0.841384
    //Endgame endgame({0xFFFFFF8000000000ull, 0xFFFF8FF000000000ull, 0xFFF08FFF00000000ull}); // 0.815005
    //Endgame endgame({0xFFFFFF8000000000ull, 0xFFFF8FF000000000ull}); // 0.752513
    //Endgame endgame({0xFFFFFF8000000000ull}); // 0.733328
    endgame.load_values("8-9-prob-5");
    //endgame.init_goal_states(1);
    //endgame.bruteforce_values();
    cout << endgame.get_state_value(0xFFFFFF8000000120ull) << endl;
    cout << endgame.get_state_value(0xFFFF8FF000000120ull) << endl;
    //endgame.save_values("8-9-prob-7");
    //endgame.print_known_ratios();
}

void endgame_G8S10_prob() {
    Endgame endgame({0xFFFFF80000000000ull, 0xFFFF0F8000000000ull});
    //Endgame endgame({0xFFFFF80000000000ull, 0xFFFF0F8000000000ull}); // 0.986459
    //Endgame endgame({0xFFFFF80000000000ull}); // 0.98588
    //endgame.load_values("8-10-prob-2");
    endgame.init_goal_states(1);
    endgame.bruteforce_values();
    cout << endgame.get_state_value(0xFFFFF82100000120ull) << endl;
    cout << endgame.get_state_value(0xFFFF2F8100000120ull) << endl;
    endgame.save_values("8-10-prob-3");
}

void endgameG8S9_eval() {
    Endgame endgame({
                            0xFFFFFF8000000000ull,
                            0xFFFF8FF000000000ull,
                            0xFFF08FFF00000000ull,
                            0x0FFFFF80F0000000ull,
                            0x0FFFF8FF00000000ull
                    });
    //endgame.load_values("8-9-eval-5");
    const r_t goal_value = get_goal_value(410887, 0.5682, 1.2);
    cout << goal_value << endl;
    endgame.init_goal_states(goal_value);
    endgame.bruteforce_values();
    cout << endgame.get_state_value(0xFFFFFF8000000120ull) << endl;
    cout << endgame.get_state_value(0xFFFF8FF000000120ull) << endl;
    endgame.save_values("8-9-eval-5");
}

void endgame_G8S10_eval() {
    Endgame endgame({0xFFFFF80000000000ull, 0xFFFF0F8000000000ull});
    //endgame.load_values("8-10-prob-2");
    const r_t goal_value = get_goal_value(410887, 0.5682, 1.2);
    cout << goal_value << endl;
    endgame.init_goal_states(goal_value);
    endgame.bruteforce_values();
    cout << endgame.get_state_value(0xFFFFF82100000120ull) << endl;
    cout << endgame.get_state_value(0xFFFF2F8100000120ull) << endl;
    endgame.save_values("8-10-eval-2");
}

vector<Endgame> endgames;

void run2() {
    init();

    load_packed_weights("stage1", tuples_4_stage_1);
    load_packed_weights("stage2", tuples_4_stage_2);
    cout << endl;

    run_algorithm_episodes(200, 200, [](const u64 board, NTuple &tuples) {
        //return eval_state(board, tuples).dir;
        //print_board(board);
        //return expectimax_limited_evals(downgraded(board), 15000, 0.02, tuples).dir;
        //return eval_state(downgraded(board), tuples).dir;
        return expectimax_limited_evals(downgraded(board), 100, 0.02, tuples).dir;
    });

    //endgames.push_back(Endgame({0xFFFFFF8000000000ull, 0xFFFF8FF000000000ull, 0xFFF08FFF00000000ull, 0x0FFFFF80F0000000ull, 0x0FFFF8FF00000000ull}));
    //endgames[0].load_values("8-9-prob-5");
    //endgames.push_back(Endgame({0xFFFFF80000000000ull, 0xFFFF0F8000000000ull}));
    //endgames[1].load_values("8-10-prob-2");

    /*auto start = time_now();
    r_t asd = 0;
    u32 n = 10000000;
    for (u32 t = 0; t < 1; ++t) {
        asd += expectimax_limited_evals(0x12344321ull, 10000000, 0.01, tuples_4_stage_2).eval;
    }
    //print average time per eval in ns
    cout << time_since(start) / n * 1e3 << endl;
    cout << asd << endl;
    return;*/

    /*auto start = time_now();
    u32 n = 10000000;
    u64 b = 0x1234001234;
    for (u32 i = 0; i < n; ++i) {
        //move_board(b, Dir((i & 3) + 1));
        fill_board(b);
        b &= 0xFFFF'FFFF'FFFF'FFF0ull;
        //cnt += popcnt(b);
    }
    cout << r_t(n) / time_since(start) << endl;
    cout << cnt << endl;
    print_board(b);
    return;*/

    //perf_test_eval(100000);

    /*run_algorithm_episodes(10, 10, [](const u64 board, NTuple &tuples) {
        //print_board(board);
        for (auto &endgame: endgames) {
            Eval eval = endgame.eval_board(board);
            if (eval.dir == None) { continue; }
            if (eval.eval <= 0.01 || eval.eval >= 0.99) { continue; }
            return eval.dir;
        }
        //const u32 states = u32(importance(sum_cells(board)));
        //return expectimax_limited_states(downgraded(board), states, 0.01, tuples).dir;
        return expectimax_limited_evals(downgraded(board), 15000, 0.02, tuples).dir;
        //return eval_state((board), tuples).dir;
    });*/

    //endgameG8S9_eval();
    //endgame_G8S10_eval();

    /*r_t avg = 0;
    for (u32 i = 0; i < remaining_scores.size(); ++i) {
        cout << i << ": " << remaining_scores[i].first << " " << remaining_scores[i].second << " " << importance(remaining_scores[i].first) << endl;
        avg += importance(remaining_scores[i].first);
    }
    cout << avg / remaining_scores.size() << endl;
    return;*/

    //endgame_G8S10();

    /*//Endgame endgame({0xFFFFF90000000000ull, 0xFFFF0F9000000000ull}); //0.72751
    //Endgame endgame({0xFFFFF90000000000ull, 0x0FFFF900F0000000ull}); // 0.721984
    //Endgame endgame({0xFFFFF90000000000ull, 0xFFF0F90F00000000ull}); // 0.721983
    //Endgame endgame({0xFFFFF90000000000ull}); // 0.721983
    Endgame endgame({0xFFF0FF9000000000ull}); // 0.833637
    endgame.load_values("9-10-prob-1B");
    //endgame.init_goal_states(1);
    //endgame.bruteforce_values();
    //cout << endgame.get_state_value(0xFFFFF92100000120ull) << endl;
    //cout << endgame.get_afterstate_value(0xFFFFF90000000010ull) << endl;
    //cout << endgame.get_state_value(0xFFFF2F9100000010ull) << endl;
    cout << endgame.get_state_value(0xFFF0FF9000010100ull) << endl;
    //endgame.save_values("9-10-prob-1B");*/

    /*print_change_success_bases(100, 100, 32000, 8, [](const u64 board, NTuple &tuples) {
        //return eval_state(board, tuples).dir;
        return expectimax_limited_states(board, 100, 0.01, tuples).dir;
    });*/

    /*run_algorithm_episodes(1, 0, [](const u64 board, NTuple &tuples) {
        //cout << get_large_tiles_mask(board, large_th) << endl;
        return expectimax_limited_states(board, 100, 0.01, tuples).dir;
    });*/

    //print_reach_probs();
    //print_all_prob_score_stuff();


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

    //print_all_bruteforces(42);
}

void run3() {
    init();

    load_packed_weights("stage1", tuples_4_stage_1);
    load_packed_weights("stage2", tuples_4_stage_2);
    cout << endl;

    run_algorithm_episodes(200, 200, [](const u64 board, NTuple &tuples) {
        return expectimax_limited_evals(downgraded(board), 100, 0.02, tuples).dir;
    });
}

int main() {
    srand(42);
    if (REDIRECT) {
        ofstream file("../output.log", ios_base::out | ios_base::trunc);
        streambuf *consoleBuffer = cout.rdbuf();
        streambuf *fileBuffer = file.rdbuf();
        cout.rdbuf(fileBuffer);

        //run();
        //run2();
        run3();

        cout.rdbuf(consoleBuffer);
        file.close();
    } else {
        //run();
        //run2();
        run3();
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

// g++ -g -m64 -mbmi2 -pthread -O2 -o 2024_CPP ../main.cpp
// valgrind --tool=callgrind --simulate-cache=yes ./2024_CPP
// callgrind_annotate callgrind.out.126551 -> full array
// callgrind_annotate callgrind.out.

