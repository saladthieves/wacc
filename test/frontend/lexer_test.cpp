#include "base_test.hpp"
#include "lexer.hpp"
#include "token.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using enum wacc::front::token::TokenType;
using wacc::front::token::Token;
using wacc::front::token::TokenType;

using std::pair;
using std::string;
using std::vector;

class LexerTest : public testing::Test, public wacc::test::base::BaseTest {};

TEST_F(LexerTest, scanEmpty) {
    // ARRANGE
    auto lexer = getLexer("");

    // ACT
    auto ptr = lexer.scan();

    // ASSERT
    auto tokens = *ptr;
    ASSERT_FALSE(tokens.empty());
    ASSERT_EQ(tokens.front().type, END);
    ASSERT_EQ(tokens.front().lineNo, 1);
    ASSERT_TRUE(tokens.front().value == "END");
}

TEST_F(LexerTest, scanEmptyText) {
    // ARRANGE
    const auto source = "           ";
    auto lexer = getLexer(source);

    // ACT
    auto ptr = lexer.scan();

    // ASSERT
    auto tokens = *ptr;
    ASSERT_FALSE(tokens.empty());
    ASSERT_EQ(tokens.front().type, END);
    ASSERT_EQ(tokens.front().lineNo, 1);
    ASSERT_TRUE(tokens.front().value == "END");
}

TEST_F(LexerTest, scanNumberConstant) {
    // ARRANGE
    auto tests = vector<pair<string, string>>{
        {"29",        "29" },
        {"    38",    "38" },
        {"65    ",    "65" },
        {"  100    ", "100"},
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = getLexer(test.first);
        auto ptr = lexer.scan();

        // ASSERT
        auto tokens = *ptr;
        ASSERT_EQ(tokens.size(), 2);

        ASSERT_EQ(tokens.front().lineNo, 1);
        ASSERT_EQ(tokens.front().type, LITERAL_INT);
        ASSERT_TRUE(tokens.front().value == test.second);

        ASSERT_EQ(tokens.back().lineNo, 1);
        ASSERT_EQ(tokens.back().type, END);
        ASSERT_TRUE(tokens.back().value == "END");
    }
}

TEST_F(LexerTest, scanNumberConstantInvalidAlpha) {
    // ARRANGE
    auto tests = vector<string>{"29s",  "38a",  "2x92",  "28ss",
                                "1xx2", "115_", "3_3_3", "123ret"};

    for (const auto& source : tests) {
        // ACT
        auto lexer = getLexer(source);
        string error{};

        try {
            lexer.scan();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
        ASSERT_TRUE(error.contains("character in number literal"));
    }
}

TEST_F(LexerTest, scanNumberConstantInvalidChar) {
    // ARRANGE
    auto tests = vector<string>{"29'", "38@", "2`92", "28'", "1\"2"};

    for (const auto& source : tests) {
        // ACT
        auto lexer = getLexer(source);
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

TEST_F(LexerTest, scanIdentifier) {
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
        auto lexer = getLexer(test.first);
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

TEST_F(LexerTest, scanIdentifierInvalid) {
    // ARRANGE
    auto tests = vector<string>{"some@ne", "may?be", "no$w",   "fi#st",
                                "noun`s",  "adv'",   "sha``e!"};

    for (const auto& source : tests) {
        auto lexer = getLexer(source);
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

TEST_F(LexerTest, scanKeywords) {
    // ARRANGE
    auto tests = vector<pair<string, TokenType>>{
        {"int",    KEYWORD_INT   },
        {"return", KEYWORD_RETURN},
        {"void",   KEYWORD_VOID  },
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = getLexer(test.first);
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

TEST_F(LexerTest, scanKeywordsInvalid) {
    // ARRANGE
    auto tests = vector<string>{
        {"integer"},
        {"returned"},
        {"voids"},
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = getLexer(test);
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

TEST_F(LexerTest, scanSingleToken) {
    // ARRANGE
    auto tests = vector<pair<string, TokenType>>{
        {"(",  OPEN_PAREN       },
        {")",  CLOSE_PAREN      },
        {"{",  OPEN_BRACE       },
        {"}",  CLOSE_BRACE      },
        {";",  SEMICOLON        },
        {"~",  OP_BIT_COMPLEMENT},
        {"-",  OP_NEGATE        },
        {"--", OP_DECREMENT     },
        {"&",  OP_BIT_AND       },
        {"|",  OP_BIT_OR        },
        {"^",  OP_BIT_XOR       },
        {"<<", OP_BIT_LSH       },
        {">>", OP_BIT_RSH       },
        {"<",  OP_LESS_THAN     },
        {"<=", OP_LESS_EQUAL    },
        {">",  OP_GREATER_THAN  },
        {">=", OP_GREATER_EQUAL },
        {"!",  OP_LOG_NOT       },
        {"&&", OP_LOG_AND       },
        {"||", OP_LOG_OR        },
        {"==", OP_EQUAL         },
        {"!=", OP_NOT_EQUAL     },
        {"=",  OP_ASSIGN        },
    };

    for (const auto& test : tests) {
        // ACT
        auto lexer = getLexer(test.first);
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

TEST_F(LexerTest, scanSingleTokenInvalid) {
    // ARRANGE
    auto tests = vector<string>{"@", "?", "\\", "`", "``", "#"};

    for (const auto& test : tests) {
        // ACT
        auto lexer = getLexer(test);
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

TEST_F(LexerTest, scanSource) {
    // ARRANGE
    const auto source =
        R"(int main(void) {
            return (-1 + 2) * ~3 - 4 / 5 % (--6) < > <= >= << >> &| ^!&& ||
            == != = ;
        })";

    auto lexer = getLexer(source);
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
    ASSERT_EQ(tokens.size(), 43);
    // clang-format off
    ASSERT_TRUE(check(tokens[0],  KEYWORD_INT,       "int",    1));
    ASSERT_TRUE(check(tokens[1],  IDENTIFIER,        "main",   1));
    ASSERT_TRUE(check(tokens[2],  OPEN_PAREN,        "(",      1));
    ASSERT_TRUE(check(tokens[3],  KEYWORD_VOID,      "void",   1));
    ASSERT_TRUE(check(tokens[4],  CLOSE_PAREN,       ")",      1));
    ASSERT_TRUE(check(tokens[5],  OPEN_BRACE,        "{",      1));
    ASSERT_TRUE(check(tokens[6],  KEYWORD_RETURN,    "return", 2));
    ASSERT_TRUE(check(tokens[7],  OPEN_PAREN,        "(",      2));
    ASSERT_TRUE(check(tokens[8],  OP_NEGATE,         "-",      2));
    ASSERT_TRUE(check(tokens[9],  LITERAL_INT,       "1",      2));
    ASSERT_TRUE(check(tokens[10], OP_ADDITION,       "+",      2));
    ASSERT_TRUE(check(tokens[11], LITERAL_INT,       "2",      2));
    ASSERT_TRUE(check(tokens[12], CLOSE_PAREN,       ")",      2));
    ASSERT_TRUE(check(tokens[13], OP_MULTIPLY,       "*",      2));
    ASSERT_TRUE(check(tokens[14], OP_BIT_COMPLEMENT, "~",      2));
    ASSERT_TRUE(check(tokens[15], LITERAL_INT,       "3",      2));
    ASSERT_TRUE(check(tokens[16], OP_NEGATE,         "-",      2));
    ASSERT_TRUE(check(tokens[17], LITERAL_INT,       "4",      2));
    ASSERT_TRUE(check(tokens[18], OP_DIVIDE,         "/",      2));
    ASSERT_TRUE(check(tokens[19], LITERAL_INT,       "5",      2));
    ASSERT_TRUE(check(tokens[20], OP_REMAINDER,      "%",      2));
    ASSERT_TRUE(check(tokens[21], OPEN_PAREN,        "(",      2));
    ASSERT_TRUE(check(tokens[22], OP_DECREMENT,      "--",     2));
    ASSERT_TRUE(check(tokens[23], LITERAL_INT,       "6",      2));
    ASSERT_TRUE(check(tokens[24], CLOSE_PAREN,       ")",      2));
    ASSERT_TRUE(check(tokens[25], OP_LESS_THAN,      "<",      2));
    ASSERT_TRUE(check(tokens[26], OP_GREATER_THAN,   ">",      2));
    ASSERT_TRUE(check(tokens[27], OP_LESS_EQUAL,     "<=",     2));
    ASSERT_TRUE(check(tokens[28], OP_GREATER_EQUAL,  ">=",     2));
    ASSERT_TRUE(check(tokens[29], OP_BIT_LSH,        "<<",     2));
    ASSERT_TRUE(check(tokens[30], OP_BIT_RSH,        ">>",     2));
    ASSERT_TRUE(check(tokens[31], OP_BIT_AND,        "&",      2));
    ASSERT_TRUE(check(tokens[32], OP_BIT_OR,         "|",      2));
    ASSERT_TRUE(check(tokens[33], OP_BIT_XOR,        "^",      2));
    ASSERT_TRUE(check(tokens[34], OP_LOG_NOT,        "!",      2));
    ASSERT_TRUE(check(tokens[35], OP_LOG_AND,        "&&",     2));
    ASSERT_TRUE(check(tokens[36], OP_LOG_OR,         "||",     2));
    ASSERT_TRUE(check(tokens[37], OP_EQUAL,          "==",     3));
    ASSERT_TRUE(check(tokens[38], OP_NOT_EQUAL,      "!=",     3));
    ASSERT_TRUE(check(tokens[39], OP_ASSIGN,         "=",      3));
    ASSERT_TRUE(check(tokens[40], SEMICOLON,         ";",      3));
    ASSERT_TRUE(check(tokens[41], CLOSE_BRACE,       "}",      4));
    ASSERT_TRUE(check(tokens[42], END,               "END",    4));
    // clang-format on
}