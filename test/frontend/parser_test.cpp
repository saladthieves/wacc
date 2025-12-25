#include "ast.hpp"
#include "base_test.hpp"
#include "formatters.hpp"
#include "matchers.hpp"
#include "parser.hpp"
#include "source.hpp"
#include "token.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <print>
#include <string>
#include <vector>

using enum wacc::front::token::TokenType;
using enum wacc::front::ast::AstUnaryOpType;
using enum wacc::front::ast::AstBinaryOpType;

using wacc::front::parse::Parser;
using wacc::front::src::Source;
using wacc::front::token::Token;
using wacc::test::fmt::formatAstExpr;

using namespace wacc::front::ast;
using namespace wacc::test::match;

using std::make_unique;
using std::pair;
using std::string;
using std::string_view;
using std::vector;

using Tokens = vector<Token>;

class ParserTest : public testing::Test, public wacc::test::base::BaseTest {};

TEST_F(ParserTest, parseThrowOnEmpty) {
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

TEST_F(ParserTest, parseMalformedExpression) {
    // ARRANGE
    const auto tests = vector<string>{
        // clang-format off
        "int main(void) { return; }",      
        "int main(void) { return (); }",
        "int main(void) { return (()); }", 
        "int main(void) { return -; }",
        "int main(void) { return ~; }",    
        "int main(void) { return -(); }",
        "int main(void) { return ~(); }",  
        "int main(void) { return --15; }",
        "int main(void) { return 1 + ; }",
        "int main(void) { return 2 - /; }",
        "int main(void) { return 8 % --; }",
        "int main(void) { return -1 / --; }",
        "int main(void) { return (2 *); }",
        "int main(void) { return ((8 %); }",
        // clang-format on
    };

    for (const auto& test : tests) {
        // ACT
        auto parser = getParser(test);
        string error{};

        try {
            parser.parse();
        } catch (const std::runtime_error& ex) {
            error = ex.what();
        }

        // ASSERT
        ASSERT_FALSE(error.empty());
        ASSERT_TRUE(error.contains("ParserError"));
        ASSERT_TRUE(error.contains("malformed") || error.contains("Expected") ||
                    error.contains("Cannot parse"));
    }
}

TEST_F(ParserTest, parseProgram) {
    // ARRANGE
    const auto source =
        R"(int main(void) {
               return 3 + 9 * 8 - 7 / 1 * 3;
           }
        )";

    auto parser = getParser(source);

    // ACT
    auto ptr = parser.parse();

    // ASSERT
    const auto& body = matchAstProg(ptr);
    matchAstReturn(body, [](auto& expr) {
        matchAstBinary(expr, [](auto& op, auto& left, auto& right) {
            ASSERT_EQ(op, AstBinaryOpType::BINARY_SUBTRACT);
            matchAstBinary(left, [](auto& op, auto& left, auto& right) {
                ASSERT_EQ(op, AstBinaryOpType::BINARY_ADD);
                matchAstConstInt(left, 3);
                matchAstBinary(right, [](auto& op, auto& left, auto& right) {
                    ASSERT_EQ(op, AstBinaryOpType::BINARY_MULTIPLY);
                    matchAstConstInt(left, 9);
                    matchAstConstInt(right, 8);
                });
            });
            matchAstBinary(right, [](auto& op, auto& left, auto& right) {
                ASSERT_EQ(op, AstBinaryOpType::BINARY_MULTIPLY);
                matchAstBinary(left, [](auto& op, auto& left, auto& right) {
                    ASSERT_EQ(op, AstBinaryOpType::BINARY_DIVIDE);
                    matchAstConstInt(left, 7);
                    matchAstConstInt(right, 1);
                });
                matchAstConstInt(right, 3);
            });
        });
    });
}

TEST_F(ParserTest, parseConstant) {
    // ARRANGE
    const auto tests = vector<pair<string, string>>{
        {"int main(void) { return 0; }",                      "0" },
        {"int main(void) { return 15; }",                     "15"},
        {"int main(void) { return 23; }",                     "23"},
        {"int main(void) { return (15); }",                   "15"},
        {"int main(void) { return ((33)); }",                 "33"},
        {"int main(void) { return (((8))); }",                "8" },
        {"int main(void) { return ((((((((((15)))))))))); }", "15"},
    };

    for (const auto& pair : tests) {
        auto parser = getParser(pair.first);
        const auto& content = pair.second;

        // ACT
        auto ptr = parser.parse();

        // ASSERT
        const auto& body = matchAstProg(ptr);
        matchAstReturn(body, [&](auto& expr) {
            string data = formatAstExpr(expr);
            ASSERT_STREQ(data.c_str(), content.c_str());
        });
    }
}

TEST_F(ParserTest, parseUnary) {
    // ARRANGE
    const auto tests = vector<pair<string, string>>{
        {"int main(void) { return -1; }",          "[-1]"               },
        {"int main(void) { return -23; }",         "[-23]"              },
        {"int main(void) { return -42; }",         "[-42]"              },
        {"int main(void) { return ~6; }",          "[~6]"               },
        {"int main(void) { return ~88; }",         "[~88]"              },
        {"int main(void) { return ~90; }",         "[~90]"              },
        {"int main(void) { return -~1; }",         "[-[~1]]"            },
        {"int main(void) { return ~-22; }",        "[~[-22]]"           },
        {"int main(void) { return ~~40; }",        "[~[~40]]"           },
        {"int main(void) { return -(~55); }",      "[-[~55]]"           },
        {"int main(void) { return ~((-31)); }",    "[~[-31]]"           },
        {"int main(void) { return ~~(-(~~~9)); }", "[~[~[-[~[~[~9]]]]]]"},
    };

    for (const auto& pair : tests) {
        auto parser = getParser(pair.first);
        const auto& content = pair.second;

        // ACT
        auto ptr = parser.parse();

        // ASSERT
        const auto& body = matchAstProg(ptr);
        matchAstReturn(body, [&](auto& expr) {
            string data = formatAstExpr(expr);
            ASSERT_STREQ(data.c_str(), content.c_str());
        });
    }
}

TEST_F(ParserTest, parseBinaryBasic) {
    // ARRANGE
    const auto tests = vector<pair<string, string>>{
        {"int main(void) { return 1 + 1; }",       "[1 + 1]"       },
        {"int main(void) { return 3 - 2; }",       "[3 - 2]"       },
        {"int main(void) { return 11 * 5; }",      "[11 * 5]"      },
        {"int main(void) { return 8 / 2; }",       "[8 / 2]"       },
        {"int main(void) { return 11 % 3; }",      "[11 % 3]"      },
        {"int main(void) { return -3 + 5; }",      "[[-3] + 5]"    },
        {"int main(void) { return 8 - ~9; }",      "[8 - [~9]]"    },
        {"int main(void) { return -72 - -9; }",    "[[-72] - [-9]]"},
        {"int main(void) { return (~70) * (3); }", "[[~70] * 3]"   },
    };

    for (const auto& pair : tests) {
        auto parser = getParser(pair.first);
        const auto& content = pair.second;

        // ACT
        auto ptr = parser.parse();

        // ASSERT
        const auto& body = matchAstProg(ptr);
        matchAstReturn(body, [&](auto& expr) {
            string data = formatAstExpr(expr);
            ASSERT_STREQ(data.c_str(), content.c_str());
        });
    }
}

TEST_F(ParserTest, parseBinaryAssoc) {
    // ARRANGE
    const auto tests = vector<pair<string, string>>{
        {"int main(void) { return 1 + 2 + 3; }",         "[[1 + 2] + 3]"      },
        {"int main(void) { return 4 + 5 - 6 + 7; }",     "[[[4 + 5] - 6] + 7]"},
        {"int main(void) { return 1 + (3 - 5); }",       "[1 + [3 - 5]]"      },
        {"int main(void) { return 0 + (2 + 4) - 9; }",   "[[0 + [2 + 4]] - 9]"},
        {"int main(void) { return (3 - (5 + 0)) - 7; }", "[[3 - [5 + 0]] - 7]"},
        {"int main(void) { return 2 + ((3) + 9); }",     "[2 + [3 + 9]]"      },
        {"int main(void) { return 1 * 3; }",             "[1 * 3]"            },
        {"int main(void) { return 2 / -9; }",            "[2 / [-9]]"         },
        {"int main(void) { return ~13 % (-~7); }",       "[[~13] % [-[~7]]]"  },
    };

    for (const auto& pair : tests) {
        auto parser = getParser(pair.first);
        const auto& content = pair.second;

        // ACT
        auto ptr = parser.parse();

        // ASSERT
        const auto& body = matchAstProg(ptr);
        matchAstReturn(body, [&](auto& expr) {
            string data = formatAstExpr(expr);
            ASSERT_STREQ(data.c_str(), content.c_str());
        });
    }
}

TEST_F(ParserTest, parseBinPrecedence) {
    // ARRANGE
    const auto tests = vector<pair<string, string>>{
        // clang-format off
        {"int main(void) { return 1 + 2 * 3; }", "[1 + [2 * 3]]" },
        {"int main(void) { return 1 * 2 - 3; }", "[[1 * 2] - 3]"},
        {"int main(void) { return ~5 * 4 - -8; }", "[[[~5] * 4] - [-8]]"},
        {"int main(void) { return (-16) % (~4 + ~~8); }", "[[-16] % [[~4] + [~[~8]]]]"},
        {"int main(void) { return 1 * 2 - 3 * (4 + 5); }", "[[1 * 2] - [3 * [4 + 5]]]"},
        {"int main(void) { return 1 * 2 + 3 / 4;}", "[[1 * 2] + [3 / 4]]"},
        {"int main(void) { return 3 + 9 * 8 - 7 / 1 * 3; }", "[[3 + [9 * 8]] - [[7 / 1] * 3]]"},
        // clang-format on
    };

    for (const auto& pair : tests) {
        auto parser = getParser(pair.first);
        const auto& expected = pair.second;

        // ACT
        auto ptr = parser.parse();

        // ASSERT
        const auto& body = matchAstProg(ptr);
        matchAstReturn(body, [&](auto& expr) {
            string actual = formatAstExpr(expr);
            ASSERT_STREQ(actual.c_str(), expected.c_str());
        });
    }
}