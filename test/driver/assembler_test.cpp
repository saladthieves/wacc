#include "args.hpp"
#include "assembler.hpp"
#include "compiler.hpp"
#include "preprocessor.hpp"

#include <filesystem>
#include <gtest/gtest.h>

using wacc::driver::DriverArgs;
using wacc::driver::runAssembler;
using wacc::driver::runCompiler;
using wacc::driver::runPreprocessor;

using std::string;
using std::filesystem::exists;

class AssemblerTest : public testing::Test {
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

    DriverArgs gccArgs{false, false, false, testSourceFile, "gcc"};
    DriverArgs clangArgs{false, false, false, testSourceFile, "clang"};
};

TEST_F(AssemblerTest, throwOnEmpty) {
    // ARRANGE
    const auto args = DriverArgs{};
    string assembly{};
    string error{};

    // ACT
    try {
        runAssembler(assembly, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("assembly file does not exist:"));
}

TEST_F(AssemblerTest, throwOnNonExistentFile) {
    // ARRANGE
    const auto args = DriverArgs{};
    string assembly{"invalid/file.c"};
    string error{};

    // ACT
    try {
        runAssembler(assembly, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("assembly file does not exist:"));
}

TEST_F(AssemblerTest, throwOnInvalidExtension) {
    // ARRANGE
    const auto args = DriverArgs{};
    string assembly{testSampleRoot};
    string error{};

    // ACT
    try {
        runAssembler(assembly, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("must end in .s"));
}

TEST_F(AssemblerTest, throwOnInvalidCompiler) {
    // ARRANGE
    const auto args =
        DriverArgs{false, false, false, testSourceFile, "unknown-compiler"};
    string error{};

    // ACT
    try {
        runPreprocessor(gccArgs);
        runCompiler(testPrepFile, gccArgs);
        runAssembler(testAsmFile, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("Running ["));
    ASSERT_TRUE(error.contains("] failed: [exitCode: "));
}

TEST_F(AssemblerTest, runCompilerClang) {
    // ARRANGE
    string error{};
    string preprocessed{};
    string assembly{};
    string output{std::format("{}/test_file", testSampleRoot)};

    // ACT
    try {
        preprocessed = runPreprocessor(clangArgs);
        assembly = runCompiler(preprocessed, clangArgs);
        runAssembler(assembly, clangArgs);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_FALSE(exists(preprocessed));
    ASSERT_FALSE(exists(assembly));
    ASSERT_TRUE(exists(output));
}

TEST_F(AssemblerTest, runCompilerGcc) {
    // ARRANGE
    string error{};
    string preprocessed{};
    string assembly{};
    string output{std::format("{}/test_file", testSampleRoot)};

    // ACT
    try {
        preprocessed = runPreprocessor(gccArgs);
        assembly = runCompiler(preprocessed, gccArgs);
        runAssembler(assembly, gccArgs);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_FALSE(exists(preprocessed));
    ASSERT_FALSE(exists(assembly));
    ASSERT_TRUE(exists(output));
}