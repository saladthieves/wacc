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
std::string formatTackyVal(const TackyVal& ast);
std::string formatTackyUnaryOp(const TackyUnaryOpType& type);
std::string formatTackyBinaryOp(const TackyBinaryOpType& type);
} // namespace
} // namespace fmt
} // namespace test
} // namespace wacc