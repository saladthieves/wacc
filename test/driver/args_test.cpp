#include "args.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

using wacc::driver::parseDriverArgs;
using wacc::driver::DriverArgs;

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
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("driver arguments"));
}

TEST(ArgsTest, selectLex) {
    // ARRANGE
    auto args = vector<string>{"--lex", "some-path"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_TRUE(arguments.lex);
    ASSERT_FALSE(arguments.parse);
    ASSERT_FALSE(arguments.codegen);
    ASSERT_TRUE(arguments.cleanUp);
    ASSERT_TRUE(arguments.path == "some-path");
}

TEST(ArgsTest, selectParse) {
    // ARRANGE
    auto args = vector<string>{"--parse", "some-path"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_FALSE(arguments.lex);
    ASSERT_TRUE(arguments.parse);
    ASSERT_FALSE(arguments.codegen);
    ASSERT_TRUE(arguments.cleanUp);
    ASSERT_TRUE(arguments.path == "some-path");
}

TEST(ArgsTest, selectCodegen) {
    // ARRANGE
    auto args = vector<string>{"--codegen", "some-path"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_FALSE(arguments.lex);
    ASSERT_FALSE(arguments.parse);
    ASSERT_TRUE(arguments.codegen);
    ASSERT_TRUE(arguments.cleanUp);
    ASSERT_TRUE(arguments.path == "some-path");
}

TEST(ArgsTest, selectSkipCleanup) {
    // ARRANGE
    auto args = vector<string>{"--skip-cleanup", "some-path"};

    // ACT
    const auto arguments = parseDriverArgs(args);

    // ASSERT
    ASSERT_FALSE(arguments.lex);
    ASSERT_FALSE(arguments.parse);
    ASSERT_FALSE(arguments.codegen);
    ASSERT_FALSE(arguments.cleanUp);
    ASSERT_TRUE(arguments.path == "some-path");
}

TEST(ArgsTest, throwOnMultipleFlagsSameKind) {
    // ARRANGE
    auto args = vector<string>{"--lex", "--parse", "some-path"};
    string error{};

    // ACT
    try {
        parseDriverArgs(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("Multiple flags provided"));
}

TEST(ArgsTest, noThrowOnMultipleFlagsDifferentKind) {
    // ARRANGE
    auto args = vector<string>{"--lex", "--skip-cleanup", "some-path"};
    string error{};
    DriverArgs arguments{};

    // ACT
    try {
        arguments = parseDriverArgs(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(arguments.lex);
    ASSERT_FALSE(arguments.parse);
    ASSERT_FALSE(arguments.codegen);
    ASSERT_FALSE(arguments.cleanUp);
    ASSERT_TRUE(arguments.path == "some-path");
}

TEST(ArgsTest, throwOnEmptyPath) {
    // ARRANGE
    auto args = vector<string>{"--lex", "--skip-cleanup"};
    string error{};

    // ACT
    try {
        parseDriverArgs(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("No source path provided."));
}

TEST(ArgsTest, throwOnMultiplePaths) {
    // ARRANGE
    vector<string> args{"--lex", "--skip-cleanup", "file.c", "file2.c"};
    string error;

    try {
        // ACT
        parseDriverArgs(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("than one source path"));
    ASSERT_EQ(args.size(), 2);
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
    ASSERT_TRUE(arguments.cleanUp);
    ASSERT_STREQ(arguments.path.c_str(), "file.c");
    ASSERT_EQ(args.size(), 1);
}