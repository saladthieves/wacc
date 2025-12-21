#pragma once

#include "asm_ast.hpp"
#include "test_utils.hpp"

#include <functional>
#include <gtest/gtest.h>
#include <string_view>

namespace wacc {
namespace test {
namespace match {
namespace {
using namespace back::ast;
using utils::as;

using std::string;
using std::string_view;

using Operand = const AsmOperandPtr&;
using MovMatcher = std::function<void(Operand src, Operand dest)>;
using UnaryMatcher = std::function<void(const AsmUnaryOpType&, Operand)>;
using AllocStackMatcher = std::function<void(const unsigned int&)>;
} // namespace

// AsmOperand matchers
void matchAsmImm(const AsmOperandPtr& ptr, int value);
void matchAsmReg(const AsmOperandPtr& ptr, AsmRegisterType type);
void matchAsmPseudo(const AsmOperandPtr& ptr, const string& value);
void matchAsmStack(const AsmOperandPtr& ptr, signed value);

// AsmInstr matchers
// AsmMov
void matchAsmMov(const AsmInstrPtr& ptr, MovMatcher matcher);
// AsmUnary
void matchAsmUnary(const AsmInstrPtr& ptr, UnaryMatcher matcher);
// AsmRet
void matchAsmRet(const AsmInstrPtr& ptr);
// AsmAllocStack
void matchAsmAllocStack(const AsmInstrPtr& ptr, AllocStackMatcher matcher);

} // namespace match
} // namespace test
} // namespace wacc