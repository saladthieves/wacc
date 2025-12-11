#pragma once

#include "ast.hpp"
#include "token.hpp"
#include "utils.hpp"

#include <vector>

namespace wacc {
namespace front {
namespace lex {
class Lexer {
public:
    using Iter = std::string_view::iterator;
    using ConstIter = std::string_view::const_iterator;
    using Tokens = std::vector<token::Token>;
    using TokensPtr = std::unique_ptr<Tokens>;

    Lexer(std::string_view source);

    ast::AstTree scan();

private:
    bool isAtEnd() const { return next == end; }

    bool isNumeric(const char& c) const { return c >= '0' && c <= '9'; }

    bool isAlpha(const char& c) const {
        return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
    }

    bool isAlphaNumeric(const char& c) const {
        return isAlpha(c) || isNumeric(c);
    }

    char peek() const { return *current; }

    char peekNext() const { return *next; }

    char advance() { return *next++; }

    void sync() { current = next; }

    void skipWhiteSpace();

    void makeToken(token::TokenType type);

    void makeEndToken();

    void scanContent();

    void scanIdentifiers();

    void scanNumberConstant();

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        const auto lineStop = utils::getLineStop(next, begin, end);
        const auto line = std::string_view{begin + lineStart, begin + lineStop};
        const auto offset = current - (begin + lineStart);
        const auto value = std::string_view{current, next};
        const auto decorated = utils::decorate(lineNo, line, offset, value);
        
        const auto message = std::format(str, std::forward<T>(args)...);

        throw std::runtime_error(
            std::format("LexerError: {}:\n{}", message, decorated));
    }

    Iter current;
    Iter next;
    ConstIter begin;
    ConstIter end;

    TokensPtr tokens;

    unsigned int lineNo{1};
    unsigned int lineStart{0};
};
} // namespace lex
} // namespace front
} // namespace wacc