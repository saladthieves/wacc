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
using namespace std::string_view_literals;

static constexpr auto labelTrue = "L_TRUE"sv;
static constexpr auto labelFalse = "L_FALSE"sv;
static constexpr auto labelEnd = "L_END"sv;
} // namespace

class IdentifierGenerator {
public:
    IdentifierGenerator();

    void reset();

    void resetSession(std::string_view session = "");

    void resetFunction(std::string_view function);

    std::string generateVariable();

    std::string generateLabel(const TackyBinary::Type& op,
                              std::string_view extra);

    std::string generate(unsigned int length, std::string_view pre = "",
                         std::string_view post = "");

private:
    std::string session;
    std::string function;
    unsigned int variableCount;
    unsigned int labelCount;
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

    TackyValPtr genForAstBinaryLogAndOr(const AstBinary& obj,
                                        TackyInstrPtrs& body) const;

    void tackyJumpZero(TackyValPtr val, std::string_view target,
                       TackyInstrPtrs& body) const;

    void tackyJumpNotZero(TackyValPtr val, std::string_view target,
                          TackyInstrPtrs& body) const;

    void tackyJump(std::string_view label, TackyInstrPtrs& body) const;

    void tackyCopy(TackyValPtr src, TackyValPtr dest,
                   TackyInstrPtrs& body) const;

    void tackyLabel(std::string_view identifier, TackyInstrPtrs& body) const;

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
    mutable IdentifierGenerator generator{};
};
} // namespace gen
} // namespace tacky
} // namespace wacc