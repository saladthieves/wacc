#include <filesystem>
#include <gtest/gtest.h>
#include <print>
#include <stdexcept>

#include "args.hpp"
#include "preprocessor.hpp"

using wacc::driver::DriverArgs;
using wacc::driver::runPreprocessor;

using std::string;
using std::filesystem::exists;

class PreprocessorTest : public testing::Test {
protected:
    static void TearDownTestSuite() {
        auto directory = std::filesystem::directory_iterator{testSampleRoot};
        for (auto& file : directory) {
            const string& path = file.path();
            if (path.ends_with(".i")) {
                std::filesystem::remove(path);
            }
        }
    }

    static constexpr string testSampleRoot = "test/test_sample";
    string testSourceFile = std::format("{}/test_file.c", testSampleRoot);
};

TEST_F(PreprocessorTest, throwOnEmptyPath) {
    // ARRANGE
    const auto args = DriverArgs{};
    string error{};

    // ACT
    try {
        runPreprocessor(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("source file does not exist:"));
}

TEST_F(PreprocessorTest, throwOnNonExistentFile) {
    // ARRANGE
    const auto args =
        DriverArgs{false, false, false, "test/test_sample/invalid.c"};
    string error{};

    // ACT
    try {
        runPreprocessor(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("source file does not exist:"));
}

TEST_F(PreprocessorTest, throwOnInvalidExtension) {
    // ARRANGE
    const auto args = DriverArgs{false, false, false, testSampleRoot};
    string error{};

    // ACT
    try {
        runPreprocessor(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("must end in .c"));
}

TEST_F(PreprocessorTest, throwOnInvalidCompiler) {
    // ARRANGE
    const auto args =
        DriverArgs{false, false, false, testSourceFile, "unknown-compiler"};
    string error{};

    // ACT
    try {
        runPreprocessor(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("Running ["));
    ASSERT_TRUE(error.contains("] failed: [exitCode: "));
}

TEST_F(PreprocessorTest, runPreprocessorGcc) {
    // ARRANGE
    const auto args = DriverArgs{false, false, false, testSourceFile, "gcc"};
    string error{};
    string outputFile{};
    // ACT
    try {
        outputFile = runPreprocessor(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(exists(testSourceFile));
    ASSERT_TRUE(exists(outputFile));
    ASSERT_TRUE(outputFile.ends_with(".i"));
}

TEST_F(PreprocessorTest, runPreprocessorClang) {
    // ARRANGE
    const auto args = DriverArgs{false, false, false, testSourceFile, "clang"};
    string error{};
    string outputFile{};

    // ACT
    try {
        outputFile = runPreprocessor(args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(exists(testSourceFile));
    ASSERT_TRUE(outputFile.ends_with(".i"));
}