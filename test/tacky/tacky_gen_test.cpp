#include "ast.hpp"
#include "base_test.hpp"
#include "tacky_formatters.hpp"
#include "tacky_gen.hpp"
#include "tacky_matchers.hpp"

#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <tuple>
#include <utility>
#include <vector>

using namespace wacc::front::ast;
using namespace wacc::tacky::ast;
using namespace wacc::test::match;
using namespace wacc::test::fmt;

using wacc::tacky::gen::IdentifierGenerator;
using wacc::tacky::gen::TackyGenerator;

using std::pair;
using std::string;
using std::vector;

class TackyGeneratorTest :
    public testing::Test,
    public wacc::test::base::BaseTest {};

TEST(IdentifierGeneratorTest, testGenerateThrow) {
    // ARRANGE
    auto generator = IdentifierGenerator{};

    // ACT
    string error{};

    try {
        generator.generate(0);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("IdentifierGenerator:"));
    ASSERT_TRUE(error.contains("length of zero."));
}

TEST(IdentifierGeneratorTest, testGeneratePre) {
    // ARRANGE
    const auto pre = "$$@";
    const auto length = 4;
    auto generator = IdentifierGenerator{};

    // ACT
    auto result = generator.generate(length, pre);

    // ASSERT
    ASSERT_TRUE(result.starts_with(pre));
    ASSERT_EQ(result.length(), 7);
}

TEST(IdentifierGeneratorTest, testGeneratePost) {
    // ARRANGE
    const auto post = "$$@";
    const auto length = 5;
    auto generator = IdentifierGenerator{};

    // ACT
    auto result = generator.generate(length, "", post);

    // ASSERT
    ASSERT_TRUE(result.ends_with(post));
    ASSERT_EQ(result.length(), 8);
}

TEST(IdentifierGeneratorTest, testGenerate) {
    // ARRANGE
    const auto length = 10;
    const auto pre = "#@#";
    const auto post = "$$@";
    auto generator = IdentifierGenerator{};

    // ACT
    auto result = generator.generate(length, pre, post);

    // ASSERT
    ASSERT_TRUE(result.starts_with(pre));
    ASSERT_TRUE(result.ends_with(post));
    ASSERT_EQ(result.length(), 16);
}

TEST(IdentifierGeneratorTest, testGenerateVariable) {
    // ARRANGE
    const auto session = "S1A2B";
    const auto function = "main";
    auto generator = IdentifierGenerator{};
    generator.resetSession(session);
    generator.resetFunction(function);

    // ACT
    auto first = generator.generateVariable();
    auto second = generator.generateVariable();

    // ASSERT
    ASSERT_STREQ(first.c_str(), "S1A2B.MAIN.TEMP.0");
    ASSERT_STREQ(second.c_str(), "S1A2B.MAIN.TEMP.1");
}

TEST(IdentifierGeneratorTest, testGenerateLabel) {
    // ARRANGE
    const auto session = "S1A2B";
    const auto function = "main";
    const auto op = TackyBinary::Type::BINARY_LOG_AND;

    auto generator = IdentifierGenerator{};
    generator.resetSession(session);
    generator.resetFunction(function);

    // ACT
    auto label = generator.generateLabel(op, "L_FALSE");

    // ASSERT
    ASSERT_STREQ(label.c_str(), "S1A2B.MAIN.BINARY_LOG_AND.0.L_FALSE");
}

TEST(IdentifierGeneratorTest, testGenerateLabelRandom) {
    // ARRANGE
    const auto session = "S1A2B";
    const auto function = "main";
    const auto opFirst = TackyBinary::Type::BINARY_LOG_AND;
    const auto opSecond = TackyBinary::Type::BINARY_LOG_OR;

    auto generator = IdentifierGenerator{};
    generator.resetSession(session);
    generator.resetFunction(function);

    // ACT
    auto first = generator.generateLabel(opFirst, "L_TRUE");
    auto second = generator.generateLabel(opSecond, "L_FALSE");

    // ASSERT
    ASSERT_STREQ(first.c_str(), "S1A2B.MAIN.BINARY_LOG_AND.0.L_TRUE");
    ASSERT_STREQ(second.c_str(), "S1A2B.MAIN.BINARY_LOG_OR.1.L_FALSE");
}

TEST(IdentifierGeneratorTest, testGenerateRandom) {
    // ARRANGE
    const auto function = "main";
    auto generator = IdentifierGenerator{};
    generator.resetFunction(function);

    // ACT
    auto first = generator.generateVariable();
    auto second = generator.generateVariable();

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
            "[-3 -> MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return !15; }", {
            "[!15 -> MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return -!23; }", {
            "[!23 -> MAIN.TEMP.0]",
            "[-MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return ~!(-(!8)); }", {
            "[!8 -> MAIN.TEMP.0]",
            "[-MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[!MAIN.TEMP.1 -> MAIN.TEMP.2]",
            "[~MAIN.TEMP.2 -> MAIN.TEMP.3]",
            "[R:MAIN.TEMP.3]",
         }},
        {"{ return ~8; }", {
            "[~8 -> MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return ~(-15); }", {
            "[-15 -> MAIN.TEMP.0]",
            "[~MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return -(~(-80)); }", {
            "[-80 -> MAIN.TEMP.0]",
            "[~MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[-MAIN.TEMP.1 -> MAIN.TEMP.2]",
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
            "[1 + 2 -> MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return 1 + 2 + 3; }", {
            "[1 + 2 -> MAIN.TEMP.0]",
            "[MAIN.TEMP.0 + 3 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 1 - (2 + 3); }", {
            "[2 + 3 -> MAIN.TEMP.0]",
            "[1 - MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 1 * 2 + 3; }", {
            "[1 * 2 -> MAIN.TEMP.0]",
            "[MAIN.TEMP.0 + 3 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 3 - 2 % 1; }", {
            "[2 % 1 -> MAIN.TEMP.0]",
            "[3 - MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 3 - 2 % 1; }", {
            "[2 % 1 -> MAIN.TEMP.0]",
            "[3 - MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 1 * 2 + 3 / 4; }", {
            "[1 * 2 -> MAIN.TEMP.0]",
            "[3 / 4 -> MAIN.TEMP.1]",
            "[MAIN.TEMP.0 + MAIN.TEMP.1 -> MAIN.TEMP.2]",
            "[R:MAIN.TEMP.2]",
         }},
        {"{ return (5 + -8) / ~3; }", {
            "[-8 -> MAIN.TEMP.0]",
            "[5 + MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[~3 -> MAIN.TEMP.2]",
            "[MAIN.TEMP.1 / MAIN.TEMP.2 -> MAIN.TEMP.3]",
            "[R:MAIN.TEMP.3]",
         }},
        {"{ return 33 == 87; }", {
            "[33 == 87 -> MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
        {"{ return 1 == 2 != 9; }", {
            "[1 == 2 -> MAIN.TEMP.0]",
            "[MAIN.TEMP.0 != 9 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
        {"{ return 1 << 2 == 9 >> !9; }", {
            "[1 << 2 -> MAIN.TEMP.0]",
            "[!9 -> MAIN.TEMP.1]",
            "[9 >> MAIN.TEMP.1 -> MAIN.TEMP.2]",
            "[MAIN.TEMP.0 == MAIN.TEMP.2 -> MAIN.TEMP.3]",
            "[R:MAIN.TEMP.3]",
         }},
        {"{ return 7 * -5 != 9; }", {
            "[-5 -> MAIN.TEMP.0]",
            "[7 * MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[MAIN.TEMP.1 != 9 -> MAIN.TEMP.2]",
            "[R:MAIN.TEMP.2]",
         }},
         {"{ return 8 << 2; }", {
            "[8 << 2 -> MAIN.TEMP.0]",
            "[R:MAIN.TEMP.0]",
         }},
         {"{ return 1 >> 2 << 3; }", {
            "[1 >> 2 -> MAIN.TEMP.0]",
            "[MAIN.TEMP.0 << 3 -> MAIN.TEMP.1]",
            "[R:MAIN.TEMP.1]",
         }},
         {"{ return 2 & -32 | 3; }", {
            "[-32 -> MAIN.TEMP.0]",
            "[2 & MAIN.TEMP.0 -> MAIN.TEMP.1]",
            "[MAIN.TEMP.1 | 3 -> MAIN.TEMP.2]",
            "[R:MAIN.TEMP.2]",
         }},
         {"{ return 15 * (8 + 9) >> -1 + ~5 / 8; }", {
             "[8 + 9 -> MAIN.TEMP.0]",
             "[15 * MAIN.TEMP.0 -> MAIN.TEMP.1]",
             "[-1 -> MAIN.TEMP.2]",
             "[~5 -> MAIN.TEMP.3]",
             "[MAIN.TEMP.3 / 8 -> MAIN.TEMP.4]",
             "[MAIN.TEMP.2 + MAIN.TEMP.4 -> MAIN.TEMP.5]",
             "[MAIN.TEMP.1 >> MAIN.TEMP.5 -> MAIN.TEMP.6]",
             "[R:MAIN.TEMP.6]",
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

TEST_F(TackyGeneratorTest, generateBinaryLogAndOr) {
    // ARRANGE
    const auto tests = vector<pair<string, vector<string>>>{
        // clang-format off
        {"{ return 3 > 2 && 2 <= 1; }", {
            "[3 > 2 -> MAIN.TEMP.0]",
            "[JMPZ MAIN.TEMP.0 BINARY_LOG_AND.0.L_FALSE]",
            "[2 <= 1 -> MAIN.TEMP.1]",
            "[JMPZ MAIN.TEMP.1 BINARY_LOG_AND.0.L_FALSE]",
            "[1 -> MAIN.TEMP.2]",
            "[JMP BINARY_LOG_AND.1.L_END]",
            "[LBL BINARY_LOG_AND.0.L_FALSE]",
            "[0 -> MAIN.TEMP.2]",
            "[LBL BINARY_LOG_AND.1.L_END]",
            "[R:MAIN.TEMP.2]",
         }},
        {"{ return 15 == 2 || 9 < 16; }", {
            "[15 == 2 -> MAIN.TEMP.0]",
            "[JMPNZ MAIN.TEMP.0 BINARY_LOG_OR.0.L_TRUE]",
            "[9 < 16 -> MAIN.TEMP.1]",
            "[JMPNZ MAIN.TEMP.1 BINARY_LOG_OR.0.L_TRUE]",
            "[0 -> MAIN.TEMP.2]",
            "[JMP BINARY_LOG_OR.1.L_END]",
            "[LBL BINARY_LOG_OR.0.L_TRUE]",
            "[1 -> MAIN.TEMP.2]",
            "[LBL BINARY_LOG_OR.1.L_END]",
            "[R:MAIN.TEMP.2]",
         }},
         {"{ return 1 > 2 || 3 < 4 && 5 != 6; }", {
            "[1 > 2 -> MAIN.TEMP.0]",
            "[JMPNZ MAIN.TEMP.0 BINARY_LOG_OR.0.L_TRUE]",
            "[3 < 4 -> MAIN.TEMP.1]",
            "[JMPZ MAIN.TEMP.1 BINARY_LOG_AND.2.L_FALSE]",
            "[5 != 6 -> MAIN.TEMP.2]",
            "[JMPZ MAIN.TEMP.2 BINARY_LOG_AND.2.L_FALSE]",
            "[1 -> MAIN.TEMP.3]",
            "[JMP BINARY_LOG_AND.3.L_END]",
            "[LBL BINARY_LOG_AND.2.L_FALSE]",
            "[0 -> MAIN.TEMP.3]",
            "[LBL BINARY_LOG_AND.3.L_END]",
            "[JMPNZ MAIN.TEMP.3 BINARY_LOG_OR.0.L_TRUE]",
            "[0 -> MAIN.TEMP.4]",
            "[JMP BINARY_LOG_OR.1.L_END]",
            "[LBL BINARY_LOG_OR.0.L_TRUE]",
            "[1 -> MAIN.TEMP.4]",
            "[LBL BINARY_LOG_OR.1.L_END]",
            "[R:MAIN.TEMP.4]",
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
        for (auto i = 0; i < std::get<1>(test).size(); ++i) {
            auto actual = formatTackyInstr(body[i]);
            auto expected = std::get<1>(test)[i];
            ASSERT_STREQ(actual.c_str(), expected.c_str());
        }
    }
}
