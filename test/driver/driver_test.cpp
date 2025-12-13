#include "driver.hpp"
#include "test_utils.hpp"

#include <filesystem>
#include <fstream>
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
    void TearDown() override {
        cleanUpSamples([](const auto& path) { return !path.ends_with(".c"); });
    }

    void writeToFile(const string& data, const string& path) {
        std::ofstream file{path};
        file << data;
        file.close();
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

TEST_F(DriverTest, runDriverCleanSpecificSuccess) {
    // ARRANGE
    auto tempRoot = std::filesystem::path{"build/test_temp"};
    if (std::filesystem::exists(tempRoot)) {
        std::filesystem::remove_all(tempRoot);
    }

    std::filesystem::create_directory(tempRoot);

    auto tempTxt = tempRoot / "other.txt";
    writeToFile("hello, world!", tempTxt);

    auto tempConfig = tempRoot / ".config";
    writeToFile("temporary config", tempConfig);

    auto tempSrc = tempRoot / "test.c";
    writeToFile("int main(void) { return 15; }", tempSrc);

    auto tempAsm = tempRoot / "test.s";
    auto tempPrep = tempRoot / "test.i";
    auto tempBin = tempRoot / "test";

    auto arguments = vector<string>{tempSrc};

    // ACT
    runDriver(arguments);

    // ASSERT
    ASSERT_TRUE(std::filesystem::exists(tempTxt));
    ASSERT_TRUE(std::filesystem::exists(tempConfig));
    ASSERT_TRUE(std::filesystem::exists(tempSrc));
    ASSERT_FALSE(std::filesystem::exists(tempAsm));
    ASSERT_FALSE(std::filesystem::exists(tempPrep));
    ASSERT_TRUE(std::filesystem::exists(tempBin));

    std::filesystem::remove_all(tempRoot);
}

TEST_F(DriverTest, runDriverCleanSpecificFail) {
    // ARRANGE
    auto tempRoot = std::filesystem::path{"build/test_temp"};
    if (std::filesystem::exists(tempRoot)) {
        std::filesystem::remove_all(tempRoot);
    }

    std::filesystem::create_directory(tempRoot);

    auto tempTxt = tempRoot / "other.txt";
    writeToFile("hello, world!", tempTxt);

    auto tempConfig = tempRoot / ".config";
    writeToFile("temporary config", tempConfig);

    auto tempSrc = tempRoot / "test.c";
    writeToFile("int main(void) { return 15; }", tempSrc);

    auto tempAsm = tempRoot / "test.s";
    auto tempPrep = tempRoot / "test.i";
    auto tempBin = tempRoot / "test";
    string error{};

    auto arguments = vector<string>{tempSrc};
    try {
        runDriver(arguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(tempBin));

    // ACT
    writeToFile("int main(void) { return 15", tempSrc);

    try {
        runDriver(arguments);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());

    ASSERT_TRUE(std::filesystem::exists(tempTxt));
    ASSERT_TRUE(std::filesystem::exists(tempConfig));
    ASSERT_TRUE(std::filesystem::exists(tempSrc));
    ASSERT_FALSE(std::filesystem::exists(tempAsm));
    ASSERT_FALSE(std::filesystem::exists(tempPrep));
    ASSERT_FALSE(std::filesystem::exists(tempBin));

    std::filesystem::remove_all(tempRoot);
}