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

using Str = std::string_view;

static constexpr unsigned INSTR_INDENT = 4;
static constexpr unsigned INSTR_WIDTH = 7;
static constexpr unsigned OP_WIDTH = 13;
} // namespace

class AsmEmitter {
public:
    AsmEmitter();

    AsmEmitter(AsmNodePtr ptr, utils::Platform platform);

    LinesPtr emit();

protected:
    void emitAsmProg(const AsmProg& obj);

    void emitAsmFun(const AsmFun& obj);

    void emitAsmInstr(const AsmInstr& obj);

    void emitAsmMov(const AsmMov& obj);

    void emitAsmRet(const AsmRet& obj);

    void emitAsmUnary(const AsmUnary& obj);

    void emitAsmBinary(const AsmBinary& obj);

    void emitAsmIdiv(const AsmIdiv& obj);

    void emitAsmCdq();

    void emitAsmAllocStack(const AsmAllocStack& obj);

    void emitAsmCmp(const AsmCmp& obj);

    void emitAsmJmp(const AsmJmp& obj);

    void emitAsmJmpCond(const AsmJmpCond& obj);

    void emitAsmSetCond(const AsmSetCond& obj);

    void emitAsmLabel(const AsmLabel& obj);

    std::string formatLabel(std::string_view value) const;

    static std::string formatAsmOperand(const AsmOperand& obj);

    static std::string formatAsmReg(const AsmReg& obj);

    static std::string formatAsmUnaryOp(const AsmUnary::Type& type);

    static std::string formatAsmBinaryOp(const AsmBinary::Type& type);

    static std::string formatAsmCondCode(const CondCode& code);

    static std::string fmtInstr(Str instr) {
        return std::format("{:{}}{}", "", INSTR_INDENT, instr);
    }

    static std::string fmtInstr(Str instr, Str op) {
        return std::format("{:{}}{:<{}} {}", //
                           "", INSTR_INDENT, instr, INSTR_WIDTH, op);
    }

    static std::string fmtInstr(Str instr, Str src, Str dest) {
        auto op = std::format("{},", src);
        return std::format("{:{}}{:<{}} {:<{}}{}", "", INSTR_INDENT, instr,
                           INSTR_WIDTH, op, OP_WIDTH, dest);
    }

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

    void pushComment(std::string_view comment) {
        lines->emplace_back(
            std::format("{:<{}}# {}", "", INSTR_INDENT, comment));
    }

    void appendComment(std::string_view comment) {
        if (lines->empty()) {
            fail("Cannot append comment to line: lines are empty.");
        }

        lines->back().append(std::format(" # {}", comment));
    }

    template <typename... T>
    [[noreturn]] static void fail(std::format_string<T...> str = "",
                                  T&&... args) {
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