#include "e2e_base.hpp"

using namespace wacc::test::e2e;

class Chapter4Test : public testing::Test {
protected:
    static void SetUpTestSuite() { setUpSuite(chapter); }

    static void TearDownTestSuite() { tearDownSuite(chapter); }

    static constexpr auto chapter = "ch04";
};

TEST_F(Chapter4Test, chapter4e2eLogNot) {
    // ARRANGE
    vector<pair<string, int>> tests{
        {"{ return !0; }",          1},
        {"{ return !1; }",          0},
        {"{ return !15; }",         0},
        {"{ return !-32; }",        0},
        {"{ return !(48 * 27); }",  0},
        {"{ return !(-22 + 22); }", 1},
        {"{ return !2 + !3; }",     0},
        {"{ return !(!2 + !3); }",  1},
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

TEST_F(Chapter4Test, chapter4e2eAndOr) {
    // ARRANGE
    vector<pair<string, int>> tests{
        {"{ return 15 && 30; }",             1},
        {"{ return 30 && !1; }",             0},
        {"{ return !1 && 22; }",             0},
        {"{ return -10 + 10 && -10 + 10; }", 0},
        {"{ return 20 || 30; }",             1},
        {"{ return 30 || 2 + -2; }",         1},
        {"{ return 8 * 0 || 1; }",           1},
        {"{ return 8 * 0 || 0 * -15; }",     0},
        {"{ return 12 && 0 || 3 * 0; }",     0},
        {"{ return 12 || 0 && 3 * 0; }",     1},
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

TEST_F(Chapter4Test, chapter4e2eRelational) {
    // ARRANGE
    vector<pair<string, int>> tests{
        {"{ return 15 < 30; }",                  1},
        {"{ return -15 < 15; }",                 1},
        {"{ return 28 > 1; }",                   1},
        {"{ return -28 > -27; }",                0},
        {"{ return 15 <= 15; }",                 1},
        {"{ return 5 * 3 <= 10; }",              0},
        {"{ return 2 >= 1 + 1; }",               1},
        {"{ return 32 / 2 >= 15 * 2; }",         0},
        {"{ return 5 != 5; }",                   0},
        {"{ return 9 != 3 + 3 + 2; }",           1},
        {"{ return 8 == 2 * 4; }",               1},
        {"{ return 1 + 2 + 3 == 3 + 1 + 2; }",   1},
        {"{ return 3 < 2 != 3 >= 2; }",          1},
        {"{ return (!2 && 3) * (8 + 5) == 0; }", 1},
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