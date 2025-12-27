#include "asm_ast.hpp"
#include "asm_pseudo_pass.hpp"
#include "matchers.hpp"

#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace wacc::back::ast;
using namespace wacc::test::match;

using wacc::back::pass::AsmPseudoPass;

using std::make_unique;
using std::string;

class AsmPseudoPassTest : public testing::Test {
protected:
    void SetUp() override { instructions = AsmInstrPtrs{}; }

    void addInstr(AsmInstrPtr instr) {
        instructions.push_back(std::move(instr));
    }

    AsmProgPtr getProgram() {
        auto fun = make_unique<AsmFun>("main", std::move(instructions));
        return make_unique<AsmProg>(std::move(fun));
    }

    AsmInstrPtrs instructions{};
};

TEST_F(AsmPseudoPassTest, throwOnNull) {
    // ARRANGE
    auto pass = AsmPseudoPass{nullptr};
    string error{};

    // ACT
    try {
        pass.run();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("AsmNodePtr root"));
}

TEST_F(AsmPseudoPassTest, runAsmMovPass) {
    // ARRANGE
    const auto identifier = "MAIN.TEMP.0";
    auto src = make_unique<AsmPseudo>(identifier);
    auto dest = make_unique<AsmPseudo>(identifier);
    addInstr(make_unique<AsmMov>(std::move(src), std::move(dest)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(node);
    ASSERT_EQ(body.size(), 1);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmStack(dest, -4);
    });
}

TEST_F(AsmPseudoPassTest, runAsmUnaryPassSingleIdent) {
    // ARRANGE
    const auto identifier = "MAIN.TEMP.0";
    auto operand = make_unique<AsmPseudo>(identifier);
    addInstr(make_unique<AsmUnary>(AsmUnaryOpType::UNARY_NEGATE,
                                   std::move(operand)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(node);
    ASSERT_EQ(body.size(), 1);

    matchAsmUnary(body[0], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmStack(operand, -4);
    });
}

TEST_F(AsmPseudoPassTest, runAsmUnaryPassMultiIdent) {
    // ARRANGE
    const auto identifier1 = "MAIN.TEMP.0";
    const auto identifier2 = "TMAIN.EMP.1";
    auto operand1 = make_unique<AsmPseudo>(identifier1);
    addInstr(make_unique<AsmUnary>(AsmUnaryOpType::UNARY_NEGATE,
                                   std::move(operand1)));
    auto operand2 = make_unique<AsmPseudo>(identifier2);
    addInstr(make_unique<AsmUnary>(AsmUnaryOpType::UNARY_NEGATE,
                                   std::move(operand2)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(node);
    ASSERT_EQ(body.size(), 2);

    matchAsmUnary(body[0], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmStack(operand, -4);
    });

    matchAsmUnary(body[1], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmStack(operand, -8);
    });
}

TEST_F(AsmPseudoPassTest, runAsmBinaryPassSingleIdent) {
    // ARRANGE
    const auto identifier = "MAIN.TEMP.0";
    auto src = make_unique<AsmPseudo>(identifier);
    auto dest = make_unique<AsmPseudo>(identifier);
    addInstr(make_unique<AsmBinary>(AsmBinaryOpType::BINARY_MULT,
                                    std::move(src), std::move(dest)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(node);

    matchAsmBinary(body[0], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, AsmBinaryOpType::BINARY_MULT);
        matchAsmStack(src, -4);
        matchAsmStack(dest, -4);
    });
}

TEST_F(AsmPseudoPassTest, runAsmBinaryPassMultiIdent) {
    // ARRANGE
    auto src = make_unique<AsmPseudo>("MAIN.TEMP.0");
    auto dest = make_unique<AsmPseudo>("MAIN.TEMP.1");
    addInstr(make_unique<AsmBinary>(AsmBinaryOpType::BINARY_ADD,
                                    std::move(src), std::move(dest)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(node);

    matchAsmBinary(body[0], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, AsmBinaryOpType::BINARY_ADD);
        matchAsmStack(src, -4);
        matchAsmStack(dest, -8);
    });
}

TEST_F(AsmPseudoPassTest, runAsmIdivPassSingleIdent) {
    // ARRANGE
    auto operand = make_unique<AsmPseudo>("MAIN.TEMP.0");
    addInstr(make_unique<AsmIdiv>(std::move(operand)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(node);

    matchAsmIdiv(body[0], [](auto& operand) { matchAsmStack(operand, -4); });
}

TEST_F(AsmPseudoPassTest, runAsmIdivPassMultiIdent) {
    // ARRANGE
    auto operand1 = make_unique<AsmPseudo>("MAIN.TEMP.0");
    addInstr(make_unique<AsmIdiv>(std::move(operand1)));

    auto operand2 = make_unique<AsmPseudo>("MAIN.TEMP.0");
    addInstr(make_unique<AsmIdiv>(std::move(operand2)));

    auto operand3 = make_unique<AsmPseudo>("MAIN.TEMP.1");
    addInstr(make_unique<AsmIdiv>(std::move(operand3)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(node);

    matchAsmIdiv(body[0], [](auto& operand) {
        matchAsmStack(operand, -4); //
    });

    matchAsmIdiv(body[1], [](auto& operand) {
        matchAsmStack(operand, -4); //
    });

    matchAsmIdiv(body[2], [](auto& operand) {
        matchAsmStack(operand, -8); //
    });
}
