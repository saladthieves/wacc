#pragma once

#include "ast.hpp"

#include <string>

namespace wacc {
namespace test {
namespace fmt {
namespace {
using namespace wacc::front::ast;
}

std::string formatAstExpr(const AstExprPtr& ptr);

namespace {
std::string formatAstConstInt(const AstConstInt& ast);

std::string formatAstUnary(const AstUnary& ast);

std::string formatAstBinary(const AstBinary& ast);

std::string formatAstUnaryOp(const AstUnaryOpType& type);

std::string formatAstBinaryOp(const AstBinaryOpType& type);
} // namespace

} // namespace fmt
} // namespace test
} // namespace wacc