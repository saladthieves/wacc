#include "asm_matchers.hpp"

#include <gtest/gtest.h>

namespace wacc::test::match {
    const AsmInstrPtrs& matchAsmProg(const AsmNodePtr& ptr) {
    auto program = as<AsmProg>(ptr);
    auto function = as<AsmFun>(program->function);
    return function->instructions;
}

// AsmOperand matchers
void matchAsmImm(const AsmOperandPtr& ptr, int value) {
    auto imm = as<AsmImm>(ptr);
    ASSERT_EQ(imm->value, value);
}

void matchAsmReg(const AsmOperandPtr& ptr, AsmReg::Type type, AsmReg::Size size) {
    auto reg = as<AsmReg>(ptr);
    ASSERT_EQ(reg->reg, type);
    ASSERT_EQ(reg->size, size);
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

void matchAsmBinary(const AsmInstrPtr& ptr, AsmBinaryMatcher matcher) {
    auto binary = as<AsmBinary>(ptr);
    matcher(binary->op, binary->src, binary->dest);
}

void matchAsmCdq(const AsmInstrPtr& ptr) {
    as<AsmCdq>(ptr);
}

void matchAsmIdiv(const AsmInstrPtr& ptr, AsmIdivMatcher matcher) {
    auto idiv = as<AsmIdiv>(ptr);
    matcher(idiv->operand);
}

void matchAsmRet(const AsmInstrPtr& ptr) {
    as<AsmRet>(ptr);
}

void matchAsmAllocStack(const AsmInstrPtr& ptr, AsmAllocStackMatcher matcher) {
    auto alloc = as<AsmAllocStack>(ptr);
    matcher(alloc->value);
}
}