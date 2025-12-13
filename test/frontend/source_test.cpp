#include "source.hpp"
#include "token.hpp"

#include <gtest/gtest.h>
#include <string_view>

using wacc::front::src::Source;
using wacc::front::token::Token;
using wacc::front::token::TokenType;

using std::string_view;

TEST(TestSource, init) {
    // ARRANGE
    const auto src = "int main(void) {";

    // ACT
    auto source = Source{src};
    auto content = string_view{source.cbegin(), source.cend()};

    // ASSERT
    ASSERT_TRUE(content == src);
}

TEST(TestSource, decorate) {
    // ARRANGE
    const auto src = "int main(void) {";
    auto source = Source{src};

    auto lineStart = 0;
    auto lineNo = 8;
    auto offset = 5;
    auto value = string_view{"main(void)"};

    // ACT
    auto decorated = source.decorate(lineStart, lineNo, offset, value);

    // ASSERT
    ASSERT_TRUE(decorated.contains("Line [8]: int main(void) {"));
    ASSERT_TRUE(decorated.contains("              ^^^^^^^^^^\n"));
}

TEST(TestSource, decorateToken) {
    // ARRANGE
    using enum TokenType;
    const auto src = "int main(void) {";
    auto source = Source{src};
    auto value = string_view{"main"};
    auto token = Token{IDENTIFIER, 1, 0, 17, 5, value};

    // ACT
    auto decorated = source.decorate(token);

    // ASSERT
    ASSERT_TRUE(decorated.contains("Line [1]: int main(void) {"));
    ASSERT_TRUE(decorated.contains("              ^^^^\n"));
}
