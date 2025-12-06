#pragma once

#include "token.hpp"

#include <memory>

namespace wacc {
namespace core {
namespace ast {

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
}

// AstNode
class AstNode {
public:
    virtual ~AstNode() = default;
};

// AstExpr
class AstExpr : public AstNode {};

// AstInt
class AstInt final : public AstExpr {
public:
    AstInt(Token token, int value);

    Token token;
    int value;
};

// AstIdent
class AstIdent final : public AstNode {
public:
    AstIdent(Token token, std::string_view value);

    Token token;
    std::string_view value;
};

// AstStmt
class AstStmt : public AstNode {};

// AstReturn
class AstReturn : public AstStmt {
public:
    AstReturn(AstExprPtr expression);

    AstExprPtr expression;
};

// AstFun
class AstFun : public AstNode {
public:
    AstFun(AstIdentPtr name, AstStmtPtr body);

    AstIdentPtr name;
    AstStmtPtr body;
};

// AstProg
class AstProg : public AstNode {
public:
    AstProg(AstFunPtr function);

    AstFunPtr function;
};

} // namespace ast
} // namespace core
} // namespace wacc