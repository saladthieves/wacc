#pragma once

#include "assembly.hpp"
#include "ast.hpp"

namespace wacc {
namespace core {
namespace gen {

namespace {
using namespace wacc::core::assembly;
using namespace wacc::core::ast;
} // namespace

class Generator {
public:
    Generator(AstNodePtr ptr);

    AsmNodePtr generate() const;

private:
    AsmProgPtr generateProgram(const AstProg& obj) const;

    AsmFunPtr generateFunction(const AstFun& obj) const;

    AsmInstrPtrs generateInstructions(const AstStmt& obj) const;

    AsmInstrPtrs generateReturnInstructions(const AstReturn& obj) const;

    AsmMovPtr generateMove(const AstExpr& obj) const;

    AsmRetPtr generateReturn() const;

    AsmImmPtr generateImmediate(const AstExpr& obj) const;

    AsmRegPtr generateRegister() const;

    [[noreturn]] void fail(std::string_view message) const;

    AstNodePtr ast;
};
} // namespace gen
} // namespace core
} // namespace wacc