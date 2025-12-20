#include "ast.hpp"

namespace wacc::front::ast {
// AstConstInt
AstConstInt::AstConstInt(Token token, int value) : token{token}, value{value} {
}

// AstUnary
AstUnary::AstUnary(AstUnaryOpType op, AstExprPtr expr) :
    op{op}, expr{std::move(expr)} {
}

// AstIdent
AstIdent::AstIdent(Token token, std::string_view value) :
    token{token}, value{value} {
}

// AstReturn
AstReturn::AstReturn(AstExprPtr expr) :
    expr{std::move(expr)} {
}

// AstFun
AstFun::AstFun(AstIdentPtr name, AstStmtPtr body) :
    name{std::move(name)}, body{std::move(body)} {
}

// AstProg
AstProg::AstProg(AstFunPtr function) : function{std::move(function)} {
}

// AstTree
AstTree::AstTree(ConstIter begin, ConstIter end, TokensPtr root) :
    begin{begin}, end{end}, root{std::move(root)} {
}
} // namespace wacc::front::ast