#include "e2e_base.hpp"

using namespace wacc::test::e2e;

class Chapter3Test : public testing::Test {
protected:
    static void SetUpTestSuite() { setUpSuite(chapter); }

    static void TearDownTestSuite() { tearDownSuite(chapter); }

    static constexpr auto chapter = "ch03";
};

TEST_F(Chapter3Test, chapter3e2e) {
    // ARRANGE
    vector<pair<string, int>> tests{
        // clang-format off
        {"{ return 0 + 0; }", 0},
        {"{ return 1 + 2; }", 3},
        {"{ return 3 - 1; }", 2},
        {"{ return 5 - 2; }", 3},
        {"{ return 2 * 3; }", 6},
        {"{ return 12 * 3; }", 36},
        {"{ return 5 / 2; }", 2},
        {"{ return 8 / 4; }", 2},
        {"{ return 5 % 2; }", 1},
        {"{ return 9 % 3; }", 0},
        {"{ return 1 + 2 + 3; }", 6},
        {"{ return 9 - 4 - 1; }", 4},
        {"{ return 12 + 1 - 3; }", 10},
        {"{ return 11 - 1 + 8; }", 18},
        {"{ return 3 * 3 * 3; }", 27},
        {"{ return 64 / 8 / 8; }", 1},
        {"{ return 9 * 3 / 3; }", 9},
        {"{ return 64 / 8 * 2; }", 16},
        {"{ return 8 % 4 * 10; }", 0},
        {"{ return 1 + 2 * 3; }", 7},
        {"{ return 1 / 2 + 3; }", 3},
        {"{ return 2 * 4 - 7; }", 1},
        {"{ return 40 - 5 % 7; }", 35},
        {"{ return (2 + 8) / 4; }", 2},
        {"{ return (2 + 8) / 4 + 5; }", 7},
        {"{ return -2 + 5 * (-(-32) / 8); }", 18},
        // clang-format on
    };

    for (const auto& pair : tests) {
        // ACT
        const auto expected = std::get<int>(pair);

        auto code = std::format("int main(void) {}", std::get<string>(pair));
        auto compiled = compile(code, chapter);
        EXPECT_TRUE(compiled.has_value());

        // ASSERT
        auto binaryPath = compiled.value();
        auto actual = run(binaryPath);
        ASSERT_EQ(actual, expected);
    }
}