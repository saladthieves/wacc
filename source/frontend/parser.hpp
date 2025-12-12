#pragma once

#include "ast.hpp"
#include "source.hpp"
#include "token.hpp"

#include <vector>

namespace wacc {
namespace front {
namespace parse {
class Parser {
    using Token = token::Token;
    using TokenType = token::TokenType;
    using Tokens = std::vector<Token>;
    using TokensPtr = std::unique_ptr<Tokens>;
    using ConstIter = Tokens::const_iterator;

public:
    Parser(TokensPtr ptr, src::Source source);

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

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        std::string decorated{};
        if (!tokens->empty()) {
            decorated = src.decorate(*current);
        }
        const auto message = std::format(str, std::forward<T>(args)...);

        throw std::runtime_error(
            std::format("ParserError: {}\n{}", message, decorated));
    }

    TokensPtr tokens;

    src::Source src;
    ConstIter current;
    ConstIter next;
    ConstIter end;
};
} // namespace parse
} // namespace front
} // namespace wacc