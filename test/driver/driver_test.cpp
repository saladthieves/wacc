#include "driver.hpp"
#include "test_utils.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using wacc::driver::runDriver;
using wacc::test::utils::samples::cleanUpSamples;
using wacc::test::utils::samples::sampleAsm;
using wacc::test::utils::samples::sampleBin;
using wacc::test::utils::samples::samplePrep;
using wacc::test::utils::samples::sampleSource;

using std::string;
using std::vector;

class DriverTest : public testing::Test {
protected:
    static void TearDownTestSuite() {
        cleanUpSamples([](const auto& path) { return !path.ends_with(".c"); });
    }
};

TEST_F(DriverTest, runDriverNoCleanupSuccess) {
    // ARRANGE
    auto arguments = vector<string>{"--skip-cleanup", sampleSource};
    string error{};

    // ACT
    try {
        runDriver(arguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(sampleSource));
    ASSERT_TRUE(std::filesystem::exists(samplePrep));
    ASSERT_TRUE(std::filesystem::exists(sampleAsm));
    ASSERT_TRUE(std::filesystem::exists(sampleBin));
}

TEST_F(DriverTest, runDriverCleanupSuccess) {
    // ARRANGE
    auto arguments = vector<string>{sampleSource};
    string error{};

    // ACT
    try {
        runDriver(arguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(sampleSource));
    ASSERT_FALSE(std::filesystem::exists(samplePrep));
    ASSERT_FALSE(std::filesystem::exists(sampleAsm));
    ASSERT_TRUE(std::filesystem::exists(sampleBin));
}

TEST_F(DriverTest, runDriverNoCleanupFail) {
    // ARRANGE
    auto validArguments = vector<string>{"--skip-cleanup", sampleSource};
    auto invalidArguments = vector<string>{"--skip-cleanup", "invalid-source"};
    string error{};

    try {
        runDriver(validArguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(samplePrep));
    ASSERT_TRUE(std::filesystem::exists(sampleAsm));
    ASSERT_TRUE(std::filesystem::exists(sampleBin));

    // ACT
    try {
        runDriver(invalidArguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(samplePrep));
    ASSERT_TRUE(std::filesystem::exists(sampleAsm));
    ASSERT_TRUE(std::filesystem::exists(sampleBin));
}

TEST_F(DriverTest, runDriverNoCleanupInvalidFile) {
    // ARRANGE
    auto arguments = vector<string>{"invalid-file"};
    string error{};

    // ACT
    try {
        runDriver(arguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
}

TEST_F(DriverTest, runDriverSpecial) {
    // ARRANGE
    auto arguments = vector<string>{"-S", sampleSource};
    string error{};

    // ACT
    try {
        runDriver(arguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(sampleSource));
    ASSERT_FALSE(std::filesystem::exists(samplePrep));
    ASSERT_TRUE(std::filesystem::exists(sampleAsm));
    ASSERT_FALSE(std::filesystem::exists(sampleBin));
}