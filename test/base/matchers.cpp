#include "matchers.hpp"
#include "asm_ast.hpp"
#include <gtest/gtest.h>

namespace wacc::test::match {
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
// AsmMov
void matchAsmMov(const AsmInstrPtr& ptr, MovMatcher matcher) {
    auto mov = as<AsmMov>(ptr);
    matcher(mov->src, mov->dest);
}

// AsmUnary
void matchAsmUnary(const AsmInstrPtr& ptr, UnaryMatcher matcher) {
    auto unary = as<AsmUnary>(ptr);
    matcher(unary->op, unary->operand);
}

// AsmRet
void matchAsmRet(const AsmInstrPtr& ptr) {
    as<AsmRet>(ptr);
}

// AsmAllocStack
void matchAsmAllocStack(const AsmInstrPtr& ptr, AllocStackMatcher matcher) {
    auto alloc = as<AsmAllocStack>(ptr);
    matcher(alloc->value);
}
} // namespace wacc::test::match