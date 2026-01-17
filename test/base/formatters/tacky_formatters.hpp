#pragma once

#include "tacky_ast.hpp"

#include <string>

namespace wacc {
namespace test {
namespace fmt {
namespace {
using namespace wacc::tacky::ast;
}

std::string formatTackyInstr(const TackyInstrPtr& ptr);

namespace {
std::string formatTackyReturn(const TackyReturn& ast);
std::string formatTackyUnary(const TackyUnary& ast);
std::string formatTackyBinary(const TackyBinary& ast);
std::string formatTackyCopy(const TackyCopy& ast);
std::string formatTackyJump(const TackyJump& ast);
std::string formatTackyJumpZero(const TackyJumpZero& ast);
std::string formatTackyJumpNotZero(const TackyJumpNotZero& ast);
std::string formatTackyLabel(const TackyLabel& ast);
std::string formatTackyVal(const TackyVal& ast);
std::string formatTackyUnaryOp(const TackyUnary::Type& type);
std::string formatTackyBinaryOp(const TackyBinary::Type& type);
} // namespace
} // namespace fmt
} // namespace test
} // namespace wacc