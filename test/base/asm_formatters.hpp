#pragma once

#include "asm_ast.hpp"

namespace wacc {
namespace test {
namespace fmt {
namespace {
using namespace wacc::back::ast;
}

std::string formatAsmInstr(const AsmInstrPtr& ptr);

namespace {
std::string formatAsmOperand(const AsmOperand& op);
std::string formatAsmRegister(const AsmReg::Type& type);
std::string formatAsmMov(const AsmMov& mov);
std::string formatAsmUnary(const AsmUnary& unary);
std::string formatAsmBinary(const AsmBinary& binary);
std::string formatAsmIdiv(const AsmIdiv& idiv);
std::string formatAsmCdq();
std::string formatAsmAllocStack(const AsmAllocStack& stack);
std::string formatAsmRet();
std::string formatAsmUnaryOp(const AsmUnary::Type& type);
std::string formatAsmBinaryOp(const AsmBinary::Type& type);
} // namespace
} // namespace fmt
} // namespace test
} // namespace wacc