#include "asm_ast.hpp"
#include "asm_formatters.hpp"
#include "asm_gen.hpp"
#include "asm_matchers.hpp"
#include "base_test.hpp"
#include "tacky_ast.hpp"

#include <cstddef>
#include <gtest/gtest.h>
#include <tuple>
#include <utility>
#include <vector>

using namespace wacc::back::ast;
using namespace wacc::tacky::ast;
using namespace wacc::test::match;
using namespace wacc::test::fmt;

using wacc::back::gen::AsmGenerator;

using std::pair;
using std::string;
using std::tuple;
using std::vector;

class AsmGeneratorTest :
    public testing::Test,
    public wacc::test::base::BaseTest {};

TEST_F(AsmGeneratorTest, throwOnNull) {
    // ARRANGE
    auto generator = AsmGenerator{nullptr};
    string error{};

    // ACT
    try {
        generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("root tree is null."));
}

TEST_F(AsmGeneratorTest, generateNegate) {
    // ARRANGE
    const auto src = "int main(void) { return -42; }";
    auto generator = getAsmGenerator(src);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 4);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 42);
        matchAsmPseudo(dest, "MAIN.TEMP.0");
    });

    matchAsmUnary(body[1], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnary::Type::UNARY_NEGATE);
        matchAsmPseudo(operand, "MAIN.TEMP.0");
    });

    matchAsmMov(body[2], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.0");
        matchAsmReg(dest, AsmReg::Type::AX);
    });

    matchAsmRet(body[3]);
}

TEST_F(AsmGeneratorTest, generateUnary) {
    // ARRANGE
    const auto code = "int main(void) { return ~(-(~25)); }";
    auto generator = getAsmGenerator(code);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 8);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 25);
        matchAsmPseudo(dest, "MAIN.TEMP.0");
    });

    matchAsmUnary(body[1], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnary::Type::UNARY_NOT);
        matchAsmPseudo(operand, "MAIN.TEMP.0");
    });

    matchAsmMov(body[2], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.0");
        matchAsmPseudo(dest, "MAIN.TEMP.1");
    });

    matchAsmUnary(body[3], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnary::Type::UNARY_NEGATE);
        matchAsmPseudo(operand, "MAIN.TEMP.1");
    });

    matchAsmMov(body[4], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.1");
        matchAsmPseudo(dest, "MAIN.TEMP.2");
    });

    matchAsmUnary(body[5], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnary::Type::UNARY_NOT);
        matchAsmPseudo(operand, "MAIN.TEMP.2");
    });

    matchAsmMov(body[6], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.2");
        matchAsmReg(dest, AsmReg::Type::AX);
    });

    matchAsmRet(body[7]);
}

TEST_F(AsmGeneratorTest, generateBinarySimple) {
    // ARRANGE
    using enum AsmBinary::Type;
    const auto tests = vector<tuple<string, int, AsmBinary::Type, int>>{
        {"{ return 1 + 2; }",           1,  BINARY_ADD,     2},
        {"{ return 2 - 1; }",           2,  BINARY_SUB,     1},
        {"{ return 7 * 0; }",           7,  BINARY_MULT,    0},
        {"{ return 3 - (5); }",         3,  BINARY_SUB,     5},
        {"{ return (8) * ((7)); }",     8,  BINARY_MULT,    7},
        {"{ return ((2)) + (((9))); }", 2,  BINARY_ADD,     9},
        {"{ return 16 << 1; }",         16, BINARY_BIT_LSH, 1},
        {"{ return (8) >> ((2)) ; }",   8,  BINARY_BIT_RSH, 2},
        {"{ return 1 & 0 ; }",          1,  BINARY_BIT_AND, 0},
        {"{ return 24 ^ ((4)) ; }",     24, BINARY_BIT_XOR, 4},
        {"{ return (3) | 1 ; }",        3,  BINARY_BIT_OR,  1},
    };

    for (const auto& test : tests) {
        auto code = std::format("int main(void) {}", std::get<0>(test));
        auto generator = getAsmGenerator(code);

        // ACT
        auto ast = generator.generate();

        // ASSERT
        const auto& body = matchAsmProg(ast);
        matchAsmMov(body[0], [&](auto& src, auto& dest) {
            matchAsmImm(src, std::get<1>(test));
            matchAsmPseudo(dest, ".MAIN.TEMP.0");
        });

        matchAsmBinary(body[1], [&](auto& op, auto& src, auto& dest) {
            ASSERT_EQ(op, std::get<2>(test));
            matchAsmImm(src, std::get<3>(test));
            matchAsmPseudo(dest, ".MAIN.TEMP.0");
        });
    }
}

TEST_F(AsmGeneratorTest, generateBinaryDivRem) {
    // ARRANGE
    using enum AsmBinary::Type;
    const auto tests = vector<tuple<string, int, bool, int>>{
        {"{ return 5 / 2; }",    5,  true,  2 },
        {"{ return (15) % 3; }", 15, false, 3 },
        {"{ return 10 / 0; }",   10, true,  0 },
        {"{ return 6 % (10); }", 6,  false, 10},
        {"{ return 8 / 12; }",   8,  true,  12},
        {"{ return (2) % 0; }",  2,  false, 0 },
    };

    for (const auto& test : tests) {
        auto code = std::format("int main(void) {}", std::get<0>(test));
        auto generator = getAsmGenerator(code);

        // ACT
        auto ast = generator.generate();

        // ASSERT
        const auto& body = matchAsmProg(ast);

        matchAsmMov(body[0], [&](auto& src, auto& dest) {
            matchAsmImm(src, std::get<1>(test));
            matchAsmReg(dest, AsmReg::Type::AX);
        });

        matchAsmCdq(body[1]);

        matchAsmIdiv(body[2], [&](auto& operand) {
            matchAsmImm(operand, std::get<3>(test));
        });

        matchAsmMov(body[3], [&](auto& src, auto& dest) {
            matchAsmReg(src, std::get<bool>(test) ? AsmReg::Type::AX
                                                  : AsmReg::Type::DX);
            matchAsmPseudo(dest, ".MAIN.TEMP.0");
        });
    }
}

TEST_F(AsmGeneratorTest, generateBinary) {
    // ARRANGE
    const auto tests = vector<pair<string, vector<string>>>{
        {"{ return 1 + 2; }",
         {
             "<MOV 1 -> MAIN.TEMP.0>",
             "<BINARY 2 ADD MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 3 - 8; }",
         {
             "<MOV 3 -> MAIN.TEMP.0>",
             "<BINARY 8 SUB MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 10 * 0; }",
         {
             "<MOV 10 -> MAIN.TEMP.0>",
             "<BINARY 0 MULT MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 10 / 2; }",
         {
             "<MOV 10 -> AX>",
             "<CDQ>",
             "<IDIV 2>",
             "<MOV AX -> MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 87 % 3; }",
         {
             "<MOV 87 -> AX>",
             "<CDQ>",
             "<IDIV 3>",
             "<MOV DX -> MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return -3 + 8; }",
         {
             "<MOV 3 -> MAIN.TEMP.0>",
             "<UNARY NEG MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> MAIN.TEMP.1>",
             "<BINARY 8 ADD MAIN.TEMP.1>",
             "<MOV MAIN.TEMP.1 -> AX>",
             "<RET>",
         }},
        {"{ return 7 * ~8; }",
         {
             "<MOV 8 -> MAIN.TEMP.0>",
             "<UNARY NOT MAIN.TEMP.0>",
             "<MOV 7 -> MAIN.TEMP.1>",
             "<BINARY MAIN.TEMP.0 MULT MAIN.TEMP.1>",
             "<MOV MAIN.TEMP.1 -> AX>",
             "<RET>",
         }},
        {"{ return -5 / -2; }",
         {
             "<MOV 5 -> MAIN.TEMP.0>",
             "<UNARY NEG MAIN.TEMP.0>",
             "<MOV 2 -> MAIN.TEMP.1>",
             "<UNARY NEG MAIN.TEMP.1>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<CDQ>",
             "<IDIV MAIN.TEMP.1>",
             "<MOV AX -> MAIN.TEMP.2>",
             "<MOV MAIN.TEMP.2 -> AX>",
             "<RET>",
         }},
        {"{ return 1 + 2 * 3; }",
         {
             "<MOV 2 -> MAIN.TEMP.0>",
             "<BINARY 3 MULT MAIN.TEMP.0>",
             "<MOV 1 -> MAIN.TEMP.1>",
             "<BINARY MAIN.TEMP.0 ADD MAIN.TEMP.1>",
             "<MOV MAIN.TEMP.1 -> AX>",
             "<RET>",
         }},
        {"{ return 3 >> 2; }",
         {
             "<MOV 3 -> MAIN.TEMP.0>",
             "<BINARY 2 RSH MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 16 << 1; }",
         {
             "<MOV 16 -> MAIN.TEMP.0>",
             "<BINARY 1 LSH MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 2 & 1; }",
         {
             "<MOV 2 -> MAIN.TEMP.0>",
             "<BINARY 1 AND MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 13 ^ 2; }",
         {
             "<MOV 13 -> MAIN.TEMP.0>",
             "<BINARY 2 XOR MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 5 | 9; }",
         {
             "<MOV 5 -> MAIN.TEMP.0>",
             "<BINARY 9 OR MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> AX>",
             "<RET>",
         }},
        {"{ return 1 & 2 | 3 ^ 4; }",
         {
             "<MOV 1 -> MAIN.TEMP.0>",
             "<BINARY 2 AND MAIN.TEMP.0>",
             "<MOV 3 -> MAIN.TEMP.1>",
             "<BINARY 4 XOR MAIN.TEMP.1>",
             "<MOV MAIN.TEMP.0 -> MAIN.TEMP.2>",
             "<BINARY MAIN.TEMP.1 OR MAIN.TEMP.2>",
             "<MOV MAIN.TEMP.2 -> AX>",
             "<RET>",
         }},
        {"{ return ~8 << 9 >> -3; }",
         {
             "<MOV 8 -> MAIN.TEMP.0>",
             "<UNARY NOT MAIN.TEMP.0>",
             "<MOV MAIN.TEMP.0 -> MAIN.TEMP.1>",
             "<BINARY 9 LSH MAIN.TEMP.1>",
             "<MOV 3 -> MAIN.TEMP.2>",
             "<UNARY NEG MAIN.TEMP.2>",
             "<MOV MAIN.TEMP.1 -> MAIN.TEMP.3>",
             "<BINARY MAIN.TEMP.2 RSH MAIN.TEMP.3>",
             "<MOV MAIN.TEMP.3 -> AX>",
             "<RET>",
         }},
    };

    for (const auto& test : tests) {
        auto code = std::format("int main(void) {}", std::get<0>(test));
        auto generator = getAsmGenerator(code);

        // ACT
        auto ast = generator.generate();

        // ASSERT
        auto& body = matchAsmProg(ast);

        for (auto i = 0; i < body.size(); ++i) {
            const auto& expected = std::get<1>(test)[i];
            const auto actual = formatAsmInstr(body[i]);
            ASSERT_STREQ(expected.c_str(), actual.c_str());
        }
    }
}