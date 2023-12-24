#include "testing.h"
#include "learn.h"

void init() {
    auto start = time_now();

    init_zeroes();
    init_moves_0<4>();
    init_moves_0<3>();
    init_moves_123();

    for (auto &t: tuples_4) {
        //TODO replace
        //t.weights.fill(tuple_init / (8 * tuples_4.size()));
        t.weights.fill(tuple_init);
    }
    for (auto &t: tuples_3) {
        //TODO replace
        //t.weights.fill(tuple_init / (8 * tuples_3.size()));
        t.weights.fill(tuple_init);
    }

    cout << "Init time: " << time_since(start) / 1e6 << endl << endl;
}

template<u8 N>
void save_all_weights(const string &ts_str) {
    const string dir = "../weights_backups";
    if constexpr (N == 4) {
        for (auto &t: tuples_4) {
            const string filename = "weights_" + t.name + "_" + ts_str + ".bin";
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            save_array(dir + "/" + filename, (char *) &t.weights, size);
        }
    } else {
        for (auto &t: tuples_3) {
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
        for (auto &t: tuples_4) {
            const string filename = "weights_" + t.name + "_" + ts_str + ".bin";
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            load_array(dir + "/" + filename, (char *) &t.weights, size);
        }
    } else {
        for (auto &t: tuples_3) {
            const string filename = "weights_" + t.name + "_" + ts_str + ".bin";
            size_t size = t.weights.size() * sizeof(t.weights[0]);
            load_array(dir + "/" + filename, (char *) &t.weights, size);
        }
    }
    cout << endl;
}

template<u8 N>
s_t play_random_game() {
    b_t board = 0;
    s_t score = 0;
    fill_board<N>(board);
    fill_board<N>(board);
    while (!game_over<N>(board)) {
        Dir d = Dir(random(4) + 1);
        b_t afterstate = moved_board<N>(board, d);
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
    b_t board = 0;
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
            u64 b = from_array<3>(board);
            move_board<3>(b, d);
            return to_array<3>(b);
        });
        test_board_reward<3, 11>([](const array<array<u8, 3>, 3> board, Dir d) {
            u64 b = from_array<3>(board);
            return get_reward<3>(b, d);
        });
        test_board_count_empty<3, 11>([](const array<array<u8, 3>, 3> board) {
            u64 b = from_array<3>(board);
            return count_empty<3>(b);
        });
        test_board_fill<3, 11>([](const array<array<u8, 3>, 3> board) {
            u64 b = from_array<3>(board);
            fill_board<3>(b);
            return to_array<3>(b);
        });
    }
    if (true) {
        test_board_move<4, 16>([](const array<array<u8, 4>, 4> board, Dir d) {
            u64 b = from_array<4>(board);
            move_board<4>(b, d);
            return to_array<4>(b);
        });
        test_board_reward<4, 16>([](const array<array<u8, 4>, 4> board, Dir d) {
            u64 b = from_array<4>(board);
            return get_reward<4>(b, d);
        });
        test_board_count_empty<4, 16>([](const array<array<u8, 4>, 4> board) {
            u64 b = from_array<4>(board);
            return count_empty<4>(b);
        });
        test_board_fill<4, 16>([](const array<array<u8, 4>, 4> board) {
            u64 b = from_array<4>(board);
            fill_board<4>(b);
            return to_array<4>(b);
        });
    }
}

template<u8 N>
void fixed_learn(r_t LR, u32 episodes, u32 training_games, u32 testing_games) {
    learning_rate = LR;
    run_learning<N>(episodes, training_games, testing_games);
    string ts_str = get_time_str();
    cout << "Saving weights: " << ts_str << endl;
    save_all_weights<N>(ts_str);
}

bool is_connected(const u32 mask) {
    //check if all 1s on the 4x4 bit matrix determined by mask are connected
    u32 visited = 0;
    for (u32 i = 0; i < 16; ++i) {
        if ((mask >> i) & 1u) {
            visited |= 1u << i;
            break;
        }
    }
    //traverse only on 1s from the mask, then check if all 1s in mask are visited
    u32 next_visited = 0;
    do {
        visited |= next_visited;
        next_visited = visited;
        for (u32 i = 0; i < 16; ++i) {
            if (((visited >> i) & 1u) == 0) continue;
            for (u32 j = 0; j < 16; ++j) {
                if (((mask >> j) & 1u) == 0) continue;
                if (abs(int(i / 4) - int(j / 4)) + abs(int(i % 4) - int(j % 4)) == 1) {
                    next_visited |= 1u << j;
                }
            }
        }
    } while (next_visited != visited);
    return visited == mask;
}

string to_hex(const u32 mask) {
    ostringstream temp;
    temp << hex << mask << dec;
    return temp.str();
}

u32 n_tuples(u8 n) {
    u32 count = 0;
    unordered_set<b_t> set;
    for (u32 mask = 1; mask <= 0xFFFFu; ++mask) {
        if (u8(popcnt(mask)) != n) continue;
        if (!is_connected(mask)) continue;
        b_t board = pdep(mask, 0x1111111111111111ull);
        bool is_new = true;
        for (const auto &b: get_transformations<4>(board)) {
            if (set.find(b) != set.end()) {
                is_new = false;
                break;
            }
        }
        if (is_new) {
            ++count;
            set.insert(board);
        }
    }
    vector<string> masks;
    for (const auto &b: set) {
        //print_board<4>(b);
        b_t bb = b;
        bb |= bb << 1;
        bb |= bb << 2;
        u64 min_mask = 0;
        for (const auto &t: get_transformations<4>(bb)) {
            if (min_mask == 0 || t < min_mask) {
                min_mask = t;
            }
        }
        cout << hex << min_mask << dec << endl;
    }
    return count;
}


array<b_t, 68> masks_6 = {
        0xFFFF000F000Full,
        0xF00FFFFF0000ull,
        0xFFFFF00F000ull,
        0xFFFFF000F00ull,
        0xFFFFF000F0ull,
        0xF000FFFFF0ull,
        0xFF000F0FFF0ull,
        0xF00FFFFFull,
        0xF0F0F0FFFull,
        0xFFFF00F000F0ull,
        0xF00FFFFF0ull,
        0xFFFFF00F0ull,
        0xF0FFF00FFull,
        0xF0FFF0FF00ull,
        0xF00FF0FFFull,
        0xF000FFFFFull,
        0xFFFFF0F0000ull,
        0xF0FFFFFull,
        0xF0FFF0FF0ull,
        0xFFFFF00Full,
        0xFFFF00FFull,
        0xFFFFF000Full,
        0xF0FFFFF0ull,
        0xFFFFFFull,
        0xF00FFF0FF0ull,
        0xFF00FFF0F00ull,
        0xF00FFFFF00ull,
        0xFFF0FFFull,
        0xF00FFF00FFull,
        0xFFFFF0F00ull,
        0xF0FFF0F0Full,
        0xFFFF0FF0ull,
        0xFFFFFF0000ull,
        0xF0FFFF00F0ull,
        0xF000F0FFFFull,
        0xF0F0F0FFF0ull,
        0xFFF0F00FF00ull,
        0xF0FFF000FFull,
        0xFFFFFF00000ull,
        0xFF0FFFFull,
        0xFF00FF000FFull,
        0xFFFFF0F000ull,
        0xF0FFF0F0F0ull,
        0xFFFF0F0Full,
        0xF0FFFF000Full,
        0xFFF0F000FF0ull,
        0xF00FFF0F0Full,
        0xF0FFFF0F00ull,
        0xF00FF00FFFull,
        0xF0FFF00FF0ull,
        0xFFFFF00F00ull,
        0xFF00F0FFF0ull,
        0xFF0FF00FF0ull,
        0xF0F0FFF00F0ull,
        0xFFFFFF000ull,
        0xF0F0FFF000Full,
        0xF0FFFFF0000ull,
        0xFFF0FF00F00ull,
        0xF00FF0FFF0ull,
        0xFF0FF0FF00ull,
        0xFF00FF00FF0ull,
        0xF0FFFFF00ull,
        0xFFFFFF0ull,
        0xF0F0FFF0F00ull,
        0xFF00FFF00F0ull,
        0xFFFFF0Full,
        0xF0FFFFF000ull,
        0xFF0FFFF0000ull
};

void select_tuples() {
    vector<u32> indices;

    vector<b_t> init_masks = {0xfff0fffull,
                              0xff00ff00ff0ull,
                              0xffffffull,
                              0xffffff0000ull,
                              0xffffff0ull,
                              0xf000fffffull,
                              0xf0ffff000full,
                              0xf0fffff00ull,
                              0xf00fffffull,
                              0xffff0f0full};
    int init_num_tuples = 10;
    for (int i = 0; i < init_num_tuples; ++i) {
        tuples_4[i].mask = init_masks[i];
        //push index of mask in masks_6 to indices
        indices.push_back(find(masks_6.begin(), masks_6.end(), init_masks[i]) - masks_6.begin());
    }
    for (u32 i = 0; i < init_num_tuples; ++i) {
        tuples_4[i].weights.fill(tuple_init);
    }
    num_tuples = init_num_tuples;

    while (true) {
        r_t best_score = 0;
        u32 best_index = 0;
        for (u32 new_index = 0; new_index < masks_6.size(); ++new_index) {
            cout << "Trying index: " << new_index << endl;
            if (find(indices.begin(), indices.end(), new_index) != indices.end()) {
                continue;
            }
            b_t new_mask = masks_6[new_index];

            tuples_4[num_tuples].mask = new_mask;
            for (u32 i = 0; i <= num_tuples; ++i) {
                tuples_4[i].weights.fill(tuple_init);
            }

            learning_rate = 0.1;
            run_training_episodes<4>(200000);

            r_t score = run_testing_episodes<4>(20000);

            if (score > best_score) {
                best_score = score;
                best_index = new_index;
            }

            cout << "Score: " << score << endl;
        }

        cout << endl;
        cout << "Num tuples: " << to_string(num_tuples) << endl;
        cout << "Best score: " << best_score << endl;
        cout << "Best index: " << best_index << endl;
        cout << "Best mask: " << hex << masks_6[best_index] << dec << endl;
        cout << endl;

        indices.push_back(best_index);
        b_t new_mask = masks_6[best_index];
        tuples_4[num_tuples].mask = new_mask;
        for (u32 i = 0; i <= num_tuples; ++i) {
            tuples_4[i].weights.fill(tuple_init);
        }

        ++num_tuples;
        if (num_tuples == 12) {
            break;
        }
    }

    for (u32 i = 0; i < num_tuples; ++i) {
        cout << "Tuple " << i << ": " << hex << tuples_4[i].mask << dec << endl;
    }
}

template<u8 N>
void run() {
    //load_all_weights<N>("1216-21-10-12");
    //run_testing_episodes<N>(1000);

    //interactive_learn(1000, 100);
    //for (u32 i = 0; i < 2; ++i) { fixed_learn<N>(0.1, 10, 10000, 1000); }

    //IMPLEMENT run_learning<4>(10, 10000, 1000, 0.75);
    //fixed_learn<N>(0.75, 10, 10000, 1000);

    select_tuples();
}

int main() {
    srand(42);

    init();

    //run_tests();
    //cout << endl;
    //perf_test(10000);
    //perf_test_general(10000);

    bool redirect = true;
    if (redirect) {
        ofstream file("../output.log", std::ios_base::out | std::ios_base::trunc);
        streambuf *consoleBuffer = cout.rdbuf();
        streambuf *fileBuffer = file.rdbuf();
        cout.rdbuf(fileBuffer);

        run<4>();

        cout.rdbuf(consoleBuffer);
        file.close();
    } else {
        run<4>();
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

