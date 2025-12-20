#include "asm_ast.hpp"
#include "asm_pseudo_pass.hpp"
#include "test_utils.hpp"

#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace wacc::back::ast;
using wacc::back::pass::AsmPseudoPass;
using wacc::test::utils::as;

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
    const auto identifier = "TEMP.0";
    auto src = make_unique<AsmPseudo>(identifier);
    auto dest = make_unique<AsmPseudo>(identifier);
    addInstr(make_unique<AsmMov>(std::move(src), std::move(dest)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    auto prog = as<AsmProg>(node);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 1);
    auto mov = as<AsmMov>(body.front());
    auto movSrc = as<AsmStack>(mov->src);
    ASSERT_EQ(movSrc->value, -4);
    auto movDest = as<AsmStack>(mov->dest);
    ASSERT_EQ(movDest->value, -4);
}

TEST_F(AsmPseudoPassTest, runAsmUnaryPass) {
    // ARRANGE
    const auto identifier = "TEMP.0";
    auto operand = make_unique<AsmPseudo>(identifier);
    addInstr(make_unique<AsmUnary>(AsmUnaryOpType::UNARY_NEGATE,
                                   std::move(operand)));

    auto pass = AsmPseudoPass{getProgram()};

    // ACT
    auto node = pass.run();

    // ASSERT
    auto prog = as<AsmProg>(node);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 1);
    auto unary = as<AsmUnary>(body.front());
    auto unaryOp = as<AsmStack>(unary->operand);
    ASSERT_EQ(unaryOp->value, -4);
    ASSERT_EQ(unary->op, AsmUnaryOpType::UNARY_NEGATE);
}

TEST_F(AsmPseudoPassTest, runAsmUnaryPassMultiple) {
    // ARRANGE
    const auto identifier1 = "TEMP.0";
    const auto identifier2 = "TEMP.1";
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
    auto prog = as<AsmProg>(node);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 2);
    
    auto unary1 = as<AsmUnary>(body.front());
    auto unary1Op = as<AsmStack>(unary1->operand);
    ASSERT_EQ(unary1Op->value, -4);
    ASSERT_EQ(unary1->op, AsmUnaryOpType::UNARY_NEGATE);

    auto unary2 = as<AsmUnary>(body.back());
    auto unary2Op = as<AsmStack>(unary2->operand);
    ASSERT_EQ(unary2Op->value, -8);
    ASSERT_EQ(unary2->op, AsmUnaryOpType::UNARY_NEGATE);
}