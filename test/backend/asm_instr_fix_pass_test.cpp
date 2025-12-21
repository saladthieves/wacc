#include "asm_ast.hpp"
#include "asm_instr_fix_pass.hpp"
#include "base_test.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>

using namespace wacc::back::ast;
using wacc::back::pass::AsmInstrFixPass;
using wacc::test::utils::as;

using std::make_unique;
using std::string;

class AsmInstrFixPassTest :
    public testing::Test,
    public wacc::test::base::BaseTest {
protected:
    void SetUp() override { instructions = AsmInstrPtrs{}; }

    void addInstr(AsmInstrPtr instr) {
        instructions.push_back(std::move(instr));
    }

    AsmProgPtr getProgram() {
        auto fun = make_unique<AsmFun>("main", std::move(instructions));
        return make_unique<AsmProg>(std::move(fun));
    }

    AsmInstrPtrs instructions;
};

TEST_F(AsmInstrFixPassTest, throwOnNull) {
    // ARRANGE
    auto pass = AsmInstrFixPass{nullptr, 0};
    string error{};

    // ACT
    try {
        pass.run();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("AsmInstrFixPassError"));
    ASSERT_TRUE(error.contains("root is null"));
}

TEST_F(AsmInstrFixPassTest, genAsmAllocStack) {
    // ARRANGE
    addInstr(
        make_unique<AsmMov>(std::make_unique<AsmStack>(-4),
                            std::make_unique<AsmReg>(AsmRegisterType::AX)));
    auto program = getProgram();
    auto pass = AsmInstrFixPass{std::move(program), -4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    auto prog = as<AsmProg>(ast);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 2);
    auto stackAlloc = as<AsmAllocStack>(body.front());
    ASSERT_EQ(stackAlloc->value, -4);
    auto mov = as<AsmMov>(body.back());
    auto movSrc = as<AsmStack>(mov->src);
    auto dest = as<AsmReg>(mov->dest);
}

TEST_F(AsmInstrFixPassTest, fixAsmMov) {
    // ARRANGE
    addInstr(make_unique<AsmMov>(make_unique<AsmStack>(-4),
                                 make_unique<AsmStack>(-4)));
    auto program = getProgram();
    auto pass = AsmInstrFixPass{std::move(program), -4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    auto prog = as<AsmProg>(ast);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 3);

    auto stackAlloc = as<AsmAllocStack>(body.front());
    ASSERT_EQ(stackAlloc->value, -4);

    auto movToR10 = as<AsmMov>(body[1]);
    auto movToR10Src = as<AsmReg>(movToR10->src);
    auto movToR10Dest = as<AsmReg>(movToR10->dest);

    auto movFromR10 = as<AsmMov>(body[2]);
    auto movFromR10Src = as<AsmReg>(movFromR10->src);
    auto movFromR10Dest = as<AsmReg>(movFromR10->dest);
}

TEST_F(AsmInstrFixPassTest, fixAsmMovAll) {
    // ARRANGE
    auto pass = getAsmInstrFixPass();

    // ACT
    auto ast = pass.run();

    // ASSERT
    auto prog = as<AsmProg>(ast);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 8);

    auto asm0 = as<AsmAllocStack>(body.front());
    ASSERT_EQ(asm0->value, -8);

    auto asm1 = as<AsmMov>(body[1]);
    auto asm1Src = as<AsmImm>(asm1->src);
    auto asm1Dest = as<AsmStack>(asm1->dest);

    auto asm2 = as<AsmUnary>(body[2]);
    ASSERT_EQ(asm2->op, AsmUnaryOpType::UNARY_NEGATE);
    auto asm2Op = as<AsmStack>(asm2->operand);

    auto asm3 = as<AsmMov>(body[3]);
    auto asm3Src = as<AsmStack>(asm3->src);
    auto asm3Dest = as<AsmReg>(asm3->dest);

    auto asm4 = as<AsmMov>(body[4]);
    auto asm4Src = as<AsmReg>(asm4->src);
    auto asm4Dest = as<AsmStack>(asm4->dest);

    auto asm5 = as<AsmUnary>(body[5]);
    ASSERT_EQ(asm5->op, AsmUnaryOpType::UNARY_NOT);
    auto asm5Op = as<AsmStack>(asm5->operand);

    auto asm6 = as<AsmMov>(body[6]);
    auto asm6Src = as<AsmStack>(asm6->src);
    auto asm6Dest = as<AsmReg>(asm6->dest);

    auto asm7 = as<AsmRet>(body[7]);
}