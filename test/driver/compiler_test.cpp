#include "args.hpp"
#include "compiler.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>

using wacc::driver::DriverArgs;
using wacc::driver::runCompiler;

using wacc::test::utils::samples::cleanUpSamples;
using wacc::test::utils::samples::rootFolder;
using wacc::test::utils::samples::sampleSource;

using std::string;

class CompilerTest : public testing::Test {
protected:
    static void TearDownTestSuite() {
        cleanUpSamples([](const auto& path) {
            return path.ends_with(".i") || path.ends_with(".s");
        });
    }

    DriverArgs gccArgs{false, false, false, false, sampleSource, "gcc"};
    DriverArgs clangArgs{false, false, false, false, sampleSource, "clang"};
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
    string preprocessed{rootFolder};
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
