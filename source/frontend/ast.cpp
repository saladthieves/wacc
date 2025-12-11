#include "ast.hpp"

namespace wacc::front::ast {
// AstInt
AstInt::AstInt(Token token, int value) : token{token}, value{value} {
}

// AstIdent
AstIdent::AstIdent(Token token, std::string_view value) :
    token{token}, value{value} {
}

// AstReturn
AstReturn::AstReturn(AstExprPtr expression) :
    expression{std::move(expression)} {
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