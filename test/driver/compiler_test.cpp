#include "args.hpp"
#include "compiler.hpp"
#include "preprocessor.hpp"

#include <filesystem>
#include <gtest/gtest.h>

using wacc::driver::DriverArgs;
using wacc::driver::runCompiler;
using wacc::driver::runPreprocessor;

using std::string;
using std::filesystem::exists;

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

TEST_F(CompilerTest, throwOnInvalidCompiler) {
    // ARRANGE
    const auto args =
        DriverArgs{false, false, false, testSourceFile, "unknown-compiler"};
    string error{};

    // ACT
    try {
        runPreprocessor(gccArgs);
        runCompiler(testPrepFile, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("Running ["));
    ASSERT_TRUE(error.contains("] failed: [exitCode: "));
}

TEST_F(CompilerTest, runCompilerClang) {
    // ARRANGE
    string error{};
    string preprocessed{};
    string output{};

    // ACT
    try {
        preprocessed = runPreprocessor(clangArgs);
        output = runCompiler(preprocessed, clangArgs);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_FALSE(exists(preprocessed));
    ASSERT_TRUE(exists(output));
    ASSERT_TRUE(output.ends_with(".s"));
}

TEST_F(CompilerTest, runCompilerGcc) {
    // ARRANGE
    string error{};
    string preprocessed{};
    string output{};

    // ACT
    try {
        preprocessed = runPreprocessor(gccArgs);
        output = runCompiler(preprocessed, gccArgs);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_FALSE(exists(preprocessed));
    ASSERT_TRUE(exists(output));
    ASSERT_TRUE(output.ends_with(".s"));
}