#include "ast.hpp"

namespace wacc::front::ast {
// AstNode
AstNode::AstNode(Type type) : type{type} {
}

// AstExpr
AstExpr::AstExpr(Type type) : AstNode(type) {
}

// AstLitInt
AstLitInt::AstLitInt(Token token, int value) :
    AstExpr(LITERAL_INT), token{token}, value{value} {
}

// AstUnary
AstUnary::AstUnary(Type op, AstExprPtr expr) :
    AstExpr(UNARY), op{op}, expr{std::move(expr)} {
}

// AstBinary
AstBinary::AstBinary(Type op, AstExprPtr left, AstExprPtr right) :
    AstExpr(BINARY), op{op}, left{std::move(left)}, right{std::move(right)} {
}

// AstIdent
AstIdent::AstIdent(Token token, std::string_view value) :
    AstNode(IDENT), token{token}, value{value} {
}

// AstStmt
AstStmt::AstStmt(Type type) : AstNode(type) {
}

// AstReturn
AstReturn::AstReturn(AstExprPtr expr) : AstStmt(RETURN), expr{std::move(expr)} {
}

// AstFun
AstFun::AstFun(AstIdentPtr name, AstStmtPtr body) :
    AstNode(FUNCTION), name{std::move(name)}, body{std::move(body)} {
}

// AstProg
AstProg::AstProg(AstFunPtr function) :
    AstNode(PROGRAM), function{std::move(function)} {
}

// AstTree
AstTree::AstTree(ConstIter begin, ConstIter end, TokensPtr root) :
    begin{begin}, end{end}, root{std::move(root)} {
}
} // namespace wacc::front::ast