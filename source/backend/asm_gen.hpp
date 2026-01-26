#pragma once

#include "asm_ast.hpp"
#include "tacky_ast.hpp"

#include <format>
#include <stdexcept>

namespace wacc {
namespace back {
namespace gen {
namespace {
using namespace wacc::tacky::ast;
using namespace wacc::back::ast;

using Body = AsmInstrPtrs;
} // namespace

class AsmGenerator {
public:
    explicit AsmGenerator(TackyNodePtr ptr);

    AsmNodePtr generate() const;

private:
    AsmProgPtr genForTackyProg(const TackyProg& obj) const;

    AsmFunPtr genForTackyFun(const TackyFun& obj) const;

    Body genForTackyInstrs(const TackyInstrPtrs& tackyBody) const;

    void genForTackyReturn(const TackyReturn& tacky, Body& body) const;

    void genForTackyUnary(const TackyUnary& tacky, Body& body) const;

    void genForTackyBinary(const TackyBinary& tacky, Body& body) const;

    void genForTackyBinaryDivRem(const TackyBinary& tacky, Body& body) const;

    void genForTackyBinaryRelational(const TackyBinary& tacky,
                                     Body& body) const;

    void genForTackyBinaryOther(const TackyBinary& tacky, Body& body) const;

    void genForTackyCopy(const TackyCopy& tacky, Body& body) const;

    void genForTackyJump(const TackyJump& tacky, Body& body) const;

    void genForTackyJumpZero(const TackyValPtr& condition,
                             std::string_view target, bool neg, Body& body) const;

    void genForTackyLabel(const TackyLabel& tacky, Body& body) const;

    AsmRegPtr genAsmReg(AsmReg::Type type) const;

    AsmOperandPtr genForTackyVal(const TackyVal& tacky) const;

    static AsmUnary::Type genForTackyUnaryOp(const TackyUnary::Type& type);

    static AsmBinary::Type genForTackyBinaryOp(const TackyBinary::Type& type);

    static CondCode
    genForTackyBinaryRelationalOp(const TackyBinary::Type& type);

    template <typename... T>
    [[noreturn]] static void fail(std::format_string<T...> str = "",
                                  T&&... args) {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(std::format("AsmGeneratorError: {}", message));
    }

    TackyNodePtr ast;
};
} // namespace gen
} // namespace back
} // namespace wacc