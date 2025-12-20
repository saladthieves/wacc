#pragma once

#include "ast.hpp"
#include "tacky_ast.hpp"
#include <format>

namespace wacc {
namespace tacky {
namespace gen {
namespace {
using namespace wacc::front::ast;
using namespace wacc::tacky::ast;
} // namespace

// S0A1B.MAIN.UNARY_NEGATE.TEMP.0
class VariableGenerator {
public:
    VariableGenerator();

    void resetSession(std::string session = "");
    void resetFunction(std::string function);
    std::string generate(TackyUnaryOpType type);

private:
    std::string session{};
    std::string function{};
    unsigned int count{0};
};

// TackyGenerator
class TackyGenerator {
public:
    TackyGenerator(AstNodePtr ptr);

    TackyNodePtr generate() const;

private:
    TackyProgPtr genForAstProg(const AstProg& obj) const;

    TackyFunPtr genForAstFun(const AstFun& obj) const;

    TackyInstrs genForAstStmt(const AstStmt& obj) const;

    void genForAstReturn(const AstReturn& obj, TackyInstrs& body) const;

    TackyValPtr genForAstExpr(const AstExpr& obj, TackyInstrs& body) const;

    TackyValPtr genForAstUnary(const AstUnary& obj, TackyInstrs& body) const;

    TackyConstant genForAstConstInt(const AstConstInt& obj) const;

    TackyUnaryOpType genForAstUnaryOp(const AstUnaryOpType& op) const;

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        const auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(
            std::format("TackyGeneratorError: {}", message));
    }

    AstNodePtr ast{nullptr};
    mutable VariableGenerator generator{};
};
} // namespace gen
} // namespace tacky
} // namespace wacc