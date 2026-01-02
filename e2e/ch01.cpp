#include "e2e_base.hpp"

#include <expected>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>

using namespace wacc::test::e2e;

using std::string;
using std::string_view;
using std::vector;

class Chapter1Test : public testing::Test {
protected:
    static void SetUpTestSuite() { setUpSuite(chapter); }

    static void TearDownTestSuite() { tearDownSuite(chapter); }

    static constexpr auto chapter = "ch01";
};

TEST_F(Chapter1Test, chapter1e2e) {
    // ARRANGE
    vector<pair<string, int>> tests = {
        // clang-format off
        { "{ return 4; }", 4}, 
        { "{ return 8; }", 8}, 
        { "{ return 15; }", 15},
        { "{ return 16; }", 16},
        { "{ return 23; }", 23},
        { "{ return 42; }", 42},
        { "{ return 25; }", 25},
        { "{ return 88; }", 88},
        { "{ return 71; }", 71},
        { "{ return 99; }", 99},
        { "{ return 101; }", 101},
        { "{ return 19; }", 19},
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
        ASSERT_EQ(actual, expected);
    }
}