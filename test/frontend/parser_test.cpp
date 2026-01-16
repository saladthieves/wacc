#include "ast.hpp"
#include "ast_formatters.hpp"
#include "ast_matchers.hpp"
#include "base_test.hpp"
#include "parser.hpp"
#include "source.hpp"
#include "token.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <print>
#include <string>
#include <vector>

using enum wacc::front::token::TokenType;
using enum wacc::front::ast::AstUnary::Type;
using enum wacc::front::ast::AstBinary::Type;

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
        "{ return; }",      
        "{ return (); }",
        "{ return (()); }", 
        "{ return -; }",
        "{ return !; }",
        "{ return ~; }",    
        "{ return -(); }",
        "{ return !(); }",
        "{ return ~(); }",  
        "{ return --15; }",
        "{ return 1 + ; }",
        "{ return 2 - /; }",
        "{ return 8 % --; }",
        "{ return -1 / --; }",
        "{ return !23 / --; }",
        "{ return (2 *); }",
        "{ return ((8 %); }",
        "{ return >>; }",
        "{ return 2 <<; }",
        "{ return & 2 ^ 33 | 9; }",
        "{ return 2 &&; }",
        "{ return || 89; }",
        "{ return 2!; }",
        "{ return 3 ==; }",
        "{ return != 8; }",
        "{ return (3) > ; }",
        "{ return (22) >= ; }",
        "{ return < - (-8); }",
        "{ return <= (-28); }",
        // clang-format on
    };

    for (const auto& test : tests) {
        // ACT
        const auto code = std::format("int main(void) {}", test);
        auto parser = getParser(code);
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

TEST_F(ParserTest, parseConstant) {
    // ARRANGE
    const auto tests = vector<pair<string, string>>{
        {"{ return 0; }",                      "0" },
        {"{ return 15; }",                     "15"},
        {"{ return 23; }",                     "23"},
        {"{ return (15); }",                   "15"},
        {"{ return ((33)); }",                 "33"},
        {"{ return (((8))); }",                "8" },
        {"{ return ((((((((((15)))))))))); }", "15"},
    };

    for (const auto& pair : tests) {
        const auto code = std::format("int main(void) {}", pair.first);
        auto parser = getParser(code);
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
        {"{ return -1; }",          "[-1]"               },
        {"{ return -23; }",         "[-23]"              },
        {"{ return !42; }",         "[!42]"              },
        {"{ return -42; }",         "[-42]"              },
        {"{ return ~6; }",          "[~6]"               },
        {"{ return ~88; }",         "[~88]"              },
        {"{ return ~90; }",         "[~90]"              },
        {"{ return -~1; }",         "[-[~1]]"            },
        {"{ return -!10; }",        "[-[!10]]"           },
        {"{ return ~-22; }",        "[~[-22]]"           },
        {"{ return ~~40; }",        "[~[~40]]"           },
        {"{ return ~!(!78); }",     "[~[![!78]]]"        },
        {"{ return -(~55); }",      "[-[~55]]"           },
        {"{ return ~((-31)); }",    "[~[-31]]"           },
        {"{ return ~~(-(~~~9)); }", "[~[~[-[~[~[~9]]]]]]"},
    };

    for (const auto& pair : tests) {
        const auto code = std::format("int main(void) {}", pair.first);
        auto parser = getParser(code);

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
        {"{ return 1 + 1; }",            "[1 + 1]"                 },
        {"{ return 3 - 2; }",            "[3 - 2]"                 },
        {"{ return 11 * 5; }",           "[11 * 5]"                },
        {"{ return 8 / 2; }",            "[8 / 2]"                 },
        {"{ return 11 % 3; }",           "[11 % 3]"                },
        {"{ return -3 + 5; }",           "[[-3] + 5]"              },
        {"{ return 8 - ~9; }",           "[8 - [~9]]"              },
        {"{ return -72 - -9; }",         "[[-72] - [-9]]"          },
        {"{ return (~70) * (3); }",      "[[~70] * 3]"             },
        {"{ return 2 & 3; }",            "[2 & 3]"                 },
        {"{ return 3 | 4; }",            "[3 | 4]"                 },
        {"{ return 15 ^ 5; }",           "[15 ^ 5]"                },
        {"{ return 16 << 2; }",          "[16 << 2]"               },
        {"{ return 18 >> 1; }",          "[18 >> 1]"               },
        {"{ return 2 && 4; }",           "[2 && 4]"                },
        {"{ return 33 || 0; }",          "[33 || 0]"               },
        {"{ return 15 < 15; }",          "[15 < 15]"               },
        {"{ return (15) <= -28; }",      "[15 <= [-28]]"           },
        {"{ return (!34) > ~43; }",      "[[!34] > [~43]]"         },
        {"{ return ((-15)) >= !-!~8; }", "[[-15] >= [![-[![~8]]]]]"},
    };

    for (const auto& pair : tests) {
        const auto code = std::format("int main(void) {}", pair.first);
        auto parser = getParser(code);
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
        {"{ return 1 + 2 + 3; }",             "[[1 + 2] + 3]"                },
        {"{ return 4 + 5 - 6 + 7; }",         "[[[4 + 5] - 6] + 7]"          },
        {"{ return 1 + (3 - 5); }",           "[1 + [3 - 5]]"                },
        {"{ return 0 + (2 + 4) - 9; }",       "[[0 + [2 + 4]] - 9]"          },
        {"{ return (3 - (5 + 0)) - 7; }",     "[[3 - [5 + 0]] - 7]"          },
        {"{ return 2 + ((3) + 9); }",         "[2 + [3 + 9]]"                },
        {"{ return 1 * 3; }",                 "[1 * 3]"                      },
        {"{ return 2 / -9; }",                "[2 / [-9]]"                   },
        {"{ return ~13 % (-~7); }",           "[[~13] % [-[~7]]]"            },
        {"{ return 3 << 2 << 1; }",           "[[3 << 2] << 1]"              },
        {"{ return -15 >> 8 << ~2; }",        "[[[-15] >> 8] << [~2]]"       },
        {"{ return -9 & ~3 & -27; }",         "[[[-9] & [~3]] & [-27]]"      },
        {"{ return ~~2 ^ 3 ^ -(-48); }",      "[[[~[~2]] ^ 3] ^ [-[-48]]]"   },
        {"{ return 2 < 3 >= 28 <= 9 < 15; }", "[[[[2 < 3] >= 28] <= 9] < 15]"},
        {"{ return ~25 == !99 != -87; }",     "[[[~25] == [!99]] != [-87]]"  },
    };

    for (const auto& pair : tests) {
        const auto code = std::format("int main(void) {}", pair.first);
        auto parser = getParser(code);
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
        {"{ return 1 + 2 * 3; }",                 "[1 + [2 * 3]]"             },
        {"{ return 1 * 2 - 3; }",                 "[[1 * 2] - 3]"             },
        {"{ return ~5 * 4 - -8; }",               "[[[~5] * 4] - [-8]]"       },
        {"{ return (-16) % (~4 + ~~8); }",        "[[-16] % [[~4] + [~[~8]]]]"},
        {"{ return 1 * 2 - 3 * (4 + 5); }",       "[[1 * 2] - [3 * [4 + 5]]]" },
        {"{ return 1 * 2 + 3 / 4;}",              "[[1 * 2] + [3 / 4]]"       },
        {"{ return 3 + 9 * 8 - 7 / 1 * 3; }",     "[[3 + [9 * 8]] - [[7 / 1] * 3]]" },
        {"{ return 2 % 5 | 9 << 3; }",            "[[2 % 5] | [9 << 3]]"      },
        {"{ return 8 - ~3 << 2 & -9 * 8; }",      "[[[8 - [~3]] << 2] & [[-9] * 8]]"      },
        {"{ return ~33 + 7 * -9 ^ 9 & 3 >> 2; }", "[[[~33] + [7 * [-9]]] ^ [9 & [3 >> 2]]]" },
        {"{ return 2 < 8 == 15 <= 20; }",         "[[2 < 8] == [15 <= 20]]"},
        {"{ return -15 != 22 >= !9; }",           "[[-15] != [22 >= [!9]]]"},
        {"{ return -3 * 9 != ~22 > (8 + 15); }",  "[[[-3] * 9] != [[~22] > [8 + 15]]]"},
        {"{ return (8 & 3) != (15 << 8) * 9; }",  "[[8 & 3] != [[15 << 8] * 9]]"},
        // clang-format on
    };

    for (const auto& pair : tests) {
        const auto code = std::format("int main(void) {}", pair.first);
        auto parser = getParser(code);
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