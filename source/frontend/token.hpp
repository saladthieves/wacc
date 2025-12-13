#pragma once

#include <format>
#include <map>
#include <string_view>

namespace wacc {
namespace front {
namespace token {
enum class TokenType : unsigned {
    // clang-format off
    KEYWORD_INT = 1,
    KEYWORD_VOID,
    KEYWORD_RETURN,

    IDENTIFIER,
    
    OPEN_PAREN, CLOSE_PAREN,
    OPEN_BRACE, CLOSE_BRACE,

    CONSTANT_INT,

    SEMICOLON,
    // clang-format on

    INVALID_TOKEN,
    END
};

class Token {
public:
    TokenType type{};
    unsigned int lineNo{1};
    unsigned int lineStart{0};
    unsigned int lineStop{0};
    unsigned int offset{0};
    std::string_view value{};
};

class Keywords {
public:
    using Words = std::map<std::string_view, TokenType>;
    using Entry = std::pair<bool, TokenType>;

    static Entry getKeyword(std::string_view value);

private:
    static const Words words;
};
} // namespace token
} // namespace front
} // namespace wacc

namespace std {
namespace {
using wacc::front::token::TokenType;
}

template <>
class formatter<TokenType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const TokenType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum TokenType;

            case KEYWORD_INT:    value = "KEYWORD_INT"; break;
            case KEYWORD_VOID:   value = "KEYWORD_VOID"; break;
            case KEYWORD_RETURN: value = "KEYWORD_RETURN"; break;
            case IDENTIFIER:     value = "IDENTIFIER"; break;
            case OPEN_PAREN:     value = "OPEN_PAREN"; break;
            case CLOSE_PAREN:    value = "CLOSE_PAREN"; break;
            case OPEN_BRACE:     value = "OPEN_BRACE"; break;
            case CLOSE_BRACE:    value = "CLOSE_BRACE"; break;
            case CONSTANT_INT:   value = "CONSTANT_INT"; break;
            case SEMICOLON:      value = "SEMICOLON"; break;
            case INVALID_TOKEN:  value = "INVALID_TOKEN"; break;
            case END:            value = "END"; break;
            default:             throw std::format_error("Unhandled token::TokenType enum");
        }
        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std
