#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "source.hpp"
#include "test_utils.hpp"
#include "token.hpp"

#include <format>
#include <gtest/gtest.h>
#include <memory>
#include <print>
#include <string>
#include <tuple>
#include <vector>

using enum wacc::front::token::TokenType;
using enum wacc::front::ast::AstUnaryOpType;
using wacc::front::lex::Lexer;
using wacc::front::parse::Parser;
using wacc::front::src::Source;
using wacc::front::token::Token;
using wacc::front::token::TokenType;
using wacc::test::utils::as;

using namespace wacc::front::ast;

using std::make_unique;
using std::string;
using std::string_view;
using std::tuple;
using std::vector;

using Tokens = vector<Token>;

TEST(ParserTest, parseThrowOnEmpty) {
    // ARRANGE
    auto source = std::string_view{""};
    auto ptr = make_unique<Tokens>();
    auto parser = Parser{std::move(ptr), Source{source}};
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
        {"15",                          KEYWORD_INT,           CONSTANT_INT},
        {"int ;",                       TokenType::IDENTIFIER, SEMICOLON   },
        {"int main",                    OPEN_PAREN,            END         },
        {"int main ()",                 KEYWORD_VOID,          CLOSE_PAREN },
        {"int main (void)}",            OPEN_BRACE,            CLOSE_BRACE },
        {"int main (void){}",           KEYWORD_RETURN,        CLOSE_BRACE },
        {"int main (void){ 15; }",      KEYWORD_RETURN,        CONSTANT_INT},
        {"int main (void){ return 42}", SEMICOLON,             CLOSE_BRACE },
    };

    for (const auto& test : tests) {
        // ACT
        auto source = Source{std::get<0>(test)};

        auto lexer = Lexer{source};
        auto parser = Parser{lexer.scan(), source};
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

TEST(ParserTest, parseMalformedExpression) {
    // ARRANGE
    const auto tests = vector<string>{
        "int main(void) { return; }",      "int main(void) { return (); }",
        "int main(void) { return (()); }", "int main(void) { return -; }",
        "int main(void) { return ~; }",    "int main(void) { return -(); }",
        "int main(void) { return ~(); }",  "int main(void) { return --15; }",
    };

    for (const auto& test : tests) {
        // ACT
        auto source = Source{test};
        auto lexer = Lexer{source};
        auto parser = Parser{lexer.scan(), source};
        string error{};

        try {
            parser.parse();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
        ASSERT_TRUE(error.contains("ParserError"));
        ASSERT_TRUE(error.contains("malformed expression") ||
                    error.contains("Expected") ||
                    error.contains("Cannot parse"));
    }
}

TEST(ParserTest, parseProgram) {
    // ARRANGE
    const auto source =
        R"(int main(void) {
        return ~(-42);
    }
    )";
    auto lexer = Lexer{Source{source}};
    auto parser = Parser{lexer.scan(), Source{source}};

    // ACT
    auto ptr = parser.parse();

    // ASSERT
    auto program = as<AstProg>(ptr);

    auto function = as<AstFun>(program->function);
    auto functionName = as<AstIdent>(function->name);
    ASSERT_TRUE(functionName->value == "main");

    auto functionBody = as<AstReturn>(function->body);
    auto returnExprUnary = as<AstUnary>(functionBody->expr);
    ASSERT_EQ(returnExprUnary->op, UNARY_COMPLEMENT);
    auto returnExprNeg = as<AstUnary>(returnExprUnary->expr);
    ASSERT_EQ(returnExprNeg->op, UNARY_NEGATE);
    auto returnExprNegVal = as<AstConstInt>(returnExprNeg->expr);
    ASSERT_TRUE(returnExprNegVal->value == 42);

    std::println("{}", ptr);
}

TEST(ParserTest, parsePrograms) {
    // ARRANGE
    const auto tests = vector<string>{
        "int main(void) { return -15; }",
        "int main(void) { return ~30; }",
        "int main(void) { return ~~45; }",
        "int main(void) { return ~-15; }",
        "int main(void) { return -~80; }",
        "int main(void) { return -~~100; }",
        "int main(void) { return -(~~200); }",
        "int main(void) { return ~-(~300); }",
        "int main(void) { return (~15); }",
        "int main(void) { return ~(15); }",
        "int main(void) { return ~~(-3); }",
        "int main(void) { return ~(-~(~-(~~-15))); }",
    };

    for (const auto& test : tests) {
        auto source = Source{test};
        auto lexer = Lexer{source};
        auto parser = Parser{lexer.scan(), source};
        string error{};

        // ACT
        try {
            parser.parse();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_TRUE(error.empty());
    }
}
