#include "cmd.hpp"

#include <gtest/gtest.h>

using wacc::utils::runCommand;

TEST(CmdTest, runCommand) {
    // ARRANGE
    const auto tool = "echo";

    // ACT
    auto result = runCommand(tool, {});
    
    // ASSERT
    ASSERT_STREQ(result.command.c_str(), tool);
    ASSERT_EQ(result.exitCode, 0);
    ASSERT_TRUE(result.exitCode == 0);
}

TEST(CmdTest, runCommandArgs) {
    // ARRANGE
    const auto tool = "echo";

    // ACT
    auto result = runCommand(tool, {"hello, world!"});
    
    // ASSERT
    ASSERT_STREQ(result.command.c_str(), "echo hello, world!");
    ASSERT_EQ(result.exitCode, 0);
    ASSERT_TRUE(result.success);
}

TEST(CmdTest, runCommandInvalid) {
    // ARRANGE
    const auto tool = "invalid-command";

    // ACT
    auto result = runCommand(tool, {"invalid arg"});
    
    // ASSERT
    ASSERT_STREQ(result.command.c_str(), "invalid-command invalid arg");
    ASSERT_NE(result.exitCode, 0);
    ASSERT_FALSE(result.success);
}