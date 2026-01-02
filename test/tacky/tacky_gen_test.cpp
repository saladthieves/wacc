#include "ast.hpp"
#include "base_test.hpp"
#include "tacky_formatters.hpp"
#include "tacky_matchers.hpp"
#include "tacky_gen.hpp"

#include <gtest/gtest.h>
#include <tuple>
#include <utility>
#include <vector>

using namespace wacc::front::ast;
using namespace wacc::tacky::ast;
using namespace wacc::test::match;
using namespace wacc::test::fmt;

using wacc::tacky::gen::TackyGenerator;
using wacc::tacky::gen::VariableGenerator;

using std::pair;
using std::string;
using std::vector;

class TackyGeneratorTest :
    public testing::Test,
    public wacc::test::base::BaseTest {};

TEST(VariableGeneratorTest, testGenerate) {
    // ARRANGE
    const auto session = "S1A2B";
    const auto function = "main";
    auto generator = VariableGenerator{};
    generator.resetSession(session);
    generator.resetFunction(function);

    // ACT
    auto first = generator.generate();
    auto second = generator.generate();

    // ASSERT
    ASSERT_STREQ(first.c_str(), "S1A2B.MAIN.TEMP.0");
    ASSERT_STREQ(second.c_str(), "S1A2B.MAIN.TEMP.1");
}

TEST(VariableGeneratorTest, testGenerateRandom) {
    // ARRANGE
    const auto function = "main";
    auto generator = VariableGenerator{};
    generator.resetFunction(function);

    // ACT
    auto first = generator.generate();
    auto second = generator.generate();

    // ASSERT
    ASSERT_TRUE(first.starts_with("S"));
    ASSERT_TRUE(first.ends_with("MAIN.TEMP.0"));
    ASSERT_TRUE(second.starts_with("S"));
    ASSERT_TRUE(second.ends_with("MAIN.TEMP.1"));
}

TEST_F(TackyGeneratorTest, throwOnNull) {
    // ARRANGE
    auto generator = TackyGenerator{nullptr};
    string error{};

    // ACT
    try {
        generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("TackyGeneratorError"));
    ASSERT_TRUE(error.contains("AstNode root tree is null"));
}

TEST_F(TackyGeneratorTest, generateUnary) {
    // ARRANGE
    const auto tests = vector<pair<string, vector<string>>>{
        // clang-format off
        {"{ return 0; }",  {"[R:0]"} },
        {"{ return 15; }", {"[R:15]"}},
        {"{ return 80; }", {"[R:80]"}},
        {"{ return -3; }", {
            "[O:- S:3 D:MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return ~8; }", {
            "[O:~ S:8 D:MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return ~(-15); }", {
            "[O:- S:15 D:MAIN.TEMP.0]",
            "[O:~ S:MAIN.TEMP.0 D:MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return -(~(-80)); }", {
            "[O:- S:80 D:MAIN.TEMP.0]",
            "[O:~ S:MAIN.TEMP.0 D:MAIN.TEMP.1]",
            "[O:- S:MAIN.TEMP.1 D:MAIN.TEMP.2]",
            "[R:MAIN.TEMP.2]",
         }},
        // clang-format on
    };

    for (const auto& test : tests) {
        auto code = std::format("int main(void) {}", std::get<0>(test));
        auto generator = getTackyGenerator(code);

        // ACT
        auto node = generator.generate();

        // ASSERT
        const auto& body = matchTackyProg(node);
        for (auto i = 0; i < body.size(); ++i) {
            auto actual = std::get<1>(test)[i];
            auto expected = formatTackyInstr(body[i]);
            ASSERT_STREQ(actual.c_str(), expected.c_str());
        }
    }
}

TEST_F(TackyGeneratorTest, generateBinary) {
    // ARRANGE
    const auto tests = vector<pair<string, vector<string>>>{
        // clang-format off
        {"{ return 1 + 2; }", {
            "[S1:1 O:+ S2:2 D:MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return 1 + 2 + 3; }", {
            "[S1:1 O:+ S2:2 D:MAIN.TEMP.0]",
            "[S1:MAIN.TEMP.0 O:+ S2:3 D:MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 1 - (2 + 3); }", {
            "[S1:2 O:+ S2:3 D:MAIN.TEMP.0]",
            "[S1:1 O:- S2:MAIN.TEMP.0 D:MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 1 * 2 + 3; }", {
            "[S1:1 O:* S2:2 D:MAIN.TEMP.0]",
            "[S1:MAIN.TEMP.0 O:+ S2:3 D:MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 3 - 2 % 1; }", {
            "[S1:2 O:% S2:1 D:MAIN.TEMP.0]",
            "[S1:3 O:- S2:MAIN.TEMP.0 D:MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 1 * 2 + 3 / 4; }", {
            "[S1:1 O:* S2:2 D:MAIN.TEMP.0]",
            "[S1:3 O:/ S2:4 D:MAIN.TEMP.1]",
            "[S1:MAIN.TEMP.0 O:+ S2:MAIN.TEMP.1 D:MAIN.TEMP.2]",
            "[R:MAIN.TEMP.2]",
         }},
        {"{ return (5 + -8) / ~3; }", {
            "[O:- S:8 D:MAIN.TEMP.0]",
            "[S1:5 O:+ S2:MAIN.TEMP.0 D:MAIN.TEMP.1]",
            "[O:~ S:3 D:MAIN.TEMP.2]",
            "[S1:MAIN.TEMP.1 O:/ S2:MAIN.TEMP.2 D:MAIN.TEMP.3]",
            "[R:MAIN.TEMP.3]",
         }},
        // clang-format on
    };

    for (const auto& test : tests) {
        auto code = std::format("int main(void) {}", std::get<0>(test));
        auto generator = getTackyGenerator(code);

        // ACT
        auto node = generator.generate();

        // ASSERT
        const auto& body = matchTackyProg(node);
        for (auto i = 0; i < body.size(); ++i) {
            auto actual = std::get<1>(test)[i];
            auto expected = formatTackyInstr(body[i]);
            ASSERT_STREQ(actual.c_str(), expected.c_str());
        }
    }
}