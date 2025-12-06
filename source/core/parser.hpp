#pragma once

#include "ast.hpp"
#include "token.hpp"

#include <memory>
#include <vector>

namespace wacc {
namespace core {
namespace parse {
class Parser {
    using Token = token::Token;
    using TokenType = token::TokenType;
    using Tokens = std::vector<Token>;
    using TokensPtr = std::unique_ptr<Tokens>;
    using ConstIter = Tokens::const_iterator;

public:
    Parser(TokensPtr ptr);

    ast::AstNodePtr parse();

private:
    bool isAtEnd() const { return next == end; }

    const Token& peek() const { return *current; }

    const Token& peekNext() const { return *next; }

    const Token& advance() { return *next++; }

    void sync() { current = next; }

    ast::AstProgPtr parseProgram();

    ast::AstFunPtr parseFunction();

    ast::AstStmtPtr parseStatement();

    ast::AstReturnPtr parseReturn();

    ast::AstExprPtr parseExpression();

    ast::AstIdentPtr parseIdentifier();

    ast::AstIntPtr parseInteger();

    const Token& expect(std::initializer_list<const TokenType> types);

    const Token& expect(const TokenType& type);

    [[noreturn]] void fail(std::string_view message);

    TokensPtr tokens;

    ConstIter current;
    ConstIter next;
    ConstIter end;
};
} // namespace parse
} // namespace core
} // namespace wacc