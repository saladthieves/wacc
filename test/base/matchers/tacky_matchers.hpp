#pragma once

#include "tacky_ast.hpp"
#include "test_utils.hpp"

#include <functional>
#include <string>
#include <string_view>

namespace wacc {
namespace test {
namespace match {
namespace {
using namespace tacky::ast;

using std::string;
using std::string_view;
using utils::as;

// clang-format off
using TackyReturnMatcher = std::function<void(const TackyValPtr&)>;
using TackyUnaryMatcher = std::function<void(const TackyUnary::Type&, const TackyValPtr& src, const TackyValPtr& dest)>;
using TackyBinaryMatcher = std::function<void(const TackyBinary::Type&, const TackyValPtr& src1, const TackyValPtr& src2, const TackyValPtr& dest)>;
// clang-format on
} // namespace

// TACKY
const TackyInstrPtrs& matchTackyProg(const TackyNodePtr& ptr);
// TackyVal matchers
void matchTackyLitInt(const TackyValPtr& ptr, int value);
void matchTackyVariable(const TackyValPtr& ptr, string identifier);
// TackyInstr matchers
void matchTackyReturn(const TackyInstrPtr& ptr, TackyReturnMatcher matcher);
void matchTackyUnary(const TackyInstrPtr& ptr, TackyUnaryMatcher matcher);
void matchTackyBinary(const TackyInstrPtr& ptr, TackyBinaryMatcher matcher);
} // namespace match
} // namespace test
} // namespace wacc