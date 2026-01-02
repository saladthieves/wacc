#include "ast_matchers.hpp"

#include <gtest/gtest.h>

namespace wacc::test::match {
const AstStmtPtr& matchAstProg(const AstNodePtr& ptr) {
    auto program = as<AstProg>(ptr);
    auto function = as<AstFun>(program->function);
    return function->body;
}

// AstExpr matchers
void matchAstLitInt(const AstExprPtr& ptr, int value) {
    auto ast = as<AstLitInt>(ptr);
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
} // namespace wcc::test::match