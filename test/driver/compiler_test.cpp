#include "args.hpp"
#include "compiler.hpp"

#include <filesystem>
#include <gtest/gtest.h>

using wacc::driver::DriverArgs;
using wacc::driver::runCompiler;

using std::string;

class CompilerTest : public testing::Test {
protected:
    static void TearDownTestSuite() {
        auto directory = std::filesystem::directory_iterator{testSampleRoot};
        for (auto& file : directory) {
            const string& path = file.path();
            if (path.ends_with(".i") || path.ends_with(".s")) {
                std::filesystem::remove(path);
            }
        }
    }

    static constexpr string testSampleRoot = "test/test_sample";
    string testSourceFile = std::format("{}/test_file.c", testSampleRoot);
    string testPrepFile = std::format("{}/test_file.i", testSampleRoot);

    DriverArgs gccArgs{false, false, false, testSourceFile, "gcc"};
    DriverArgs clangArgs{false, false, false, testSourceFile, "clang"};
};

TEST_F(CompilerTest, throwOnEmpty) {
    // ARRANGE
    const auto args = DriverArgs{};
    string preprocessed{};
    string error{};

    // ACT
    try {
        runCompiler(preprocessed, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("preprocessed file does not exist:"));
}

TEST_F(CompilerTest, throwOnNonExistentFile) {
    // ARRANGE
    const auto args = DriverArgs{};
    string preprocessed{"invalid/file.c"};
    string error{};

    // ACT
    try {
        runCompiler(preprocessed, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("preprocessed file does not exist:"));
}

TEST_F(CompilerTest, throwOnInvalidExtension) {
    // ARRANGE
    const auto args = DriverArgs{};
    string preprocessed{testSampleRoot};
    string error{};

    // ACT
    try {
        runCompiler(preprocessed, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("must end in .i"));
}
