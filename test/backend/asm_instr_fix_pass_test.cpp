#include "asm_ast.hpp"
#include "asm_instr_fix_pass.hpp"
#include "base_test.hpp"
#include "matchers.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>

using namespace wacc::back::ast;
using namespace wacc::test::match;

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
    auto pass = AsmInstrFixPass{std::move(program), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    auto prog = as<AsmProg>(ast);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) { ASSERT_EQ(value, 4); });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmRegisterType::AX);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmMov) {
    // ARRANGE
    addInstr(make_unique<AsmMov>(make_unique<AsmStack>(-4),
                                 make_unique<AsmStack>(-4)));
    auto program = getProgram();
    auto pass = AsmInstrFixPass{std::move(program), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    auto prog = as<AsmProg>(ast);
    auto fun = as<AsmFun>(prog->function);
    auto& body = fun->instructions;
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) { ASSERT_EQ(value, 4); });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmRegisterType::R10);
    });

    matchAsmMov(body[2], [](auto& src, auto& dest) {
        matchAsmReg(src, AsmRegisterType::R10);
        matchAsmStack(dest, -4);
    });
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
    ASSERT_EQ(body.size(), 11);

    matchAsmAllocStack(body[0], [](auto& value) { ASSERT_EQ(value, 12); });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmImm(src, 25);
        matchAsmStack(dest, -4);
    });

    matchAsmUnary(body[2], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NOT);
        matchAsmStack(operand, -4);
    });

    matchAsmMov(body[3], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmRegisterType::R10);
    });

    matchAsmMov(body[4], [](auto& src, auto& dest) {
        matchAsmReg(src, AsmRegisterType::R10);
        matchAsmStack(dest, -8);
    });

    matchAsmUnary(body[5], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmStack(operand, -8);
    });

    matchAsmMov(body[6], [](auto& src, auto& dest) {
        matchAsmStack(src, -8);
        matchAsmReg(dest, AsmRegisterType::R10);
    });

    matchAsmMov(body[7], [](auto& src, auto& dest) {
        matchAsmReg(src, AsmRegisterType::R10);
        matchAsmStack(dest, -12);
    });

    matchAsmUnary(body[8], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NOT);
        matchAsmStack(operand, -12);
    });

    matchAsmMov(body[9], [](auto& src, auto& dest) {
        matchAsmStack(src, -12);
        matchAsmReg(dest, AsmRegisterType::AX);
    });

    matchAsmRet(body[10]);
}