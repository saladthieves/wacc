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

class VariableGenerator {
public:
    VariableGenerator();

    void reset();

    void resetSession(std::string_view session = "");

    void resetFunction(std::string_view function);

    std::string generate();

private:
    std::string session{};
    std::string function{};
    unsigned int count{0};
};

// TackyGenerator
class TackyGenerator {
public:
    explicit TackyGenerator(AstNodePtr ptr);

    TackyNodePtr generate() const;

private:
    TackyProgPtr genForAstProg(const AstProg& obj) const;

    TackyFunPtr genForAstFun(const AstFun& obj) const;

    TackyInstrPtrs genForAstStmt(const AstStmt& obj) const;

    void genForAstReturn(const AstReturn& obj, TackyInstrPtrs& body) const;

    TackyValPtr genForAstExpr(const AstExpr& obj, TackyInstrPtrs& body) const;

    TackyValPtr genForAstUnary(const AstUnary& obj, TackyInstrPtrs& body) const;

    TackyValPtr genForAstBinary(const AstBinary& obj,
                                TackyInstrPtrs& body) const;

    static TackyLitInt genForAstLitInt(const AstLitInt& obj);

    static TackyUnary::Type genForAstUnaryOp(const AstUnary::Type& type);

    static TackyBinary::Type genForAstBinaryOp(const AstBinary::Type& type);

    template <typename... T>
    [[noreturn]] static void fail(std::format_string<T...> str = "",
                                  T&&... args) {
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