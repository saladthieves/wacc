#pragma once

#include "asm_ast.hpp"

#include <map>
#include <string>

namespace wacc {
namespace back {
namespace pass {
namespace {
using namespace wacc::back::ast;
using Key = std::string;
using Value = signed int;
using StackMap = std::map<Key, Value>;
} // namespace

class AsmPseudoPass {
public:
    explicit AsmPseudoPass(AsmNodePtr ptr);

    AsmNodePtr run();

    unsigned int getAbsoluteOffset() const;

private:
    void runPass(AsmInstrPtrs& instructions);

    void replace(AsmOperandPtr& ptr);

    signed int getAdjustedOffset();

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(
            std::format("AsmPseudoPassError: {}", message));
    }

    AsmNodePtr ast{nullptr};
    StackMap stacks{};
    signed int offset{0};
};
} // namespace pass
} // namespace back
} // namespace wacc