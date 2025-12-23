#include "e2e_base.hpp"

using namespace wacc::test::e2e;

class Chapter2Test : public testing::Test {
protected:
    static void SetUpTestSuite() { setUpSuite(chapter); }

    static void TearDownTestSuite() { tearDownSuite(chapter); }

    static constexpr auto chapter = "ch02";
};

TEST_F(Chapter2Test, chapter2e2e) {
    // ARRANGE
    // ARRANGE
    vector<string> tests = {
        // clang-format off
        "int main(void) { return 1; }", 
        "int main(void) { return (15); }", 
        "int main(void) { return (42); }", 
        "int main(void) { return -10; }", 
        "int main(void) { return -80; }", 
        "int main(void) { return ~25; }", 
        "int main(void) { return ~9; }", 
        "int main(void) { return ~~18; }", 
        "int main(void) { return ~~27; }", 
        "int main(void) { return -(30); }", 
        "int main(void) { return -(5); }", 
        "int main(void) { return -(~3); }", 
        "int main(void) { return -(~9); }",
        "int main(void) { return ~(-9); }", 
        "int main(void) { return ~(-3); }",
        "int main(void) { return ~~(-9); }", 
        "int main(void) { return ~~(3); }",
        "int main(void) { return ~~(99); }",
        "int main(void) { return ~~(~98); }",
        "int main(void) { return ~-(~5); }",
        "int main(void) { return -~(-55); }",
        "int main(void) { return -~(-(~6)); }",
        "int main(void) { return ~(~(~12)); }",
        "int main(void) { return -(~(-19)); }",
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