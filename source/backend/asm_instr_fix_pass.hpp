#pragma once

#include "asm_ast.hpp"

namespace wacc {
namespace back {
namespace pass {
namespace {
using namespace wacc::back::ast;
}

class AsmInstrFixPass {
    using StackPos = AsmInstrPtrs::iterator;

public:
    AsmInstrFixPass(AsmNodePtr ptr, unsigned int stackOffset);

    AsmNodePtr run();

private:
    void runPass(AsmInstrPtrs& instructions);

    void genAsmAllocStack(AsmInstrPtrs& instructions);

    void fixAsmMov(StackPos pos, AsmInstrPtrs& instructions);

    void fixAsmIdiv(StackPos pos, AsmInstrPtrs& instructions);

    void fixAsmBinary(StackPos pos, AsmInstrPtrs& instructions);

    void fixAsmBinaryMult(StackPos pos, AsmBinary& binary, AsmInstrPtrs& instructions); 

    bool isFixable(const AsmInstrPtr& ptr) const;

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(
            std::format("AsmInstrFixPassError: {}", message));
    }

    AsmNodePtr ast{nullptr};
    unsigned int stackOffset{0};
};
} // namespace pass
} // namespace back
} // namespace wacc