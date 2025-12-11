#include "token.hpp"

namespace wacc::front::token {
const Keywords::Words Keywords::words = {
    {"int",    TokenType::KEYWORD_INT   },
    {"return", TokenType::KEYWORD_RETURN},
    {"void",   TokenType::KEYWORD_VOID  },
};

auto Keywords::getKeyword(std::string_view value) -> Entry {
    auto result = words.find(value);
    if (result == words.end()) {
        return {false, TokenType::INVALID_TOKEN};
    } else {
        return {true, result->second};
    }
}
} // namespace wacc::front::token