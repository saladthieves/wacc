#pragma once

#include "asm_ast.hpp"
#include "utils.hpp"

#include <memory>
#include <string>
#include <vector>

namespace wacc {
namespace back {
namespace emit {
namespace {
using namespace wacc::back::ast;

using Line = std::string;
using Lines = std::vector<Line>;
using LinesPtr = std::unique_ptr<Lines>;

static constexpr auto INDENT = "    ";
} // namespace

class AsmEmitter {
public:
    AsmEmitter(AsmNodePtr ptr, utils::Platform platform);

    LinesPtr emit();

private:
    void emitAsmProg(const AsmProg& obj);

    void emitAsmFun(const AsmFun& obj);

    void emitAsmInstr(const AsmInstr& obj);

    void emitAsmMov(const AsmMov& obj);

    void emitAsmRet(const AsmRet& obj);
    
    void emitAsmUnary(const AsmUnary& obj);

    void emitAsmAllocStack(const AsmAllocStack& obj);

    std::string formatAsmOperand(const AsmOperand& obj) const;

    std::string formatAsmReg(const AsmReg& obj) const;

    std::string formatAsmUnaryOp(const AsmUnary::Type& type) const;

    template <typename... T>
    void pushLine(std::format_string<T...> str, T&&... args) {
        lines->emplace_back(std::format(str, std::forward<T>(args)...));
    }

    template <typename... T>
    void appendLine(std::format_string<T...> str, T&&... args) {
        if (lines->empty()) {
            fail("Cannot append to line: lines are empty.");
        }

        auto segment = std::format(str, std::forward<T>(args)...);
        lines->back().append(segment);
    }

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(std::format("AsmEmitterError: {}", message));
    }

    AsmNodePtr ast{nullptr};
    LinesPtr lines{nullptr};
    utils::Platform platform;
};
} // namespace emit
} // namespace back
} // namespace wacc