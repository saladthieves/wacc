#pragma once

#include "token.hpp"

#include <memory>

namespace wacc {
namespace core {
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
std::string indent(int level = 0);
}

} // namespace ast
} // namespace core
} // namespace wacc