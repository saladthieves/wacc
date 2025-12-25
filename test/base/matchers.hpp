#pragma once

#include "asm_ast.hpp"
#include "ast.hpp"
#include "tacky_ast.hpp"
#include "test_utils.hpp"

#include <functional>
#include <gtest/gtest.h>
#include <string_view>

namespace wacc {
namespace test {
namespace match {
namespace {
using namespace front::ast;
using namespace back::ast;
using namespace tacky::ast;

using utils::as;

using std::string;
using std::string_view;

// clang-format off
using AstUnaryMatcher = std::function<void(const AstUnaryOpType& op, const AstExprPtr& expr)>;
using AstBinaryMatcher = std::function<void(const AstBinaryOpType& op, const AstExprPtr& left, const AstExprPtr& right)>;
using AstReturnMatcher = std::function<void(const AstExprPtr& expr)>;

using TackyReturnMatcher = std::function<void(const TackyValPtr&)>;
using TackyUnaryMatcher = std::function<void(const TackyUnaryOpType&, const TackyValPtr& src, const TackyValPtr& dest)>;

using AsmMovMatcher = std::function<void(const AsmOperandPtr& src, const AsmOperandPtr& dest)>;
using AsmUnaryMatcher = std::function<void(const AsmUnaryOpType&, const AsmOperandPtr&)>;
using AsmAllocStackMatcher = std::function<void(const unsigned int&)>;
// clang-format on
} // namespace

// AST
const AstStmtPtr& matchAstProg(const AstNodePtr& ptr);
// AstExpr matchers
void matchAstConstInt(const AstExprPtr& ptr, int value);
void matchAstUnary(const AstExprPtr& ptr, AstUnaryMatcher matcher);
void matchAstBinary(const AstExprPtr& ptr, AstBinaryMatcher matcher);
// AstNode matchers
void matchAstIdent(const AstNodePtr& ptr, std::string_view value);
// AstStmt matchers
void matchAstReturn(const AstStmtPtr& ptr, AstReturnMatcher matcher);

// TACKY
const TackyInstrPtrs& matchTackyProg(const TackyNodePtr& ptr);
// TackyVal matchers
void matchTackyConstant(const TackyValPtr& ptr, int value);
void matchTackyVariable(const TackyValPtr& ptr, string identifier);
// TackyInstr matchers
void matchTackyReturn(const TackyInstrPtr& ptr, TackyReturnMatcher matcher);
void matchTackyUnary(const TackyInstrPtr& ptr, TackyUnaryMatcher matcher);

// ASM
const AsmInstrPtrs& matchAsmProg(const AsmNodePtr& ptr);
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