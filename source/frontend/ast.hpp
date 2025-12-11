#pragma once

#include "token.hpp"

#include <format>

namespace wacc {
namespace front {
namespace ast {
enum class AstNodeType {
    EXPRESSION,
    INTEGER,
    IDENTIFIER,
    STATEMENT,
    RETURN,
    FUNCTION,
    PROGRAM
};

// Forward Declarations
class AstNode;
class AstExpr;
class AstInt;
class AstIdent;
class AstStmt;
class AstReturn;
class AstFun;
class AstProg;

// Aliases
using AstNodePtr = std::unique_ptr<AstNode>;
using AstExprPtr = std::unique_ptr<AstExpr>;
using AstIntPtr = std::unique_ptr<AstInt>;
using AstIdentPtr = std::unique_ptr<AstIdent>;
using AstStmtPtr = std::unique_ptr<AstStmt>;
using AstReturnPtr = std::unique_ptr<AstReturn>;
using AstFunPtr = std::unique_ptr<AstFun>;
using AstProgPtr = std::unique_ptr<AstProg>;

namespace {
using Token = token::Token;
using enum AstNodeType;
constexpr auto INDENT = "  ";
} // namespace

// AstNode
class AstNode {
public:
    virtual AstNodeType type() const = 0;

    virtual std::string format(unsigned level) const = 0;
    virtual ~AstNode() = default;
};

// AstExpr
class AstExpr : public AstNode {
public:
    virtual AstNodeType type() const override { return EXPRESSION; }

    virtual std::string format(unsigned level) const override = 0;
};

// AstInt
class AstInt final : public AstExpr {
public:
    AstInt(Token token, int value);

    virtual AstNodeType type() const override { return INTEGER; }

    virtual std::string format(unsigned level) const override;

    Token token;
    int value;
};

// AstIdent
class AstIdent final : public AstNode {
public:
    AstIdent(Token token, std::string_view value);

    virtual AstNodeType type() const override { return IDENTIFIER; }

    virtual std::string format(unsigned level) const override;

    Token token;
    std::string_view value;
};

// AstStmt
class AstStmt : public AstNode {
public:
    virtual AstNodeType type() const override { return STATEMENT; }

    virtual std::string format(unsigned level) const override = 0;
};

// AstReturn
class AstReturn : public AstStmt {
public:
    AstReturn(AstExprPtr expression);

    virtual AstNodeType type() const override { return RETURN; }

    virtual std::string format(unsigned level) const override;

    AstExprPtr expression;
};

// AstFun
class AstFun : public AstNode {
public:
    AstFun(AstIdentPtr name, AstStmtPtr body);

    virtual AstNodeType type() const override { return FUNCTION; }

    virtual std::string format(unsigned level) const override;

    AstIdentPtr name;
    AstStmtPtr body;
};

// AstProg
class AstProg : public AstNode {
public:
    AstProg(AstFunPtr function);

    virtual AstNodeType type() const override { return PROGRAM; }

    virtual std::string format(unsigned level) const override;

    AstFunPtr function;
};

namespace {
// TODO: Remove after reformatting
std::string indent(int level = 0);
} // namespace
} // namespace ast
} // namespace front
} // namespace wacc

namespace std {
namespace {
using wacc::front::ast::AstNodeType;
}

template <>
class formatter<AstNodeType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AstNodeType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AstNodeType;
            case EXPRESSION: value = "EXPRESSION"; break;
            case INTEGER:    value = "INTEGER"; break;
            case IDENTIFIER: value = "IDENTIFIER"; break;
            case STATEMENT:  value = "STATEMENT"; break;
            case RETURN:     value = "RETURN"; break;
            case FUNCTION:   value = "FUNCTION"; break;
            case PROGRAM:    value = "PROGRAM"; break;
            default:         throw std::format_error("Unhandled ast::AstNodeType enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std