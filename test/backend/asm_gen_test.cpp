#include "asm_ast.hpp"
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

using wacc::back::gen::AsmGenerator;

using std::make_unique;
using std::string;
using std::tuple;
using std::vector;

class AsmGeneratorTest :
    public testing::Test,
    public wacc::test::base::BaseTest {
protected:
    void addInstr(TackyInstrPtr ptr) {
        body.push_back(std::move(ptr)); //
    }

    TackyNodePtr genAsm() {
        auto fun = make_unique<TackyFun>("main", std::move(body));
        return make_unique<TackyProg>(std::move(fun));
    }

    TackyInstrPtrs body{};
};

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

TEST_F(AsmGeneratorTest, generateReturn) {
    // ARRANGE
    const auto code = "int main(void) { return 42; }";
    auto generator = getAsmGenerator(code);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 42);
        matchAsmReg(dest, AsmReg::Type::AX);
    });

    matchAsmRet(body[1]);
}

TEST_F(AsmGeneratorTest, generateUnaryComplNeg) {
    // ARRANGE
    using enum AsmUnary::Type;
    const auto tests = vector<tuple<string, AsmUnary::Type, int>>{
        {"{ return ~23; }", UNARY_COMPLEMENT, 23},
        {"{ return -15; }", UNARY_NEGATE,     15},
    };

    for (const auto& test : tests) {
        const auto code = std::format("int main(void) {}", std::get<0>(test));
        auto generator = getAsmGenerator(code);

        // ACT
        auto ast = generator.generate();

        // ASSERT
        const auto& body = matchAsmProg(ast);
        ASSERT_EQ(body.size(), 4);

        matchAsmMov(body[0], [&](auto& src, auto& dest) {
            matchAsmImm(src, std::get<int>(test));
            matchAsmPseudo(dest, "MAIN.TEMP.0");
        });

        matchAsmUnary(body[1], [&](auto& op, auto& operand) {
            ASSERT_EQ(op, std::get<1>(test));
            matchAsmPseudo(operand, "MAIN.TEMP.0");
        });

        matchAsmMov(body[2], [](auto& src, auto& dest) {
            matchAsmPseudo(src, "MAIN.TEMP.0");
            matchAsmReg(dest, AsmReg::Type::AX);
        });

        matchAsmRet(body[3]);
    }
}

TEST_F(AsmGeneratorTest, generateUnaryNot) {
    // ARRANGE
    const auto code = "int main(void) { return !17; }";
    auto generator = getAsmGenerator(code);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 5);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 0);
        matchAsmPseudo(dest, "MAIN.TEMP.0");
    });

    matchAsmCmp(body[1], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 17);
    });

    matchAsmSetCond(body[2], [](auto& condition, auto& operand) {
        ASSERT_EQ(condition, AsmSetCond::Code::EQUAL);
        matchAsmPseudo(operand, "MAIN.TEMP.0");
    });

    matchAsmMov(body[3], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.0");
        matchAsmReg(dest, AsmReg::Type::AX);
    });

    matchAsmRet(body[4]);
}

TEST_F(AsmGeneratorTest, generateUnary) {
    // ARRANGE
    const auto code = "int main(void) { return ~-(!9); }";

    // ACT
    auto generator = getAsmGenerator(code);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 9);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 0);
        matchAsmPseudo(dest, "MAIN.TEMP.0");
    });

    matchAsmCmp(body[1], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 9);
    });

    matchAsmSetCond(body[2], [](auto& condition, auto& operand) {
        ASSERT_EQ(condition, AsmSetCond::Code::EQUAL);
        matchAsmPseudo(operand, "MAIN.TEMP.0");
    });

    matchAsmMov(body[3], [&](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.0");
        matchAsmPseudo(dest, "MAIN.TEMP.1");
    });

    matchAsmUnary(body[4], [&](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnary::Type::UNARY_NEGATE);
        matchAsmPseudo(operand, "MAIN.TEMP.1");
    });

    matchAsmMov(body[5], [&](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.1");
        matchAsmPseudo(dest, "MAIN.TEMP.2");
    });

    matchAsmUnary(body[6], [&](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnary::Type::UNARY_COMPLEMENT);
        matchAsmPseudo(operand, "MAIN.TEMP.2");
    });

    matchAsmMov(body[7], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.2");
        matchAsmReg(dest, AsmReg::Type::AX);
    });

    matchAsmRet(body[8]);
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

TEST_F(AsmGeneratorTest, generateBinaryRelational) {
    // ARRANGE
    using enum AsmSetCond::Code;
    const auto tests = vector<tuple<string, int, AsmSetCond::Code, int>>{
        {"{ return 1 == 2; }",          1,  EQUAL,         2  },
        {"{ return 0 == 15; }",         0,  EQUAL,         15 },
        {"{ return 8 != 72; }",         8,  NOT_EQUAL,     72 },
        {"{ return 9 != 22; }",         9,  NOT_EQUAL,     22 },
        {"{ return (28) > 30; }",       28, GREATER,       30 },
        {"{ return 79 > 77; }",         79, GREATER,       77 },
        {"{ return 17 < ((122)); }",    17, LESS,          122},
        {"{ return 15 < 30; }",         15, LESS,          30 },
        {"{ return 11 >= 22; }",        11, GREATER_EQUAL, 22 },
        {"{ return (33) >= 55; }",      33, GREATER_EQUAL, 55 },
        {"{ return 19 <= 55; }",        19, LESS_EQUAL,    55 },
        {"{ return 91 <= (((191))); }", 91, LESS_EQUAL,    191},
    };
    for (const auto& test : tests) {
        const auto code =
            std::format("int main(void) {}", std::get<string>(test));
        auto generator = getAsmGenerator(code);

        // ACT
        auto ast = generator.generate();

        // ASSERT
        const auto& body = matchAsmProg(ast);
        ASSERT_EQ(body.size(), 5);

        matchAsmMov(body[0], [](auto& src, auto& dest) {
            matchAsmImm(src, 0);
            matchAsmPseudo(dest, ".MAIN.TEMP.0");
        });

        matchAsmCmp(body[1], [&](auto& left, auto& right) {
            matchAsmImm(right, std::get<1>(test));
            matchAsmImm(left, std::get<3>(test));
        });

        matchAsmSetCond(body[2], [&](auto& condition, auto& operand) {
            ASSERT_EQ(condition, std::get<2>(test));
            matchAsmPseudo(operand, ".MAIN.TEMP.0");
        });

        matchAsmMov(body[3], [](auto& src, auto& dest) {
            matchAsmPseudo(src, "MAIN.TEMP.0");
            matchAsmReg(dest, AsmReg::Type::AX);
        });

        matchAsmRet(body[4]);
    }
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

TEST_F(AsmGeneratorTest, generateBinaryAnd) {
    // ARRANGE
    const auto code = "int main(void) { return 28 && 30; }";
    const auto lFalse = ".BINARY_LOG_AND.0.L_FALSE";
    const auto lEnd = ".BINARY_LOG_AND.1.L_END";
    const auto lResult = ".MAIN.TEMP.0";

    auto generator = getAsmGenerator(code);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    matchAsmCmp(body[0], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 28);
    });
    matchAsmJmpCond(body[1], AsmJmpCond::Code::EQUAL, lFalse);

    matchAsmCmp(body[2], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 30);
    });
    matchAsmJmpCond(body[3], AsmJmpCond::Code::EQUAL, lFalse);

    matchAsmMov(body[4], [&](auto& src, auto& dest) {
        matchAsmImm(src, 1);
        matchAsmPseudo(dest, lResult);
    });
    matchAsmJmp(body[5], lEnd);

    matchAsmLabel(body[6], lFalse);
    matchAsmMov(body[7], [&](auto& src, auto& dest) {
        matchAsmImm(src, 0);
        matchAsmPseudo(dest, lResult);
    });
    matchAsmLabel(body[8], lEnd);
}

TEST_F(AsmGeneratorTest, generateBinaryOr) {
    // ARRANGE
    const auto code = "int main(void) { return 11 || 22; }";
    const auto lTrue = ".BINARY_LOG_OR.0.L_TRUE";
    const auto lEnd = ".BINARY_LOG_OR.1.L_END";
    const auto lResult = ".MAIN.TEMP.0";

    auto generator = getAsmGenerator(code);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    matchAsmCmp(body[0], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 11);
    });
    matchAsmJmpCond(body[1], AsmJmpCond::Code::NOT_EQUAL, lTrue);

    matchAsmCmp(body[2], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 22);
    });
    matchAsmJmpCond(body[3], AsmJmpCond::Code::NOT_EQUAL, lTrue);

    matchAsmMov(body[4], [&](auto& src, auto& dest) {
        matchAsmImm(src, 0);
        matchAsmPseudo(dest, lResult);
    });
    matchAsmJmp(body[5], lEnd);

    matchAsmLabel(body[6], lTrue);
    matchAsmMov(body[7], [&](auto& src, auto& dest) {
        matchAsmImm(src, 1);
        matchAsmPseudo(dest, lResult);
    });
    matchAsmLabel(body[8], lEnd);
}

TEST_F(AsmGeneratorTest, generateCopy) {
    // ARRANGE
    addInstr(make_unique<TackyCopy>(make_unique<TackyLitInt>(2),
                                    make_unique<TackyLitInt>(3)));
    auto generator = AsmGenerator{genAsm()};

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 2);
        matchAsmImm(dest, 3);
    });
}

TEST_F(AsmGeneratorTest, generateJmp) {
    // ARRANGE
    const auto target = "BINARY_LOG_AND.0.L_FALSE";
    addInstr(make_unique<TackyJump>(target));
    auto generator = AsmGenerator{genAsm()};

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    matchAsmJmp(body[0], target);
}

TEST_F(AsmGeneratorTest, generateJumpZero) {
    // ARRANGE
    const auto target = "BINARY_LOG_OR.0.L_TRUE";
    const auto value = 15;
    addInstr(
        make_unique<TackyJumpZero>(make_unique<TackyLitInt>(value), target));
    auto generator = AsmGenerator{genAsm()};

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    matchAsmCmp(body[0], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 15);
    });

    matchAsmJmpCond(body[1], AsmJmpCond::Code::EQUAL, target);
}

TEST_F(AsmGeneratorTest, generateJumpNotZero) {
    // ARRANGE
    const auto target = "BINARY_LOG_AND.1.L_FALSE";
    const auto value = 15;
    addInstr(
        make_unique<TackyJumpNotZero>(make_unique<TackyLitInt>(value), target));
    auto generator = AsmGenerator{genAsm()};

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    matchAsmCmp(body[0], [](auto& left, auto& right) {
        matchAsmImm(left, 0);
        matchAsmImm(right, 15);
    });

    matchAsmJmpCond(body[1], AsmJmpCond::Code::NOT_EQUAL, target);
}

TEST_F(AsmGeneratorTest, generateLabel) {
    // ARRANGE
    const auto label = "BINARY_LOG_OR.0.L_TRUE";
    addInstr(make_unique<TackyLabel>(label));
    auto generator = AsmGenerator{genAsm()};

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    matchAsmLabel(body[0], label);
}
