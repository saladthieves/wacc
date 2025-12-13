#include <filesystem>
#include <gtest/gtest.h>
#include <print>
#include <stdexcept>

#include "args.hpp"
#include "preprocessor.hpp"
#include "test_utils.hpp"

using wacc::driver::DriverArgs;
using wacc::driver::runPreprocessor;
using wacc::test::utils::samples::cleanUpSamples;

using wacc::test::utils::samples::rootFolder;
using wacc::test::utils::samples::sampleSource;

using std::string;
using std::filesystem::exists;

class PreprocessorTest : public testing::Test {
protected:
    static void TearDownTestSuite() {
        cleanUpSamples([](const auto& path) { return path.ends_with(".i"); });
    }
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
    const auto args = DriverArgs{false, false, false,
                                 false, false, "test/test_sample/invalid.c"};
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
    const auto args = DriverArgs{false, false, false, false, false, rootFolder};
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
    const auto args = DriverArgs{
        false, false, false, false, false, sampleSource, "unknown-compiler"};
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
    const auto args =
        DriverArgs{false, false, false, false, false, sampleSource, "gcc"};
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
    ASSERT_TRUE(exists(sampleSource));
    ASSERT_TRUE(exists(outputFile));
    ASSERT_TRUE(outputFile.ends_with(".i"));
}

TEST_F(PreprocessorTest, runPreprocessorClang) {
    // ARRANGE
    const auto args =
        DriverArgs{false, false, false, false, false, sampleSource, "clang"};
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
    ASSERT_TRUE(exists(sampleSource));
    ASSERT_TRUE(outputFile.ends_with(".i"));
}