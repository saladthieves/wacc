#pragma once

#include "token.hpp"

#include <cctype>
#include <memory>
#include <string_view>
#include <vector>

namespace wacc {
namespace core {
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

    [[noreturn]] void fail(std::string_view message);

    Iter current;
    Iter next;
    ConstIter end;

    unsigned int line;

    TokensPtr tokens;
};
} // namespace lex
} // namespace core
} // namespace wacc