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
    vector<string> tests = {
        // clang-format off
        "int main(void) { return 4; }", 
        "int main(void) { return 8; }", 
        "int main(void) { return 15; }",
        "int main(void) { return 16; }",
        "int main(void) { return 23; }",
        "int main(void) { return 42; }",
        "int main(void) { return 25; }",
        "int main(void) { return 88; }",
        "int main(void) { return 71; }",
        "int main(void) { return 99; }",
        "int main(void) { return 101; }",
        "int main(void) { return 19; }",
        // clang-format on
    };

    for (const auto& code : tests) {
        // ACT
        auto compiled = compile(code, chapter);
        EXPECT_TRUE(compiled.has_value());

        // ASSERT
        auto binaryPath = compiled.value();
        auto exitCode = run(binaryPath);
        ASSERT_NE(exitCode, 0);
    }
}