#pragma once

#include "asm_ast.hpp"
#include "test_utils.hpp"

#include <functional>
#include <string>
#include <string_view>

namespace wacc {
namespace test {
namespace match {
namespace {
using namespace back::ast;

using std::string;
using std::string_view;
using utils::as;

// clang-format off
using AsmMovMatcher = std::function<void(const AsmOperandPtr& src, const AsmOperandPtr& dest)>;
using AsmUnaryMatcher = std::function<void(const AsmUnary::Type&, const AsmOperandPtr&)>;
using AsmBinaryMatcher = std::function<void(const AsmBinary::Type&, const AsmOperandPtr&, const AsmOperandPtr&)>;
using AsmIdivMatcher = std::function<void(const AsmOperandPtr& operand)>;
using AsmAllocStackMatcher = std::function<void(const unsigned int&)>;
// clang-format on
} // namespace

const AsmInstrPtrs& matchAsmProg(const AsmNodePtr& ptr);
// AsmOperand matchers
void matchAsmImm(const AsmOperandPtr& ptr, int value);
void matchAsmReg(const AsmOperandPtr& ptr, AsmReg::Type type, AsmReg::Size size = AsmReg::Size::DOUBLE_WORD);
void matchAsmPseudo(const AsmOperandPtr& ptr, const string& value);
void matchAsmStack(const AsmOperandPtr& ptr, signed value);
// AsmInstr matchers
void matchAsmMov(const AsmInstrPtr& ptr, AsmMovMatcher matcher);
void matchAsmUnary(const AsmInstrPtr& ptr, AsmUnaryMatcher matcher);
void matchAsmBinary(const AsmInstrPtr& ptr, AsmBinaryMatcher matcher);
void matchAsmCdq(const AsmInstrPtr& ptr);
void matchAsmIdiv(const AsmInstrPtr& ptr, AsmIdivMatcher matcher);
void matchAsmRet(const AsmInstrPtr& ptr);
void matchAsmAllocStack(const AsmInstrPtr& ptr, AsmAllocStackMatcher matcher);

} // namespace match
} // namespace test
} // namespace wacc