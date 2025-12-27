#include "matchers.hpp"
#include "asm_ast.hpp"
#include "ast.hpp"
#include "tacky_ast.hpp"

#include <gtest/gtest.h>

namespace wacc::test::match {

// AST
const AstStmtPtr& matchAstProg(const AstNodePtr& ptr) {
    auto program = as<AstProg>(ptr);
    auto function = as<AstFun>(program->function);
    return function->body;
}

// AstExpr matchers
void matchAstConstInt(const AstExprPtr& ptr, int value) {
    auto ast = as<AstConstInt>(ptr);
    ASSERT_EQ(ast->value, value);
}

void matchAstUnary(const AstExprPtr& ptr, AstUnaryMatcher matcher) {
    auto unary = as<AstUnary>(ptr);
    matcher(unary->op, unary->expr);
}

void matchAstBinary(const AstExprPtr& ptr, AstBinaryMatcher matcher) {
    auto binary = as<AstBinary>(ptr);
    matcher(binary->op, binary->left, binary->right);
}

// AstNode matchers
void matchAstIdent(const AstNodePtr& ptr, std::string_view value) {
    auto ident = as<AstIdent>(ptr);
    ASSERT_TRUE(ident->value == value);
}

// AstStmt matchers
void matchAstReturn(const AstStmtPtr& ptr, AstReturnMatcher matcher) {
    auto ast = as<AstReturn>(ptr);
    matcher(ast->expr);
}

// TACKY
const TackyInstrPtrs& matchTackyProg(const TackyNodePtr& ptr) {
    auto program = as<TackyProg>(ptr);
    auto function = as<TackyFun>(program->function);
    return function->body;
}

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

void matchTackyBinary(const TackyInstrPtr& ptr, TackyBinaryMatcher matcher) {
    auto binary = as<TackyBinary>(ptr);
    matcher(binary->op, binary->src1, binary->src2, binary->dest);
}

// ASM
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
} // namespace wacc::test::match