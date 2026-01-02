#pragma once

#include "ast.hpp"
#include "source.hpp"
#include "token.hpp"

#include <map>
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

    using PrecedenceKey = TokenType;
    using PrecedenceValue = unsigned int;
    using PrecedenceMap = std::map<PrecedenceKey, PrecedenceValue>;

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

    ast::AstExprPtr parseExpression(PrecedenceValue value = 0);

    ast::AstExprPtr parseFactor();

    ast::AstIdentPtr parseIdentifier();

    ast::AstLitIntPtr parseLiteralInteger();

    ast::AstUnaryPtr parseUnaryExpression();

    ast::AstUnary::Type parseUnaryOperator();

    ast::AstBinary::Type parseBinaryOperator();

    bool isFactor(const TokenType& type) const;

    bool isBinaryOp(const TokenType& type) const;

    unsigned int getPrecedence(const TokenType& type) const;

    const Token& expect(std::initializer_list<const TokenType> types);

    const Token& expect(const TokenType& type);

    const Token& expectAny(std::initializer_list<const TokenType> types);

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

    static const PrecedenceMap precedences;
};
} // namespace parse
} // namespace front
} // namespace wacc