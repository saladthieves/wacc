#pragma once

#include "asm_ast.hpp"
#include "ast.hpp"

#include <__format/format_functions.h>
#include <format>
#include <stdexcept>

namespace wacc {
namespace back {
namespace gen {
namespace {
using namespace wacc::front::ast;
using namespace wacc::back::ast;
} // namespace

class AsmGenerator {
public:
    AsmGenerator(AstNodePtr ptr);

    AsmNodePtr generate() const;

private:
    AsmProgPtr genForAstProg(const AstProg& obj) const;

    AsmFunPtr genForAstFun(const AstFun& obj) const;

    AsmInstrPtrs genForAstStmt(const AstStmt& obj) const;

    AsmInstrPtrs genForAstReturn(const AstReturn& obj) const;

    AsmMovPtr genAsmMov(const AstExpr& obj) const;

    AsmRetPtr genAsmRet() const;

    AsmImmPtr genForAstInt(const AstInt& obj) const;

    AsmRegPtr genAsmReg() const;

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(std::format("AsmGeneratorError: {}", message));
    }

    AstNodePtr ast;
};
} // namespace gen
} // namespace back
} // namespace wacc