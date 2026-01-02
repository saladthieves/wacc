#pragma once

#include "ast.hpp"
#include "test_utils.hpp"

#include <functional>
#include <string>
#include <string_view>

namespace wacc {
namespace test {
namespace match {
namespace {
using namespace front::ast;

using std::string;
using std::string_view;
using utils::as;
} // namespace

// clang-format off
using AstUnaryMatcher = std::function<void(const AstUnary::Type& op, const AstExprPtr& expr)>;
using AstBinaryMatcher = std::function<void(const AstBinary::Type& op, const AstExprPtr& left, const AstExprPtr& right)>;
using AstReturnMatcher = std::function<void(const AstExprPtr& expr)>;
// clang format-on

const AstStmtPtr& matchAstProg(const AstNodePtr& ptr);
// AstExpr matchers
void matchAstLitInt(const AstExprPtr& ptr, int value);
void matchAstUnary(const AstExprPtr& ptr, AstUnaryMatcher matcher);
void matchAstBinary(const AstExprPtr& ptr, AstBinaryMatcher matcher);
// AstNode matchers
void matchAstIdent(const AstNodePtr& ptr, std::string_view value);
// AstStmt matchers
void matchAstReturn(const AstStmtPtr& ptr, AstReturnMatcher matcher);
}
} // namespace test
} // namespace wacc