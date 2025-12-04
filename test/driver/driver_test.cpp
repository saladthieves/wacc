#include "cmd.hpp"
#include "driver.hpp"

#include <filesystem>
#include <gtest/gtest.h>

using wacc::driver::runDriver;
using wacc::utils::runCommand;

using std::string;
using std::vector;
using std::filesystem::exists;

class DriverTest : public testing::Test {
protected:
    static void TearDownTestSuite() {
        auto directory = std::filesystem::directory_iterator{testSampleRoot};
        for (auto& file : directory) {
            const string& path = file.path();
            if (!path.ends_with(".c")) {
                std::filesystem::remove(path);
            }
        }
    }

    static constexpr string testSampleRoot = "test/test_sample";
    string testSourceFile = std::format("{}/test_file.c", testSampleRoot);
    string testPrepFile = std::format("{}/test_file.i", testSampleRoot);
    string testAsmFile = std::format("{}/test_file.s", testSampleRoot);
    string testOutputFile = std::format("{}/test_file", testSampleRoot);
};

TEST_F(DriverTest, runCompiler) {
    using namespace std::string_literals;
    // ARRANGE
    auto args = std::vector{"--lex"s, "--parse"s, "--codegen"s, testSourceFile};

    // ACT
    runDriver(args);

    // ASSERT
    ASSERT_TRUE(exists(testSourceFile));
    ASSERT_TRUE(exists(testOutputFile));
    ASSERT_FALSE(exists(testPrepFile));
    ASSERT_FALSE(exists(testAsmFile));
}

TEST_F(DriverTest, runCompilerExecOutput) {
    using namespace std::string_literals;
    // ARRANGE
    auto args = std::vector{"--lex"s, "--parse"s, "--codegen"s, testSourceFile};

    // ACT
    runDriver(args);

    // ASSERT
    ASSERT_TRUE(exists(testSourceFile));
    ASSERT_TRUE(exists(testOutputFile));

    auto result = runCommand(testOutputFile, {});
    ASSERT_STREQ(result.command.c_str(), testOutputFile.c_str());
    ASSERT_NE(result.exitCode, 0);
    ASSERT_FALSE(result.success);
}