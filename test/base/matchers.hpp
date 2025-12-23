#pragma once

#include "asm_ast.hpp"
#include "tacky_ast.hpp"
#include "test_utils.hpp"

#include <functional>
#include <gtest/gtest.h>
#include <string_view>

namespace wacc {
namespace test {
namespace match {
namespace {
using namespace back::ast;
using namespace tacky::ast;

using utils::as;

using std::string;
using std::string_view;

// clang-format off
using TackyReturnMatcher = std::function<void(const TackyValPtr&)>;
using TackyUnaryMatcher = std::function<void(const TackyUnaryOpType&, const TackyValPtr& src, const TackyValPtr& dest)>;

using AsmMovMatcher = std::function<void(const AsmOperandPtr& src, const AsmOperandPtr& dest)>;
using AsmUnaryMatcher = std::function<void(const AsmUnaryOpType&, const AsmOperandPtr&)>;
using AsmAllocStackMatcher = std::function<void(const unsigned int&)>;
// clang-format on
} // namespace

// TackyVal matchers
void matchTackyConstant(const TackyValPtr& ptr, int value);
void matchTackyVariable(const TackyValPtr& ptr, string identifier);

// TackyInstr matchers
void matchTackyReturn(const TackyInstrPtr& ptr, TackyReturnMatcher matcher);
void matchTackyUnary(const TackyInstrPtr& ptr, TackyUnaryMatcher matcher);

// AsmOperand matchers
void matchAsmImm(const AsmOperandPtr& ptr, int value);
void matchAsmReg(const AsmOperandPtr& ptr, AsmRegisterType type);
void matchAsmPseudo(const AsmOperandPtr& ptr, const string& value);
void matchAsmStack(const AsmOperandPtr& ptr, signed value);

// AsmInstr matchers
void matchAsmMov(const AsmInstrPtr& ptr, AsmMovMatcher matcher);
void matchAsmUnary(const AsmInstrPtr& ptr, AsmUnaryMatcher matcher);
void matchAsmRet(const AsmInstrPtr& ptr);
void matchAsmAllocStack(const AsmInstrPtr& ptr, AsmAllocStackMatcher matcher);

} // namespace match
} // namespace test
} // namespace wacc