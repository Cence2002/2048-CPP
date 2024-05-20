#include "endgame_bruteforce.h"
#include "board_all.h"
#include "algorithm.h"
#include "learn.h"
#include <variant>

constexpr int SEED_OVERRIDE = 15;
constexpr bool REDIRECT_OUTPUT = false;

constexpr NTuple &ntuple = ntuple_bence_stage_1;

constexpr bool TWO_STAGE = true;
constexpr NTuple &ntuple_stage_2 = ntuple_bence_stage_2;

struct testing_params {
    const enum eval_mode : int {
        Direct_eval = 0,
        Limited_depth_prob = 1,
        Limited_evals = 2,
    } mode;

    const int games;
    const int threads;
    const bool downgrade;

    const int depth_limit;
    const float probability_limit;

    const int evals_limit;
    const float ratio_limit;
};

//constexpr testing_params TEST_SETTINGS = {testing_params::Direct_eval, 1000, 20, true};
//constexpr testing_params TEST_SETTINGS = {testing_params::Limited_depth_prob, 100, 20, false, 2, 0.05};
constexpr testing_params TEST_SETTINGS = {testing_params::Limited_evals, 10, 0, true, 0, 0, 1000, 0.02};

struct training_params {
    int epochs;
    int training_games;
    int testing_games;
    float learning_rate;
    int threads;
};

constexpr auto TRAIN_SETTINGS = {
        training_params{80, 1000000, 100000, 0.1, 20},
        training_params{40, 1000000, 100000, 0.01, 20},
        training_params{10, 1000000, 100000, 0.001, 20},
};

void init() {
    auto start = time_now();

    init_rng(SEED_OVERRIDE);

    std::cout << "Threads: " << std::thread::hardware_concurrency() << std::endl;
}

template<board_concept board_type>
std::pair<s_t, u32> play_random_game() {
    board_type board{};
    s_t score = 0;
    u32 moves = 0;
    board.spawn();
    board.spawn();
    while ((board.slid(Left) != board) ||
           (board.slid(Up) != board) ||
           (board.slid(Right) != board) ||
           (board.slid(Down) != board)) {
        const Dir d = Dir(random(4) + 1);
        if (board.slid(d) == board) { continue; }
        score += board.get_reward(d);
        ++moves;
        board.slide(d);
        board.spawn();
    }
    return {score, moves};
}

template<board_concept board_type>
void performance_test_random_games(const u32 games) {
    board_type::init_tables();
    auto start = time_now();
    u64 total_score = 0;
    u64 total_moves = 0;
    for (u32 i = 0; i < games; ++i) {
        const auto [score, moves] = play_random_game<board_type>();
        total_score += score;
        total_moves += moves;
    }
    std::cout << "Average score: " << r_t(total_score) / r_t(games) << std::endl;
    std::cout << "Average moves: " << r_t(total_moves) / r_t(games) << std::endl;
    std::cout << "Moves per second: " << r_t(total_moves) / (time_since(start) / 1e6) << std::endl << std::endl;
}

template<board_concept board_type>
void performance_test_random_moves(const u32 moves) {
    board_type::init_tables();
    auto start = time_now();
    board_type board{};
    u64 total_score = 0;
    for (u32 i = 0; i < moves; ++i) {
        const Dir d = Dir((i & 0b11u) + 1);
        total_score += board.get_reward(d);
        board.slide(d);
        if (board.empty_count() == 0) {
            board = {};
        } else if ((i & 0xFFu) == 0) {
            board.spawn();
        }
    }
    std::cout << "Total score: " << total_score << std::endl;
    std::cout << "Moves per second: " << moves / (time_since(start) / 1e6) << std::endl << std::endl;
}

std::pair<s_t, u32> play_eval_game() {
    b_t board{};
    s_t score = 0;
    u32 moves = 0;
    board.spawn();
    board.spawn();
    while (true) {
        const Dir d = eval_state(board, ntuple).dir;
        if (d == None) { break; }
        score += board.get_reward(d);
        ++moves;
        board.slide(d);
        board.spawn();
    }
    return {score, moves};
}

void performance_test_eval_games(const u32 games) {
    b_t::init_tables();
    auto start = time_now();
    u64 total_score = 0;
    u64 total_moves = 0;
    for (u32 i = 0; i < games; ++i) {
        const auto [score, moves] = play_eval_game();
        total_score += score;
        total_moves += moves;
    }
    std::cout << "Average score: " << r_t(total_score) / r_t(games) << std::endl;
    std::cout << "Average moves: " << r_t(total_moves) / r_t(games) << std::endl;
    std::cout << "Moves per second: " << r_t(total_moves) / (time_since(start) / 1e6) << std::endl << std::endl;
}

r_t play_original_game() {
    const auto start = time_now();
    b_t board{};
    board.spawn();
    board.spawn();
    while (true) {
        const Eval eval = eval_state(board, ntuple);
        if (eval.dir == None) { return 0; }
        board = eval.afterstate;
        if (board.get_max_cell() == 11) {
            return time_since(start);
        }
        board.spawn();
    }
}

void performance_test_original_games(const u32 games = 1000) {
    load_packed_weights("stage1", ntuple_bence_stage_1);
    load_packed_weights("stage2", ntuple_bence_stage_2);

    b_t::init_tables();
    r_t total_time = 0;
    u32 total_wins = 0;
    for (u32 i = 0; i < games; ++i) {
        const r_t time = play_original_game();
        if (time > 0) {
            total_time += time;
            ++total_wins;
        }
    }
    std::cout << "Win rate: " << r_t(total_wins) / r_t(games) << std::endl;
    std::cout << "Average time: " << total_time / r_t(total_wins) << std::endl;
}

r_t get_goal_value(const r_t cumulative_score_gain, const r_t reaching_prob, const r_t scale) {
    return cumulative_score_gain / reaching_prob * scale;
}

void run_performance_testing(const u32 games = 10000, const u32 moves = 1000000) {
    std::cout << "Running " << games << " random games:" << std::endl;

    std::cout << "Using type:\tb_t_sim" << std::endl;
    performance_test_random_games<b_t_sim>(games);

    std::cout << "Using type:\tb_t_mat" << std::endl;
    performance_test_random_games<b_t_mat>(games);

    std::cout << "Using type:\tb_t_arr_32" << std::endl;
    performance_test_random_games<b_t_arr<l_t_32>>(games);

    std::cout << "Using type:\tb_t_arr_16" << std::endl;
    performance_test_random_games<b_t_arr<l_t_16>>(games);

    std::cout << "Using type:\tb_t_64" << std::endl;
    performance_test_random_games<b_t_64>(games);

    std::cout << "Using type:\tb_t_opt" << std::endl;
    performance_test_random_games<b_t_opt>(games);


    std::cout << "Running " << moves << " random moves:" << std::endl;

    std::cout << "Using type:\tb_t_sim" << std::endl;
    performance_test_random_moves<b_t_sim>(moves);

    std::cout << "Using type:\tb_t_mat" << std::endl;
    performance_test_random_moves<b_t_mat>(moves);

    std::cout << "Using type:\tb_t_arr_32" << std::endl;
    performance_test_random_moves<b_t_arr<l_t_32>>(moves);

    std::cout << "Using type:\tb_t_arr_16" << std::endl;
    performance_test_random_moves<b_t_arr<l_t_16>>(moves);

    std::cout << "Using type:\tb_t_64" << std::endl;
    performance_test_random_moves<b_t_64>(moves);

    std::cout << "Using type:\tb_t_opt" << std::endl;
    performance_test_random_moves<b_t_opt>(moves);
}

void run_performance_testing_eval_games(const u32 games = 10) {
    load_packed_weights("stage1", ntuple_bence_stage_1);

    std::cout << "Running " << games << " actual games with direct evaluation:" << std::endl;
    performance_test_eval_games(games);
}

void endgame_G8S10_eval() {
    Endgame endgame({0xFFFFF80000000000ull, 0xFFFF0F8000000000ull});
    const r_t goal_value = get_goal_value(410887, 0.5682, 1.2);
    std::cout << goal_value << std::endl;
    endgame.init_goal_states(goal_value);
    endgame.bruteforce_values();
    std::cout << endgame.get_state_value(0xFFFFF82100000120ull) << std::endl;
    std::cout << endgame.get_state_value(0xFFFF2F8100000120ull) << std::endl;
    endgame.save_values("8-10-eval-2");
}

std::vector<Endgame> endgames;

void run_algorithm_testing() {
    init();

    load_packed_weights("stage1", ntuple_bence_stage_1);
    load_packed_weights("stage2", ntuple_bence_stage_2);

    //endgame_G8S10_eval();
    //endgames.push_back(Endgame({0xFFFFFF8000000000ull, 0xFFFF8FF000000000ull, 0xFFF08FFF00000000ull, 0x0FFFFF80F0000000ull, 0x0FFFF8FF00000000ull}));
    //endgames[0].load_values("8-9-eval-500");

    run_algorithm_episodes(TEST_SETTINGS.games, TEST_SETTINGS.threads, [](b_t board) {
        if constexpr (TEST_SETTINGS.downgrade) {
            board = downgraded(board);
        }

        for (auto &endgame: endgames) {
            Eval eval = endgame.eval_board(board);
            if (eval.dir == None) { continue; }
            if (eval.eval <= 0.01 || eval.eval >= 0.99) { continue; }
            return eval.dir;
        }

        const bool next_stage = board.get_max_cell() >= 14;
        const NTuple &current_ntuple = (TWO_STAGE && next_stage) ? ntuple_stage_2 : ntuple;
        switch (TEST_SETTINGS.mode) {
            case testing_params::Direct_eval:
                return eval_state(board, current_ntuple).dir;
            case testing_params::Limited_depth_prob:
                return expectimax_limited_depth_prob(board, TEST_SETTINGS.depth_limit, TEST_SETTINGS.probability_limit, current_ntuple).dir;
            case testing_params::Limited_evals:
                return expectimax_limited_evals(board, TEST_SETTINGS.evals_limit, TEST_SETTINGS.ratio_limit, current_ntuple).dir;
        }
        return None;
    });
}

void run_training() {
    init();

    load_packed_weights("stage1", ntuple_bence_stage_1);
    for (const auto &params: TRAIN_SETTINGS) {
        fixed_learn(params.learning_rate, params.epochs, params.training_games, params.testing_games, params.threads, ntuple);
    }
}

int main() {
    srand(42);

    init();
    b_t::init_tables();

    if constexpr (REDIRECT_OUTPUT) {
        std::ofstream file("../output.log", std::ios_base::out | std::ios_base::trunc);
        std::streambuf *consoleBuffer = std::cout.rdbuf();
        std::streambuf *fileBuffer = file.rdbuf();
        std::cout.rdbuf(fileBuffer);

        run_algorithm_testing();

        std::cout.rdbuf(consoleBuffer);
        file.close();
    } else {
        run_algorithm_testing();
        //run_performance_testing(10000, 10000000);
        //run_performance_testing_eval_games(1000);
        //performance_test_original_games(10000);

        //run_training();
    }

    return 0;
}

// To brute force for 3x3:
// goal of player: maximizing or minimizing (or random)
// goal of game: maximizing or minimizing or random (with arbitrary probability for 2 instead of 0.9)
// metrics: score, number of moves, probability of reaching a certain tile (1-10), some entropy
// extra: ratio of reachable positions compared to all (3% for 3x3, supposably even less for 4x4)

// Average sum per moves: n/(0.9*2+0.1*4) =>
// => n/2.2
// Average sum
// Average score for 2^2: (0.45*4+0.1*0)/(0.45+0.1) =>
// => 36/11 (= 3.27)
// Average score for 2^n: f(2)=36/11,f(n)=2*f(n-1)+2^n =>
// => (n-13/11)*2^n

// Score for 2 = 4          => 3.3
// Score for 3 = 8          => 14.5
// Score for 4 = 16         => 45.1
// Score for 5 = 32         => 122.2
// Score for 6 = 64         => 308.4
// Score for 7 = 128        => 744.7
// Score for 8 = 256        => 1745.5
// Score for 9 = 512        => 4002.9
// Score for A = 1024       => 9029.8
// Score for B = 2048       => 20107.6
// Score for C = 4096       => 44311.3
// Score for D = 8192       => 96814.5
// Score for E = 16384      => 210013.1
// Score for F = 32768      => 452794.2
// Score for G = 65536      => 971124.4
// Score for H = 131072     => 2073320.7

// Moves for N = 2^N        => 2^N/2.2

// g++ -g -m64 -mbmi2 -pthread -O2 -o 2024_CPP ../main.cpp
// valgrind --tool=callgrind --simulate-cache=yes ./2024_CPP
// callgrind_annotate callgrind.out.126551 -> full array
// callgrind_annotate callgrind.out.
