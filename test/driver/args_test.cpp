#include "args.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

using wacc::driver::parseDriverArgs;

using std::string;
using std::vector;

TEST(ArgsTest, throwOnEmptyArgs) {
    // ARRANGE
    auto args = vector<string>{};
    string error;

    try {
        // ACT
        parseDriverArgs(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(!error.empty());
    ASSERT_TRUE(error.contains("driver arguments"));
}

TEST(ArgsTest, throwOnNoPath) {
    // ARRANGE
    auto args = vector<string>{"--lex", "--parse", "--codegen"};
    string error;

    try {
        // ACT
        parseDriverArgs(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(!error.empty());
    ASSERT_TRUE(error.contains("source path provided"));
    ASSERT_TRUE(args.empty());
}

TEST(ArgsTest, throwOnMultiplePaths) {
    // ARRANGE
    vector<string> args{"--lex", "--parse", "--codegen", "file.c", "file2.c"};
    string error;

    try {
        // ACT
        parseDriverArgs(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(!error.empty());
    ASSERT_TRUE(error.contains("than one source path"));
    ASSERT_EQ(args.size(), 2);
}

TEST(ArgsTest, parseArgsFull) {
    // ARRANGE
    auto args = vector<string>{"--lex", "--parse", "--codegen", "file.c"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_TRUE(arguments.lex);
    ASSERT_TRUE(arguments.parse);
    ASSERT_TRUE(arguments.codegen);
    ASSERT_STREQ(arguments.path.c_str(), "file.c");
    ASSERT_EQ(args.size(), 1);
}

TEST(ArgsTest, parseArgsLex) {
    // ARRANGE
    auto args = vector<string>{"--parse", "--codegen", "file.c"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_FALSE(arguments.lex);
    ASSERT_TRUE(arguments.parse);
    ASSERT_TRUE(arguments.codegen);
    ASSERT_STREQ(arguments.path.c_str(), "file.c");
    ASSERT_EQ(args.size(), 1);
}

TEST(ArgsTest, parseArgsParse) {
    // ARRANGE
    auto args = vector<string>{"--lex", "--codegen", "file.c"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_TRUE(arguments.lex);
    ASSERT_FALSE(arguments.parse);
    ASSERT_TRUE(arguments.codegen);
    ASSERT_STREQ(arguments.path.c_str(), "file.c");
    ASSERT_EQ(args.size(), 1);
}

TEST(ArgsTest, parseArgsCodegen) {
    // ARRANGE
    auto args = vector<string>{"--lex", "--parse", "file.c"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_TRUE(arguments.lex);
    ASSERT_TRUE(arguments.parse);
    ASSERT_FALSE(arguments.codegen);
    ASSERT_STREQ(arguments.path.c_str(), "file.c");
    ASSERT_EQ(args.size(), 1);
}

TEST(ArgsTest, parseNoFlags) {
    // ARRANGE
    auto args = vector<string>{"file.c"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_FALSE(arguments.lex);
    ASSERT_FALSE(arguments.parse);
    ASSERT_FALSE(arguments.codegen);
    ASSERT_STREQ(arguments.path.c_str(), "file.c");
    ASSERT_EQ(args.size(), 1);
}
