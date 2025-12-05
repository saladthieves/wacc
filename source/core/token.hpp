#pragma once

#include <map>
#include <string_view>

namespace wacc {
namespace core {
namespace token {
enum class TokenType : unsigned int {
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
    unsigned int line{0};
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
} // namespace core
} // namespace wacc