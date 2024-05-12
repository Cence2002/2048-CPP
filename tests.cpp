#include <gtest/gtest.h>
#include "board_all.h"

class TestingEnvironment : public testing::Environment {
public:
    void SetUp() override {
        l_t_32::init_tables();
        l_t_16::init_tables();
        b_t_mat::init_tables();
        b_t_arr<l_t_32>::init_tables();
        b_t_arr<l_t_16>::init_tables();
        b_t_64::init_tables();
        b_t_opt::init_tables();
    }
};

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new TestingEnvironment);
    return RUN_ALL_TESTS();
}

template<u8 MAX>
std::vector<std::array<u8, 4>> generate_all_lines() {
    std::vector<std::array<u8, 4>> lines;
    for (u32 bits = 0; bits < E(20); ++bits) {
        std::array<u8, 4> line{};
        for (u8 i = 0; i < 4; i++) {
            const u8 cell = (bits >> (i * 4)) & 0xFu;
            if (cell > MAX) {
                continue;
            }
            line[i] = cell;
        }
        lines.push_back(line);
    }
    return lines;
}

template<u8 MIN, u8 MAX>
std::vector<std::array<std::array<u8, 4>, 4>> generate_boards(u32 count) {
    std::vector<std::array<std::array<u8, 4>, 4>> boards;
    for (u32 t = 0; t < count; ++t) {
        std::array<std::array<u8, 4>, 4> board{};
        for (u8 y = 0; y < 4; y++) {
            for (u8 x = 0; x < 4; x++) {
                if (random(4) == 0) {
                    board[y][x] = 0;
                    continue;
                } else {
                    board[y][x] = MIN + random(MAX - MIN + 1);
                }
            }
        }
        boards.push_back(board);
    }
    return boards;
}

template<u8 MAX>
std::vector<std::array<std::array<u8, 4>, 4>> generate_boards(u32 count) {
    return generate_boards<0, MAX>(count);
}

template<line_concept line_type>
void COMPARE_LINES(const std::array<u8, 4> &items, const std::array<u8, 4> &left_items, const std::array<u8, 4> &right_items, u32 reward) {
    line_type line, left, right;
    line.load_array(items);
    left.load_array(left_items);
    right.load_array(right_items);

    EXPECT_EQ(line.get_reward(), reward);

    line.left();
    EXPECT_EQ(line.get_array(), left.get_array());

    line.right();
    EXPECT_EQ(line.get_array(), right.get_array());
}

template<line_concept line_type_1 = l_t_16, line_concept line_type_2 = l_t_32>
void MATCH_LINES(const std::array<u8, 4> &items) {
    line_type_1 line_1;
    line_type_2 line_2;

    line_1.load_array(items);
    line_2.load_array(items);
    EXPECT_EQ(line_1.get_reward(), line_2.get_reward());

    line_1.load_array(items);
    line_2.load_array(items);
    line_1.left();
    line_2.left();
    EXPECT_EQ(line_1.get_array(), line_2.get_array());

    line_1.load_array(items);
    line_2.load_array(items);
    line_1.right();
    line_2.right();
    EXPECT_EQ(line_1.get_array(), line_2.get_array());
}

template<typename T = std::array<std::array<u8, 4>, 4>>
void EXPECT_EQS(const std::vector<T> &elements) {
    for (size_t i = 1; i < elements.size(); i++) {
        EXPECT_EQ(elements[i], elements[0]);
    }
}

TEST(LINES, BASE_CASES) {
    l_t_16 line, moved;

    line.load_array({0, 0, 0, 0});
    moved.load_array({0, 0, 0, 0});
    EXPECT_EQ(line.get_reward(), 0);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());

    line.load_array({2, 4, 6, 0});
    moved.load_array({2, 4, 6, 0});
    EXPECT_EQ(line.get_reward(), 0);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());

    line.load_array({0, 2, 0, 4});
    moved.load_array({2, 4, 0, 0});
    EXPECT_EQ(line.get_reward(), 0);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());

    line.load_array({2, 2, 0, 0});
    moved.load_array({3, 0, 0, 0});
    EXPECT_EQ(line.get_reward(), 8);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());

    line.load_array({0, 2, 0, 2});
    moved.load_array({3, 0, 0, 0});
    EXPECT_EQ(line.get_reward(), 8);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());

    line.load_array({2, 2, 4, 4});
    moved.load_array({3, 5, 0, 0});
    EXPECT_EQ(line.get_reward(), 40);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());

    line.load_array({2, 2, 2, 2});
    moved.load_array({3, 3, 0, 0});
    EXPECT_EQ(line.get_reward(), 16);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());
    moved.load_array({4, 0, 0, 0});
    EXPECT_EQ(line.get_reward(), 16);
    line.left();
    EXPECT_EQ(line.get_array(), moved.get_array());
}

TEST(LINES, SPECIAL_CASES) {
    l_t_16 line_16, moved_16;
    l_t_32 line_32, moved_32;

    const u8 F = 0xFu;
    const u8 G = 0x10u;
    const u8 H = 0x11u;

    line_16.load_array({F, F, 0, 0});
    line_32.load_array({F, F, 0, 0});
    moved_16.load_array({F, F, 0, 0});
    moved_32.load_array({G, 0, 0, 0});
    EXPECT_EQ(line_16.get_reward(), 0);
    EXPECT_EQ(line_32.get_reward(), E(G));
    line_16.left();
    line_32.left();
    EXPECT_EQ(line_16.get_array(), moved_16.get_array());
    EXPECT_EQ(line_32.get_array(), moved_32.get_array());

    line_16.load_array({0, F, 0, F});
    line_32.load_array({0, F, 0, F});
    moved_16.load_array({F, F, 0, 0});
    moved_32.load_array({G, 0, 0, 0});
    EXPECT_EQ(line_16.get_reward(), 0);
    EXPECT_EQ(line_32.get_reward(), E(G));
    line_16.left();
    line_32.left();
    EXPECT_EQ(line_16.get_array(), moved_16.get_array());
    EXPECT_EQ(line_32.get_array(), moved_32.get_array());

    line_16.load_array({F, F, F, F});
    line_32.load_array({F, F, F, F});
    moved_16.load_array({F, F, F, F});
    moved_32.load_array({G, G, 0, 0});
    EXPECT_EQ(line_16.get_reward(), 0);
    EXPECT_EQ(line_32.get_reward(), E(G) + E(G));
    line_16.left();
    line_32.left();
    EXPECT_EQ(line_16.get_array(), moved_16.get_array());
    EXPECT_EQ(line_32.get_array(), moved_32.get_array());

    line_32.load_array({0, G, 0, G});
    moved_32.load_array({H, 0, 0, 0});
    EXPECT_EQ(line_32.get_reward(), E(H));
    line_32.left();
    EXPECT_EQ(line_32.get_array(), moved_32.get_array());
}

TEST(LINES, MATCH_BASE_CASES) {
    for (const auto &line: generate_all_lines<14>()) {
        l_t_16 line_16;
        l_t_32 line_32;

        line_16.load_array(line);
        line_32.load_array(line);
        EXPECT_EQ(line_16.get_reward(), line_32.get_reward());

        line_16.load_array(line);
        line_32.load_array(line);
        line_16.left();
        line_32.left();
        EXPECT_EQ(line_16.get_array(), line_32.get_array());

        line_16.load_array(line);
        line_32.load_array(line);
        line_16.right();
        line_32.right();
        EXPECT_EQ(line_16.get_array(), line_32.get_array());

        MATCH_LINES(line);
    }
}

TEST(BOARDS, BASE_CASES) {
    b_t_arr<l_t_16> board, left, right, up, down;
    b_t_arr<l_t_16> left_goal, right_goal, up_goal, down_goal;
    board.load_matrix({{
                               {0, 0, 0, 0},
                               {0, 2, 2, 0},
                               {0, 2, 2, 0},
                               {0, 0, 0, 0}
                       }});
    left_goal.load_matrix({{
                                   {0, 0, 0, 0},
                                   {3, 0, 0, 0},
                                   {3, 0, 0, 0},
                                   {0, 0, 0, 0}
                           }});
    right_goal.load_matrix({{
                                    {0, 0, 0, 0},
                                    {0, 0, 0, 3},
                                    {0, 0, 0, 3},
                                    {0, 0, 0, 0}
                            }});
    up_goal.load_matrix({{
                                 {0, 3, 3, 0},
                                 {0, 0, 0, 0},
                                 {0, 0, 0, 0},
                                 {0, 0, 0, 0}
                         }});
    down_goal.load_matrix({{
                                   {0, 0, 0, 0},
                                   {0, 0, 0, 0},
                                   {0, 0, 0, 0},
                                   {0, 3, 3, 0}
                           }});
    EXPECT_EQ(board.get_reward(Left), 16);
    EXPECT_EQ(board.get_reward(Right), 16);
    EXPECT_EQ(board.get_reward(Up), 16);
    EXPECT_EQ(board.get_reward(Down), 16);

    (left = board).slide(Left);
    (right = board).slide(Right);
    (up = board).slide(Up);
    (down = board).slide(Down);
    EXPECT_EQ(left.get_matrix(), left_goal.get_matrix());
    EXPECT_EQ(right.get_matrix(), right_goal.get_matrix());
    EXPECT_EQ(up.get_matrix(), up_goal.get_matrix());
    EXPECT_EQ(down.get_matrix(), down_goal.get_matrix());

    board.load_matrix({{
                               {2, 0, 2, 4},
                               {2, 2, 2, 2},
                               {1, 2, 0, 1},
                               {2, 1, 0, 2}
                       }});
    left_goal.load_matrix({{
                                   {3, 4, 0, 0},
                                   {3, 3, 0, 0},
                                   {1, 2, 1, 0},
                                   {2, 1, 2, 0}
                           }});
    right_goal.load_matrix({{
                                    {0, 0, 3, 4},
                                    {0, 0, 3, 3},
                                    {0, 1, 2, 1},
                                    {0, 2, 1, 2}
                            }});
    up_goal.load_matrix({{
                                 {3, 3, 3, 4},
                                 {1, 1, 0, 2},
                                 {2, 0, 0, 1},
                                 {0, 0, 0, 2}
                         }});
    down_goal.load_matrix({{
                                   {0, 0, 0, 4},
                                   {3, 0, 0, 2},
                                   {1, 3, 0, 1},
                                   {2, 1, 3, 2}
                           }});
    EXPECT_EQ(board.get_reward(Left), 24);
    EXPECT_EQ(board.get_reward(Right), 24);
    EXPECT_EQ(board.get_reward(Up), 24);
    EXPECT_EQ(board.get_reward(Down), 24);

    (left = board).slide(Left);
    (right = board).slide(Right);
    (up = board).slide(Up);
    (down = board).slide(Down);
    EXPECT_EQ(left.get_matrix(), left_goal.get_matrix());
    EXPECT_EQ(right.get_matrix(), right_goal.get_matrix());
    EXPECT_EQ(up.get_matrix(), up_goal.get_matrix());
    EXPECT_EQ(down.get_matrix(), down_goal.get_matrix());
}

TEST(BOARDS, SPECIAL_CASES) {
    b_t_arr<l_t_16> board_16, left_16, right_16, up_16, down_16;
    b_t_arr<l_t_16> left_goal_16, right_goal_16, up_goal_16, down_goal_16;
    b_t_arr<l_t_32> board_32, left_32, right_32, up_32, down_32;
    b_t_arr<l_t_32> left_goal_32, right_goal_32, up_goal_32, down_goal_32;

    const u8 F = 0xFu;
    const u8 G = 0x10u;
    const u8 H = 0x11u;

    board_16.load_matrix({{
                                  {F, F, 0, 0},
                                  {0, F, 0, F},
                                  {0, F, F, F},
                                  {0, F, 0, 0}
                          }});
    board_32.load_matrix({{
                                  {F, F, 0, 0},
                                  {0, F, 0, F},
                                  {0, F, F, F},
                                  {0, F, 0, 0}
                          }});

    left_goal_16.load_matrix({{
                                      {F, F, 0, 0},
                                      {F, F, 0, 0},
                                      {F, F, F, 0},
                                      {F, 0, 0, 0}
                              }});
    left_goal_32.load_matrix({{
                                      {G, 0, 0, 0},
                                      {G, 0, 0, 0},
                                      {G, F, 0, 0},
                                      {F, 0, 0, 0}
                              }});

    right_goal_16.load_matrix({{
                                       {0, 0, F, F},
                                       {0, 0, F, F},
                                       {0, F, F, F},
                                       {0, 0, 0, F}
                               }});
    right_goal_32.load_matrix({{
                                       {0, 0, 0, G},
                                       {0, 0, 0, G},
                                       {0, 0, F, G},
                                       {0, 0, 0, F}
                               }});

    up_goal_16.load_matrix({{
                                    {F, F, F, F},
                                    {0, F, 0, F},
                                    {0, F, 0, 0},
                                    {0, F, 0, 0}
                            }});
    up_goal_32.load_matrix({{
                                    {F, G, F, G},
                                    {0, G, 0, 0},
                                    {0, 0, 0, 0},
                                    {0, 0, 0, 0}
                            }});

    down_goal_16.load_matrix({{
                                      {0, F, 0, 0},
                                      {0, F, 0, 0},
                                      {0, F, 0, F},
                                      {F, F, F, F}
                              }});
    down_goal_32.load_matrix({{
                                      {0, 0, 0, 0},
                                      {0, 0, 0, 0},
                                      {0, G, 0, 0},
                                      {F, G, F, G}
                              }});

    EXPECT_EQ(board_16.get_reward(Left), 0);
    EXPECT_EQ(board_16.get_reward(Right), 0);
    EXPECT_EQ(board_16.get_reward(Up), 0);
    EXPECT_EQ(board_16.get_reward(Down), 0);

    (left_16 = board_16).slide(Left);
    (right_16 = board_16).slide(Right);
    (up_16 = board_16).slide(Up);
    (down_16 = board_16).slide(Down);
    EXPECT_EQ(left_16.get_matrix(), left_goal_16.get_matrix());
    EXPECT_EQ(right_16.get_matrix(), right_goal_16.get_matrix());
    EXPECT_EQ(up_16.get_matrix(), up_goal_16.get_matrix());
    EXPECT_EQ(down_16.get_matrix(), down_goal_16.get_matrix());

    EXPECT_EQ(board_32.get_reward(Left), E(G) * 3);
    EXPECT_EQ(board_32.get_reward(Right), E(G) * 3);
    EXPECT_EQ(board_32.get_reward(Up), E(G) * 3);
    EXPECT_EQ(board_32.get_reward(Down), E(G) * 3);

    (left_32 = board_32).slide(Left);
    (right_32 = board_32).slide(Right);
    (up_32 = board_32).slide(Up);
    (down_32 = board_32).slide(Down);
    EXPECT_EQ(left_32.get_matrix(), left_goal_32.get_matrix());
    EXPECT_EQ(right_32.get_matrix(), right_goal_32.get_matrix());
    EXPECT_EQ(up_32.get_matrix(), up_goal_32.get_matrix());
    EXPECT_EQ(down_32.get_matrix(), down_goal_32.get_matrix());


    board_32.load_matrix({{
                                  {F, F, G, 0},
                                  {0, F, 0, F},
                                  {0, F, G, 0},
                                  {0, F, 0, 0}
                          }});
    left_goal_32.load_matrix({{
                                      {G, G, 0, 0},
                                      {G, 0, 0, 0},
                                      {F, G, 0, 0},
                                      {F, 0, 0, 0}
                              }});
    right_goal_32.load_matrix({{
                                       {0, 0, G, G},
                                       {0, 0, 0, G},
                                       {0, 0, F, G},
                                       {0, 0, 0, F}
                               }});
    up_goal_32.load_matrix({{
                                    {F, G, H, F},
                                    {0, G, 0, 0},
                                    {0, 0, 0, 0},
                                    {0, 0, 0, 0}
                            }});
    down_goal_32.load_matrix({{
                                      {0, 0, 0, 0},
                                      {0, 0, 0, 0},
                                      {0, G, 0, 0},
                                      {F, G, H, F}
                              }});

    EXPECT_EQ(board_32.get_reward(Left), E(G) * 2);
    EXPECT_EQ(board_32.get_reward(Right), E(G) * 2);
    EXPECT_EQ(board_32.get_reward(Up), E(G) * 2 + E(H));
    EXPECT_EQ(board_32.get_reward(Down), E(G) * 2 + E(H));

    (left_32 = board_32).slide(Left);
    (right_32 = board_32).slide(Right);
    (up_32 = board_32).slide(Up);
    (down_32 = board_32).slide(Down);
    EXPECT_EQ(left_32.get_matrix(), left_goal_32.get_matrix());
    EXPECT_EQ(right_32.get_matrix(), right_goal_32.get_matrix());
    EXPECT_EQ(up_32.get_matrix(), up_goal_32.get_matrix());
    EXPECT_EQ(down_32.get_matrix(), down_goal_32.get_matrix());
}

TEST(BOARDS, MATCH_BASE_CASES) {
    for (const auto &board: generate_boards<14>(1000000)) {
        b_t_sim board_sim; // used as reference
        b_t_mat board_mat;
        b_t_arr<l_t_16> board_arr_16;
        b_t_arr<l_t_32> board_arr_32;
        b_t_64 board_64;
        b_t_opt board_opt;

        board_sim.load_matrix(board);
        board_mat.load_matrix(board);
        board_arr_16.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        EXPECT_EQ(board_sim.get_reward(Left), board_mat.get_reward(Left));
        EXPECT_EQ(board_sim.get_reward(Left), board_arr_16.get_reward(Left));
        EXPECT_EQ(board_sim.get_reward(Left), board_arr_32.get_reward(Left));
        EXPECT_EQ(board_sim.get_reward(Left), board_64.get_reward(Left));
        EXPECT_EQ(board_sim.get_reward(Left), board_opt.get_reward(Left));
        EXPECT_EQ(board_sim.get_reward(Right), board_arr_16.get_reward(Right));
        EXPECT_EQ(board_sim.get_reward(Right), board_arr_32.get_reward(Right));
        EXPECT_EQ(board_sim.get_reward(Right), board_64.get_reward(Right));
        EXPECT_EQ(board_sim.get_reward(Right), board_opt.get_reward(Right));
        EXPECT_EQ(board_sim.get_reward(Up), board_arr_16.get_reward(Up));
        EXPECT_EQ(board_sim.get_reward(Up), board_arr_32.get_reward(Up));
        EXPECT_EQ(board_sim.get_reward(Up), board_64.get_reward(Up));
        EXPECT_EQ(board_sim.get_reward(Up), board_opt.get_reward(Up));
        EXPECT_EQ(board_sim.get_reward(Down), board_arr_16.get_reward(Down));
        EXPECT_EQ(board_sim.get_reward(Down), board_arr_32.get_reward(Down));
        EXPECT_EQ(board_sim.get_reward(Down), board_64.get_reward(Down));
        EXPECT_EQ(board_sim.get_reward(Down), board_opt.get_reward(Down));

        board_sim.load_matrix(board);
        board_mat.load_matrix(board);
        board_arr_16.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_sim.slide(Left);
        board_mat.slide(Left);
        board_arr_16.slide(Left);
        board_arr_32.slide(Left);
        board_64.slide(Left);
        board_opt.slide(Left);
        EXPECT_EQ(board_sim.get_matrix(), board_mat.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_16.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_32.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_opt.get_matrix());

        board_sim.load_matrix(board);
        board_mat.load_matrix(board);
        board_arr_16.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_sim.slide(Right);
        board_mat.slide(Right);
        board_arr_16.slide(Right);
        board_arr_32.slide(Right);
        board_64.slide(Right);
        board_opt.slide(Right);
        EXPECT_EQ(board_sim.get_matrix(), board_mat.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_16.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_32.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_opt.get_matrix());

        board_sim.load_matrix(board);
        board_mat.load_matrix(board);
        board_arr_16.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_sim.slide(Up);
        board_mat.slide(Up);
        board_arr_16.slide(Up);
        board_arr_32.slide(Up);
        board_64.slide(Up);
        board_opt.slide(Up);
        EXPECT_EQ(board_sim.get_matrix(), board_mat.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_16.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_32.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_opt.get_matrix());

        board_sim.load_matrix(board);
        board_mat.load_matrix(board);
        board_arr_16.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_sim.slide(Down);
        board_mat.slide(Down);
        board_arr_16.slide(Down);
        board_arr_32.slide(Down);
        board_64.slide(Down);
        board_opt.slide(Down);
        EXPECT_EQ(board_sim.get_matrix(), board_mat.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_16.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_arr_32.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_sim.get_matrix(), board_opt.get_matrix());
    }
}

TEST(BOARDS, MATCH_SPECIAL_CASES) {
    for (const auto &board: generate_boards<10, 15>(1000000)) {
        b_t_arr<l_t_16> board_arr_16; // used as reference
        b_t_64 board_64;
        b_t_opt board_opt;

        board_arr_16.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        EXPECT_EQ(board_arr_16.get_reward(Left), board_64.get_reward(Left));
        EXPECT_EQ(board_arr_16.get_reward(Left), board_opt.get_reward(Left));
        EXPECT_EQ(board_arr_16.get_reward(Right), board_64.get_reward(Right));
        EXPECT_EQ(board_arr_16.get_reward(Right), board_opt.get_reward(Right));
        EXPECT_EQ(board_arr_16.get_reward(Up), board_64.get_reward(Up));
        EXPECT_EQ(board_arr_16.get_reward(Up), board_opt.get_reward(Up));
        EXPECT_EQ(board_arr_16.get_reward(Down), board_64.get_reward(Down));
        EXPECT_EQ(board_arr_16.get_reward(Down), board_opt.get_reward(Down));

        board_arr_16.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_arr_16.slide(Left);
        board_64.slide(Left);
        board_opt.slide(Left);
        EXPECT_EQ(board_arr_16.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_arr_16.get_matrix(), board_opt.get_matrix());

        board_arr_16.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_arr_16.slide(Right);
        board_64.slide(Right);
        board_opt.slide(Right);
        EXPECT_EQ(board_arr_16.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_arr_16.get_matrix(), board_opt.get_matrix());

        board_arr_16.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_arr_16.slide(Up);
        board_64.slide(Up);
        board_opt.slide(Up);
        EXPECT_EQ(board_arr_16.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_arr_16.get_matrix(), board_opt.get_matrix());

        board_arr_16.load_matrix(board);
        board_64.load_matrix(board);
        board_opt.load_matrix(board);
        board_arr_16.slide(Down);
        board_64.slide(Down);
        board_opt.slide(Down);
        EXPECT_EQ(board_arr_16.get_matrix(), board_64.get_matrix());
        EXPECT_EQ(board_arr_16.get_matrix(), board_opt.get_matrix());
    }
    for (const auto &board: generate_boards<12, 17>(1000000)) {
        b_t_mat board_mat; // used as reference
        b_t_arr<l_t_32> board_arr_32;

        board_mat.load_matrix(board);
        board_arr_32.load_matrix(board);
        EXPECT_EQ(board_mat.get_reward(Left), board_arr_32.get_reward(Left));
        EXPECT_EQ(board_mat.get_reward(Right), board_arr_32.get_reward(Right));
        EXPECT_EQ(board_mat.get_reward(Up), board_arr_32.get_reward(Up));
        EXPECT_EQ(board_mat.get_reward(Down), board_arr_32.get_reward(Down));

        board_mat.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_mat.slide(Left);
        board_arr_32.slide(Left);
        EXPECT_EQ(board_mat.get_matrix(), board_arr_32.get_matrix());

        board_mat.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_mat.slide(Right);
        board_arr_32.slide(Right);
        EXPECT_EQ(board_mat.get_matrix(), board_arr_32.get_matrix());

        board_mat.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_mat.slide(Up);
        board_arr_32.slide(Up);
        EXPECT_EQ(board_mat.get_matrix(), board_arr_32.get_matrix());

        board_mat.load_matrix(board);
        board_arr_32.load_matrix(board);
        board_mat.slide(Down);
        board_arr_32.slide(Down);
        EXPECT_EQ(board_mat.get_matrix(), board_arr_32.get_matrix());
    }
}
