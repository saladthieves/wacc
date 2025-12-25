#pragma once

#include "ast.hpp"
#include "tacky_ast.hpp"

#include <string>

namespace wacc {
namespace test {
namespace fmt {
namespace {
using namespace wacc::front::ast;
using namespace wacc::tacky::ast;
} // namespace

std::string formatAstExpr(const AstExprPtr& ptr);

std::string formatTackyInstr(const TackyInstrPtr& ptr);

namespace {
// Ast
std::string formatAstConstInt(const AstConstInt& ast);
std::string formatAstUnary(const AstUnary& ast);
std::string formatAstBinary(const AstBinary& ast);
std::string formatAstUnaryOp(const AstUnaryOpType& type);
std::string formatAstBinaryOp(const AstBinaryOpType& type);
// Tacky
std::string formatTackyReturn(const TackyReturn& ast);
std::string formatTackyUnary(const TackyUnary& ast);
std::string formatTackyBinary(const TackyBinary& ast);
std::string formatTackyVal(const TackyVal& ast);
std::string formatTackyUnaryOp(const TackyUnaryOpType& type);
std::string formatTackyBinaryOp(const TackyBinaryOpType& type);
} // namespace

} // namespace fmt
} // namespace test
} // namespace wacc