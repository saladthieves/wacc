#include "lexer.hpp"
#include "source.hpp"
#include "token.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using enum wacc::front::token::TokenType;
using wacc::front::lex::Lexer;
using wacc::front::src::Source;
using wacc::front::token::Token;
using wacc::front::token::TokenType;

using std::pair;
using std::string;
using std::vector;

TEST(LexerTest, scanEmpty) {
    // ARRANGE
    const auto source = "";
    auto lexer = Lexer{Source{source}};

    // ACT
    auto ptr = lexer.scan();

    // ASSERT
    auto tokens = *ptr;
    ASSERT_FALSE(tokens.empty());
    ASSERT_EQ(tokens.front().type, END);
    ASSERT_EQ(tokens.front().lineNo, 1);
    ASSERT_TRUE(tokens.front().value == "END");
}

TEST(LexerTest, scanEmptyText) {
    // ARRANGE
    const auto source = "           ";
    auto lexer = Lexer{Source{source}};

    // ACT
    auto ptr = lexer.scan();

    // ASSERT
    auto tokens = *ptr;
    ASSERT_FALSE(tokens.empty());
    ASSERT_EQ(tokens.front().type, END);
    ASSERT_EQ(tokens.front().lineNo, 1);
    ASSERT_TRUE(tokens.front().value == "END");
}

TEST(LexerTest, scanNumberConstant) {
    // ARRANGE
    auto tests = vector<pair<string, string>>{
        {"29",        "29" },
        {"    38",    "38" },
        {"65    ",    "65" },
        {"  100    ", "100"},
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = Lexer{Source{test.first}};
        auto ptr = lexer.scan();

        // ASSERT
        auto tokens = *ptr;
        ASSERT_EQ(tokens.size(), 2);

        ASSERT_EQ(tokens.front().lineNo, 1);
        ASSERT_EQ(tokens.front().type, CONSTANT_INT);
        ASSERT_TRUE(tokens.front().value == test.second);

        ASSERT_EQ(tokens.back().lineNo, 1);
        ASSERT_EQ(tokens.back().type, END);
        ASSERT_TRUE(tokens.back().value == "END");
    }
}

TEST(LexerTest, scanNumberConstantInvalidAlpha) {
    // ARRANGE
    auto tests = vector<string>{"29s",  "38a",  "2x92",  "28ss",
                                "1xx2", "115_", "3_3_3", "123ret"};

    for (const auto& source : tests) {
        // ACT
        auto lexer = Lexer{Source{source}};
        string error{};

        try {
            lexer.scan();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
        ASSERT_TRUE(error.contains("character in number constant"));
    }
}

TEST(LexerTest, scanNumberConstantInvalidChar) {
    // ARRANGE
    auto tests = vector<string>{"29!", "38@", "2`92", "28'", "1\"2"};

    for (const auto& source : tests) {
        // ACT
        auto lexer = Lexer{Source{source}};
        string error{};

        try {
            lexer.scan();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
        ASSERT_TRUE(error.contains("Unexpected token character"));
    }
}

TEST(LexerTest, scanIdentifier) {
    // ARRANGE
    auto tests = vector<pair<string, string>>{
        {"hello",           "hello"    },
        {"   world",        "world"    },
        {"_maybe   ",       "_maybe"   },
        {" comp_   ",       "comp_"    },
        {" _control_   ",   "_control_"},
        {"    tr4pp3d_   ", "tr4pp3d_" },
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = Lexer{Source{test.first}};
        auto ptr = lexer.scan();

        // ASSERT
        auto tokens = *ptr;
        ASSERT_EQ(tokens.size(), 2);

        ASSERT_EQ(tokens.front().lineNo, 1);
        ASSERT_EQ(tokens.front().type, IDENTIFIER);
        ASSERT_TRUE(tokens.front().value == test.second);

        ASSERT_EQ(tokens.back().lineNo, 1);
        ASSERT_EQ(tokens.back().type, END);
        ASSERT_TRUE(tokens.back().value == "END");
    }
}

TEST(LexerTest, scanIdentifierInvalid) {
    // ARRANGE
    auto tests = vector<string>{"some@ne", "may?be", "no!w",   "fi#st",
                                "noun`s",  "adv'",   "sha``e!"};

    for (const auto& source : tests) {
        auto lexer = Lexer{Source{source}};
        string error{};

        // ACT
        try {
            lexer.scan();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
    }
}

TEST(LexerTest, scanKeywords) {
    // ARRANGE
    auto tests = vector<pair<string, TokenType>>{
        {"int",    KEYWORD_INT   },
        {"return", KEYWORD_RETURN},
        {"void",   KEYWORD_VOID  },
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = Lexer{Source{test.first}};
        string error{};

        auto ptr = lexer.scan();

        // ASSERT
        auto tokens = *ptr;
        ASSERT_EQ(tokens.size(), 2);

        ASSERT_EQ(tokens.front().lineNo, 1);
        ASSERT_EQ(tokens.front().type, test.second);
        ASSERT_TRUE(tokens.front().value == test.first);

        ASSERT_EQ(tokens.back().lineNo, 1);
        ASSERT_EQ(tokens.back().type, END);
        ASSERT_TRUE(tokens.back().value == "END");
    }
}

TEST(LexerTest, scanKeywordsInvalid) {
    // ARRANGE
    auto tests = vector<string>{
        {"integer"},
        {"returned"},
        {"voids"},
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = Lexer{Source{test}};
        string error{};

        auto ptr = lexer.scan();

        // ASSERT
        auto tokens = *ptr;
        ASSERT_EQ(tokens.size(), 2);

        ASSERT_EQ(tokens.front().lineNo, 1);
        ASSERT_EQ(tokens.front().type, IDENTIFIER);
        ASSERT_TRUE(tokens.front().value == test);

        ASSERT_EQ(tokens.back().lineNo, 1);
        ASSERT_EQ(tokens.back().type, END);
        ASSERT_TRUE(tokens.back().value == "END");
    }
}

TEST(LexerTest, scanSingleToken) {
    // ARRANGE
    auto tests = vector<pair<string, TokenType>>{
        {"(",  OPEN_PAREN   },
        {")",  CLOSE_PAREN  },
        {"{",  OPEN_BRACE   },
        {"}",  CLOSE_BRACE  },
        {";",  SEMICOLON    },
        {"~",  OP_COMPLEMENT},
        {"-",  OP_NEGATE    },
        {"--", OP_DECREMENT },
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = Lexer{Source{test.first}};
        string error{};

        auto ptr = lexer.scan();

        // ASSERT
        auto tokens = *ptr;
        ASSERT_EQ(tokens.size(), 2);

        ASSERT_EQ(tokens.front().lineNo, 1);
        ASSERT_EQ(tokens.front().type, test.second);
        ASSERT_TRUE(tokens.front().value == test.first);

        ASSERT_EQ(tokens.back().lineNo, 1);
        ASSERT_EQ(tokens.back().type, END);
        ASSERT_TRUE(tokens.back().value == "END");
    }
}

TEST(LexerTest, scanSingleTokenInvalid) {
    // ARRANGE
    auto tests = vector<string>{"@", "?", "!", "`", "``", "#"};

    for (const auto& test : tests) {
        // ACT
        auto lexer = Lexer{Source{test}};
        string error{};

        try {
            lexer.scan();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
        ASSERT_TRUE(error.contains("Unexpected token character: "));
    }
}

TEST(LexerTest, scanSource) {
    // ARRANGE
    const auto source =
        R"(int main(void) {
            return ~(-- -2);
        })";

    auto lexer = Lexer{Source{source}};
    string error{};
    vector<Token> tokens;

    // ACT
    try {
        tokens = *lexer.scan();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    const auto check = [](auto& token, const auto& type, const auto& value,
                          const auto& lineNo) {
        return token.type == type && token.value == value &&
               token.lineNo == lineNo;
    };

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_EQ(tokens.size(), 16);

    ASSERT_TRUE(check(tokens[0], KEYWORD_INT, "int", 1));
    ASSERT_TRUE(check(tokens[1], IDENTIFIER, "main", 1));
    ASSERT_TRUE(check(tokens[2], OPEN_PAREN, "(", 1));
    ASSERT_TRUE(check(tokens[3], KEYWORD_VOID, "void", 1));
    ASSERT_TRUE(check(tokens[4], CLOSE_PAREN, ")", 1));
    ASSERT_TRUE(check(tokens[5], OPEN_BRACE, "{", 1));
    ASSERT_TRUE(check(tokens[6], KEYWORD_RETURN, "return", 2));
    ASSERT_TRUE(check(tokens[7], OP_COMPLEMENT, "~", 2));
    ASSERT_TRUE(check(tokens[8], OPEN_PAREN, "(", 2));
    ASSERT_TRUE(check(tokens[9], OP_DECREMENT, "--", 2));
    ASSERT_TRUE(check(tokens[10], OP_NEGATE, "-", 2));
    ASSERT_TRUE(check(tokens[11], CONSTANT_INT, "2", 2));
    ASSERT_TRUE(check(tokens[12], CLOSE_PAREN, ")", 2));
    ASSERT_TRUE(check(tokens[13], SEMICOLON, ";", 2));
    ASSERT_TRUE(check(tokens[14], CLOSE_BRACE, "}", 3));
    ASSERT_TRUE(check(tokens[15], END, "END", 3));
}