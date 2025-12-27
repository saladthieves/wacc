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
} // namespace

class AsmGenerator {
public:
    AsmGenerator(TackyNodePtr ptr);

    AsmNodePtr generate() const;

private:
    AsmProgPtr genForTackyProg(const TackyProg& obj) const;

    AsmFunPtr genForTackyFun(const TackyFun& obj) const;

    AsmInstrPtrs genForTackyInstrs(const TackyInstrPtrs& tackyBody) const;

    void genForTackyReturn(const TackyReturn& tacky,
                           AsmInstrPtrs& asmBody) const;

    void genForTackyUnary(const TackyUnary& tacky, AsmInstrPtrs& asmBody) const;

    void genForTackyBinary(const TackyBinary& tacky,
                           AsmInstrPtrs& asmBody) const;

    void genForTackyDivRem(const TackyBinary& tacky,
                           AsmInstrPtrs& asmBody) const;

    AsmRegPtr genAsmReg(AsmRegisterType type) const;

    AsmOperandPtr genForTackyVal(const TackyVal& tacky) const;

    AsmUnaryOpType genForTackyUnaryOp(const TackyUnaryOpType& type) const;

    AsmBinaryOpType genForTackyBinaryOp(const TackyBinaryOpType& type) const;

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(std::format("AsmGeneratorError: {}", message));
    }

    TackyNodePtr ast;
};
} // namespace gen
} // namespace back
} // namespace wacc