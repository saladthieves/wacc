#include "e2e_base.hpp"
#include <print>

using namespace wacc::test::e2e;

class Chapter2Test : public testing::Test {
protected:
    static void SetUpTestSuite() { setUpSuite(chapter); }

    static void TearDownTestSuite() { tearDownSuite(chapter); }

    static constexpr auto chapter = "ch02";
};

TEST_F(Chapter2Test, chapter2e2e) {
    // ARRANGE
    vector<pair<string, int>> tests = {
        // clang-format off
        { "{ return 1; }", 1},
        { "{ return (15); }", (15)},
        { "{ return (42); }", (42)},
        { "{ return 10; }", 10},
        { "{ return 80; }", 80},
        { "{ return ~~25; }", ~~25},
        { "{ return ~~9; }", ~~9},
        { "{ return ~~18; }", ~~18},
        { "{ return ~~27; }", ~~27},
        { "{ return -(-30); }", 30},
        { "{ return (5); }", (5)},
        { "{ return -(~3); }", -(~3)},
        { "{ return -(~9); }", -(~9)},
        { "{ return ~(-9); }", ~(-9)},
        { "{ return ~(-3); }", ~(-3)},
        { "{ return ~~(-9); }", 247},
        { "{ return ~~(3); }", ~~(3)},
        { "{ return ~~(99); }", ~~(99)},
        { "{ return ~~(~98); }", 157},
        { "{ return ~-(~5); }", 249},
        { "{ return -~(-55); }", 202},
        { "{ return -~(-(~6)); }", 8},
        { "{ return ~(~(~12)); }", 243},
        { "{ return -(~(-19)); }", 238},
        // clang-format on
    };

    for (const auto& pair : tests) {
        const auto expected = std::get<int>(pair);

        auto code = std::format("int main(void) {}", std::get<string>(pair));
        auto compiled = compile(code, chapter);
        EXPECT_TRUE(compiled.has_value());

        // ASSERT
        auto binaryPath = compiled.value();
        auto actual = run(binaryPath);
        EXPECT_EQ(actual, expected);
    }
}