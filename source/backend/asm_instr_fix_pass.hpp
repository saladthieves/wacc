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

    void genAsmAllocStack();

    void fixAsmMov(AsmInstrPtr ptr);

    void fixAsmIdiv(AsmInstrPtr ptr);

    void fixAsmBinary(AsmInstrPtr ptr);

    void fixAsmBinaryMult(AsmInstrPtr ptr);

    void fixAsmBinaryShift(AsmInstrPtr ptr);

    void fixAsmBinaryAndXorOr(AsmInstrPtr ptr);

    template <typename... T>
    [[noreturn]] static void fail(std::format_string<T...> str = "",
                                  T&&... args) {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(
            std::format("AsmInstrFixPassError: {}", message));
    }

    AsmNodePtr ast{nullptr};
    AsmInstrPtrs fixed{};
    unsigned int stackOffset{0};
};
} // namespace pass
} // namespace back
} // namespace wacc