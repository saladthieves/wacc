#pragma once

#include <format>
#include <map>
#include <string_view>
#include <utility>

namespace wacc {
namespace front {
namespace token {
enum class TokenType : std::uint8_t {
    // clang-format off
    KEYWORD_INT = 1,
    KEYWORD_VOID,
    KEYWORD_RETURN,

    IDENTIFIER,
    
    OPEN_PAREN, CLOSE_PAREN,
    OPEN_BRACE, CLOSE_BRACE,

    OP_BIT_AND, OP_BIT_OR, OP_BIT_XOR,
    OP_BIT_LSH, OP_BIT_RSH,
    OP_BIT_COMPLEMENT,

    OP_LOG_AND, OP_LOG_OR, OP_LOG_NOT,
    
    OP_NEGATE, OP_ADDITION,
    OP_MULTIPLY, OP_DIVIDE, OP_REMAINDER,
    
    OP_EQUAL, OP_NOT_EQUAL,
    OP_LESS_THAN, OP_LESS_EQUAL,
    OP_GREATER_THAN, OP_GREATER_EQUAL,
    OP_ASSIGN,

    OP_DECREMENT,

    LITERAL_INT,

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
    static constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    static auto format(const TokenType& type, format_context& context) {
        std::string value{};

        switch (type) {
            using enum TokenType;

            case KEYWORD_INT:       value = "KEYWORD_INT"; break;
            case KEYWORD_VOID:      value = "KEYWORD_VOID"; break;
            case KEYWORD_RETURN:    value = "KEYWORD_RETURN"; break;
            case IDENTIFIER:        value = "IDENTIFIER"; break;
            case OPEN_PAREN:        value = "OPEN_PAREN"; break;
            case CLOSE_PAREN:       value = "CLOSE_PAREN"; break;
            case OPEN_BRACE:        value = "OPEN_BRACE"; break;
            case CLOSE_BRACE:       value = "CLOSE_BRACE"; break;
            case OP_BIT_AND:        value = "OP_BIT_AND"; break;
            case OP_BIT_OR:         value = "OP_BIT_OR"; break;
            case OP_BIT_XOR:        value = "OP_BIT_XOR"; break;
            case OP_BIT_LSH:        value = "OP_BIT_LSH"; break;
            case OP_BIT_RSH:        value = "OP_BIT_RSH"; break;
            case OP_BIT_COMPLEMENT: value = "OP_BIT_COMPLEMENT"; break;
            case OP_LOG_AND:        value = "OP_LOG_AND"; break;
            case OP_LOG_OR:         value = "OP_LOG_OR"; break;
            case OP_LOG_NOT:        value = "OP_LOG_NOT"; break;
            case OP_EQUAL:          value = "OP_EQUAL"; break;
            case OP_NEGATE:         value = "OP_NEGATE"; break;
            case OP_ADDITION:       value = "OP_ADDITION"; break;
            case OP_MULTIPLY:       value = "OP_MULTIPLY"; break;
            case OP_DIVIDE:         value = "OP_DIVIDE"; break;
            case OP_REMAINDER:      value = "OP_REMAINDER"; break;
            case OP_LESS_THAN:      value = "OP_LESS_THAN"; break;
            case OP_LESS_EQUAL:     value = "OP_LESS_EQUAL"; break;
            case OP_GREATER_THAN:   value = "OP_GREATER_THAN"; break;
            case OP_GREATER_EQUAL:  value = "OP_GREATER_EQUAL"; break;
            case OP_DECREMENT:      value = "OP_DECREMENT"; break;
            case LITERAL_INT:       value = "LITERAL_INT"; break;
            case SEMICOLON:         value = "SEMICOLON"; break;
            case INVALID_TOKEN:     value = "INVALID_TOKEN"; break;
            case END:               value = "END"; break;
            default:                {
                throw std::format_error(
                    std::format("Unhandled token::TokenType::[{}] enum",
                                std::to_underlying(type)));
            }
        }
        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std
