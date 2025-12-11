#pragma once

#include "token.hpp"

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

    TokensPtr scan();

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

    void makeEndToken() {
        tokens->emplace_back(token::TokenType::END, line, "END");
    }

    void scanContent();

    void scanIdentifiers();

    void scanNumberConstant();

    template <typename... T>
    [[noreturn]] void fail(std::format_string<T...> str = "",
                           T&&... args) const {
        auto message = std::format(str, std::forward<T>(args)...);
        throw std::runtime_error(std::format("LexerError:\n  message: {}", message));
    }

    Iter current;
    Iter next;
    ConstIter end;

    // TODO: Add missing values to keep track of current line segments
    unsigned int line;

    TokensPtr tokens;
};
} // namespace lex
} // namespace front
} // namespace wacc