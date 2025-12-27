#pragma once

#include "asm_ast.hpp"
#include "ast.hpp"
#include "tacky_ast.hpp"

#include <string>

namespace wacc {
namespace test {
namespace fmt {
namespace {
using namespace wacc::front::ast;
using namespace wacc::tacky::ast;
using namespace wacc::back::ast;
} // namespace
// TODO: Split into three different files

std::string formatAstExpr(const AstExprPtr& ptr);

std::string formatTackyInstr(const TackyInstrPtr& ptr);

std::string formatAsmInstr(const AsmInstrPtr& ptr);

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
// Asm
std::string formatAsmOperand(const AsmOperand& op);
std::string formatAsmRegister(const AsmRegisterType& type);
std::string formatAsmMov(const AsmMov& mov);
std::string formatAsmUnary(const AsmUnary& unary);
std::string formatAsmBinary(const AsmBinary& binary);
std::string formatAsmIdiv(const AsmIdiv& idiv);
std::string formatAsmCdq();
std::string formatAsmAllocStack(const AsmAllocStack& stack);
std::string formatAsmRet();
std::string formatAsmUnaryOp(const AsmUnaryOpType& type);
std::string formatAsmBinaryOp(const AsmBinaryOpType& type);
} // namespace

} // namespace fmt
} // namespace test
} // namespace wacc