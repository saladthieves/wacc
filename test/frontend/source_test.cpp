#include "base_test.hpp"
#include "source.hpp"
#include "token.hpp"

#include <gtest/gtest.h>

using wacc::front::token::Token;
using wacc::front::token::TokenType;

using std::string_view;

class SourceTest : public testing::Test, public wacc::test::base::BaseTest {};

TEST_F(SourceTest, init) {
    // ARRANGE
    const auto src = "int main(void) {";

    // ACT
    auto source = getSource(src);
    auto content = string_view{source.cbegin(), source.cend()};

    // ASSERT
    ASSERT_TRUE(content == src);
}

TEST_F(SourceTest, decorate) {
    // ARRANGE
    const auto src = "int main(void) {";
    auto source = getSource(src);

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

TEST_F(SourceTest, decorateToken) {
    // ARRANGE
    const auto src = "int main(void) {";
    auto source = getSource(src);
    auto value = string_view{"main"};
    auto token = Token{TokenType::IDENTIFIER, 1, 0, 17, 5, value};

    // ACT
    auto decorated = source.decorate(token);

    // ASSERT
    ASSERT_TRUE(decorated.contains("Line [1]: int main(void) {"));
    ASSERT_TRUE(decorated.contains("              ^^^^\n"));
}