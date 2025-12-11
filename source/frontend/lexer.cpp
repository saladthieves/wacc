#include "lexer.hpp"

namespace wacc::front::lex {
Lexer::Lexer(std::string_view source) :
    current{source.begin()}, next{source.begin()}, end{source.cend()}, line{1},
    tokens{std::make_unique<Tokens>()} {
}

auto Lexer::scan() -> TokensPtr {
    while (!isAtEnd()) {
        skipWhiteSpace();
        if (isAtEnd()) break;

        const char& c = advance();

        if (isAlphaNumeric(c)) {
            scanContent();
            continue;
        }

        switch (c) {
            using enum token::TokenType;

            case '(': makeToken(OPEN_PAREN); break;
            case ')': makeToken(CLOSE_PAREN); break;
            case '{': makeToken(OPEN_BRACE); break;
            case '}': makeToken(CLOSE_BRACE); break;
            case ';': makeToken(SEMICOLON); break;

            default: fail("Unexpected token character: [{}]", c);
        }
    }

    makeEndToken();

    return std::move(tokens);
}

void Lexer::skipWhiteSpace() {
    while (!isAtEnd()) {
        const char& c = peekNext();
        if (std::isspace(c)) {
            if (c == '\n') ++line;
            advance();
            continue;
        } else {
            break;
        }
    }

    sync();
}

void Lexer::makeToken(token::TokenType type) {
    tokens->emplace_back(type, line, std::string_view{current, next});
    sync();
}

void Lexer::scanContent() {
    const char& c = peek();
    if (isAlpha(c)) {
        scanIdentifiers();
    } else {
        scanNumberConstant();
    }
}

void Lexer::scanIdentifiers() {
    while (!isAtEnd() && isAlphaNumeric(peekNext())) {
        advance();
    }

    const auto result = token::Keywords::getKeyword({current, next});
    if (result.first) {
        makeToken(result.second);
    } else {
        makeToken(token::TokenType::IDENTIFIER);
    }
}

void Lexer::scanNumberConstant() {
    while (!isAtEnd() && isNumeric(peekNext())) {
        advance();
    }

    const char& c = peekNext();
    if (isAlpha(c)) {
        fail("Unexpected character in number constant: [{}]", c);
    }

    makeToken(token::TokenType::CONSTANT_INT);
}
} // namespace wacc::front::lex