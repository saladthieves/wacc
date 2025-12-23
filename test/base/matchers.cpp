#include "matchers.hpp"
#include "asm_ast.hpp"
#include "tacky_ast.hpp"
#include <gtest/gtest.h>

namespace wacc::test::match {
// TackyVal matchers
void matchTackyConstant(const TackyValPtr& ptr, int value) {
    auto constant = as<TackyConstant>(ptr);
    ASSERT_EQ(constant->value, value);
}

void matchTackyVariable(const TackyValPtr& ptr, string identifier) {
    auto var = as<TackyVariable>(ptr);
    ASSERT_TRUE(var->identifier.ends_with(identifier));
}

// TackyInstr matchers
void matchTackyReturn(const TackyInstrPtr& ptr, TackyReturnMatcher matcher) {
    auto tackyReturn = as<TackyReturn>(ptr);
    matcher(tackyReturn->val);
}

void matchTackyUnary(const TackyInstrPtr& ptr, TackyUnaryMatcher matcher) {
    auto unary = as<TackyUnary>(ptr);
    matcher(unary->op, unary->src, unary->dest);
}

// AsmOperand matchers
void matchAsmImm(const AsmOperandPtr& ptr, int value) {
    auto imm = as<AsmImm>(ptr);
    ASSERT_EQ(imm->value, value);
}

void matchAsmReg(const AsmOperandPtr& ptr, AsmRegisterType type) {
    auto reg = as<AsmReg>(ptr);
    ASSERT_EQ(reg->reg, type);
}

void matchAsmPseudo(const AsmOperandPtr& ptr, const string& value) {
    auto pseudo = as<AsmPseudo>(ptr);
    ASSERT_TRUE(pseudo->identifier.ends_with(value));
}

void matchAsmStack(const AsmOperandPtr& ptr, signed value) {
    auto stack = as<AsmStack>(ptr);
    ASSERT_EQ(stack->value, value);
}

// AsmInstr matchers
void matchAsmMov(const AsmInstrPtr& ptr, AsmMovMatcher matcher) {
    auto mov = as<AsmMov>(ptr);
    matcher(mov->src, mov->dest);
}

void matchAsmUnary(const AsmInstrPtr& ptr, AsmUnaryMatcher matcher) {
    auto unary = as<AsmUnary>(ptr);
    matcher(unary->op, unary->operand);
}

void matchAsmRet(const AsmInstrPtr& ptr) {
    as<AsmRet>(ptr);
}

void matchAsmAllocStack(const AsmInstrPtr& ptr, AsmAllocStackMatcher matcher) {
    auto alloc = as<AsmAllocStack>(ptr);
    matcher(alloc->value);
}
} // namespace wacc::test::match