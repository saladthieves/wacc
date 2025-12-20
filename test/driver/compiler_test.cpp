#include "args.hpp"
#include "compiler.hpp"
#include "test_utils.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <stdexcept>

using wacc::driver::CompilerResult;
using wacc::driver::DriverArgs;
using wacc::driver::runCompiler;

using wacc::test::utils::samples::cleanUpSamples;
using wacc::test::utils::samples::sampleAsm;
using wacc::test::utils::samples::sampleCode;
using wacc::test::utils::samples::samplePrep;
using wacc::test::utils::samples::sampleSource;

using std::string;

class CompilerTest : public testing::Test {
protected:
    void TearDown() override {
        cleanUpSamples([](const auto& path) {
            return path.ends_with(".i") || path.ends_with(".s");
        });
    }

    void writeToPrepFile(const string& data) {
        std::ofstream file{samplePrep};
        file << data;
        file.close();
    }

    DriverArgs sampleArgs{false, false, false,        false,
                          false, false, sampleSource, "gcc"};
};

TEST_F(CompilerTest, throwOnNonExistentFile) {
    // ARRANGE
    const auto preprocessed = "invalid.file";
    string error{};

    // ACT
    try {
        runCompiler(preprocessed, sampleArgs);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("provided preprocessed file"));
    ASSERT_TRUE(error.contains("does not exist: "));
}

TEST_F(CompilerTest, throwOnInvalidExtension) {
    // ARRANGE
    const auto preprocessed = sampleSource;
    string error{};

    // ACT
    try {
        runCompiler(preprocessed, sampleArgs);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("The preprocessed file"));
    ASSERT_TRUE(error.contains("must end in .i"));
}

TEST_F(CompilerTest, compileLexerOnly) {
    // ARRANGE
    auto args =
        DriverArgs{true, false, false, false, false, false, sampleSource};
    string source = "main int void";
    writeToPrepFile(source);
    string error{};

    // ACT
    try {
        runCompiler(samplePrep, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
}

TEST_F(CompilerTest, compileParserOnly) {
    // ARRANGE
    auto args =
        DriverArgs{false, true, false, false, false, false, sampleSource};
    writeToPrepFile(sampleCode);
    string error{};

    // ACT
    try {
        runCompiler(samplePrep, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_FALSE(std::filesystem::exists(sampleAsm));
}

TEST_F(CompilerTest, compileTackyOnly) {    // TODO: Fix this
    // ARRANGE
    auto args =
        DriverArgs{false, false, true, false, false, false, sampleSource};
    writeToPrepFile(sampleCode);
    string error{};

    // ACT
    try {
        runCompiler(samplePrep, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_FALSE(std::filesystem::exists(sampleAsm));
}

TEST_F(CompilerTest, compileCodegenOnly) {
    // ARRANGE
    auto args =
        DriverArgs{false, false, false, true, false, false, sampleSource};
    writeToPrepFile(sampleCode);
    string error{};

    // ACT
    try {
        runCompiler(samplePrep, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_FALSE(std::filesystem::exists(sampleAsm));
}

TEST_F(CompilerTest, compile) {
    // ARRANGE
    auto args =
        DriverArgs{false, false, false, false, false, false, sampleSource};
    writeToPrepFile(sampleCode);
    string error{};
    CompilerResult result{};

    // ACT
    try {
        result = runCompiler(samplePrep, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(sampleSource));
    ASSERT_TRUE(std::filesystem::exists(samplePrep));
    ASSERT_TRUE(std::filesystem::exists(sampleAsm));
    ASSERT_TRUE(result.proceed);
    ASSERT_TRUE(result.path.ends_with(sampleAsm));
}