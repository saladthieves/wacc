#include "tacky_matchers.hpp"

#include <gtest/gtest.h>

namespace wacc::test::match {
// TACKY
const TackyInstrPtrs& matchTackyProg(const TackyNodePtr& ptr) {
    auto program = as<TackyProg>(ptr);
    auto function = as<TackyFun>(program->function);
    return function->body;
}

// TackyVal matchers
void matchTackyConstant(const TackyValPtr& ptr, int value) {
    auto constant = as<TackyLitInt>(ptr);
    ASSERT_EQ(constant->value, value);
}

void matchTackyVariable(const TackyValPtr& ptr, string identifier) {
    auto var = as<TackyVariable>(ptr);
    ASSERT_TRUE(var->identifier.ends_with(identifier));
}

// TackyInstr matchers
void matchTackyReturn(const TackyInstrPtr& ptr, TackyReturnMatcher matcher) {
    auto tackyReturn = as<TackyReturn>(ptr);
    matcher(tackyReturn->val);
}

void matchTackyUnary(const TackyInstrPtr& ptr, TackyUnaryMatcher matcher) {
    auto unary = as<TackyUnary>(ptr);
    matcher(unary->op, unary->src, unary->dest);
}

void matchTackyBinary(const TackyInstrPtr& ptr, TackyBinaryMatcher matcher) {
    auto binary = as<TackyBinary>(ptr);
    matcher(binary->op, binary->src1, binary->src2, binary->dest);
}
} // namespace wacc::test::match