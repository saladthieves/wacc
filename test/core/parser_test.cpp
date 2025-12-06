#include "ast.hpp"
#include "formatting.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "test_utils.hpp"
#include "token.hpp"

#include <format>
#include <gtest/gtest.h>
#include <memory>
#include <print>
#include <string>
#include <tuple>
#include <vector>

using enum wacc::core::token::TokenType;
using wacc::core::lex::Lexer;
using wacc::core::parse::Parser;
using wacc::core::token::Token;
using wacc::core::token::TokenType;
using wacc::test::utils::as;

using namespace wacc::core::ast;

using std::make_unique;
using std::string;
using std::tuple;
using std::vector;

using Tokens = vector<Token>;

TEST(ParserTest, parseThrowOnEmpty) {
    // ARRANGE
    auto tokens = make_unique<Tokens>();
    auto parser = Parser{std::move(tokens)};
    string error{};

    // ACT
    try {
        parser.parse();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("No tokens found"));
}

TEST(ParserTest, parseInvalidProgram) {
    // ARRANGE
    const auto tests = vector<tuple<string, TokenType, TokenType>>{
        {"15",                          KEYWORD_INT,    CONSTANT_INT},
        {"int ;",                       IDENTIFIER,     SEMICOLON   },
        {"int main",                    OPEN_PAREN,     END         },
        {"int main ()",                 KEYWORD_VOID,   CLOSE_PAREN },
        {"int main (void)}",            OPEN_BRACE,     CLOSE_BRACE },
        {"int main (void){}",           KEYWORD_RETURN, CLOSE_BRACE },
        {"int main (void){ 15; }",      KEYWORD_RETURN, CONSTANT_INT},
        {"int main (void){ return; }",  CONSTANT_INT,   SEMICOLON   },
        {"int main (void){ return 42}", SEMICOLON,      CLOSE_BRACE },
    };

    for (const auto& test : tests) {
        // ACT
        auto source = std::get<0>(test);

        auto lexer = Lexer{source};
        auto tokens = lexer.scan();
        auto parser = Parser{std::move(tokens)};
        string error{};

        try {
            parser.parse();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
        ASSERT_TRUE(error.contains("ParserError"));

        auto expected = std::get<1>(test);
        ASSERT_TRUE(error.contains(std::format("Expected [{}]", expected)));

        auto actual = std::get<2>(test);
        ASSERT_TRUE(error.contains(std::format("but got [{}]", actual)));
    }
}

TEST(ParserTest, parseProgram) {
    // ARRANGE
    const auto source =
        R"(int main(void) {
        return 42;
    }
    )";
    auto lexer = Lexer{source};
    auto tokens = lexer.scan();
    auto parser = Parser{std::move(tokens)};

    // ACT
    auto node = parser.parse();

    // ASSERT
    auto program = as<AstProg>(node);

    auto function = as<AstFun>(program->function);
    auto functionName = as<AstIdent>(function->name);
    ASSERT_TRUE(functionName->value == "main");

    auto functionBody = as<AstReturn>(function->body);
    auto returnExpression = as<AstInt>(functionBody->expression);
    ASSERT_EQ(returnExpression->token.type, CONSTANT_INT);
    ASSERT_TRUE(returnExpression->value == 42);
}