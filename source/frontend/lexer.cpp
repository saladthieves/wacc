#include "lexer.hpp"
#include "source.hpp"
#include "token.hpp"
#include "utils.hpp"

namespace wacc::front::lex {
Lexer::Lexer(src::Source source) :
    src{source}, current{src.cbegin()}, next{src.cbegin()}, begin{src.cbegin()},
    end{src.cend()}, tokens{std::make_unique<Tokens>()} {
}

Lexer::TokensPtr Lexer::scan() {
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
            case '~': makeToken(OP_BIT_COMPLEMENT); break;
            case '&': makeToken(OP_BIT_AND); break;
            case '|': makeToken(OP_BIT_OR); break;
            case '^': makeToken(OP_BIT_XOR); break;
            case '+': makeToken(OP_ADDITION); break;
            case '*': makeToken(OP_MULTIPLY); break;
            case '/': makeToken(OP_DIVIDE); break;
            case '%': makeToken(OP_REMAINDER); break;
            case '<': {
                const auto& nextChar = peekNext();
                if (nextChar == '<' || nextChar == '=') {
                    advance();
                    makeToken(nextChar == '<' ? OP_BIT_LSH : OP_LESS_EQUAL);
                    break;
                }
                makeToken(OP_LESS_THAN);
                break;
            }
            case '>': {
                const auto& nextChar = peekNext();
                if (nextChar == '>' || nextChar == '=') {
                    advance();
                    makeToken(nextChar == '>' ? OP_BIT_RSH : OP_GREATER_EQUAL);
                    break;
                }
                makeToken(OP_GREATER_THAN);
                break;
            }
            case '-': {
                if (peekNext() == '-') {
                    advance();
                    makeToken(OP_DECREMENT);
                } else {
                    makeToken(OP_NEGATE);
                }
                break;
            }
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
            advance();
            if (c == '\n') {
                ++lineNo;
                lineStart = next - begin;
            }
            continue;
        } else {
            break;
        }
    }

    sync();
}

void Lexer::makeToken(token::TokenType type) {
    const auto lineStop = utils::getLineStop(next, begin, end);
    const auto offset = current - (begin + lineStart);
    tokens->emplace_back(type, lineNo, lineStart, lineStop, offset,
                         std::string_view{current, next});
    sync();
}

void Lexer::makeEndToken() {
    const auto lineStop = utils::getLineStop(next, begin, end);
    const auto offset = current - (begin + lineStart);
    tokens->emplace_back(token::TokenType::END, lineNo, lineStart, lineStop,
                         offset, "END");
}

void Lexer::scanContent() {
    const char& c = peek();
    if (isAlpha(c)) {
        scanIdentifiers();
    } else {
        scanNumberLiteral();
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

void Lexer::scanNumberLiteral() {
    while (!isAtEnd() && isNumeric(peekNext())) {
        advance();
    }

    const char& c = peekNext();
    if (isAlpha(c)) {
        fail("Unexpected character in number literal: [{}]", c);
    }

    makeToken(token::TokenType::LITERAL_INT);
}
} // namespace wacc::front::lex