#include "args.hpp"
#include "assembler.hpp"
#include "test_utils.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using wacc::driver::DriverArgs;
using wacc::driver::runAssembler;
using wacc::test::utils::samples::cleanUpSamples;
using wacc::test::utils::samples::sampleAsm;
using wacc::test::utils::samples::sampleBin;
using wacc::test::utils::samples::sampleSource;

using std::string;
using std::vector;

class AssemblerTest : public testing::Test {
protected:
    void TearDown() override {
        cleanUpSamples([](const auto& path) { return !path.ends_with(".c"); });
    }

    void writeToAsmFile(const vector<string>& lines) {
        std::stringstream buffer{};

        for (const auto& line : lines) {
            buffer << line << '\n';
        }

        std::ofstream file{sampleAsm};
        file << buffer.rdbuf();
        file.close();
    }

    DriverArgs sampleArgs{false, false, false, false, false, sampleSource};
};

TEST_F(AssemblerTest, throwOnInvalidPath) {
    // ARRANGE
    const auto assembly = "invalid-path";
    auto args = DriverArgs{false, false, false, false, false, sampleSource};
    string error{};

    // ACT
    try {
        runAssembler(assembly, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("The provided assembly"));
    ASSERT_TRUE(error.contains("does not exist: "));
}

TEST_F(AssemblerTest, throwOnInvalidExtension) {
    // ARRANGE
    const auto assembly = sampleSource;
    auto args = DriverArgs{false, false, false, false, false, sampleSource};
    string error{};

    // ACT
    try {
        runAssembler(assembly, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("The assembly file"));
    ASSERT_TRUE(error.contains("must end in .s"));
}

TEST_F(AssemblerTest, throwOnInvalidAssembly) {
    // ARRANGE
    const auto assembly = sampleAsm;
    auto args = DriverArgs{false, false, false, false, false, sampleSource};
    const auto lines = vector<string>{"invalid", "assembly"};
    writeToAsmFile(lines);
    string error{};

    // ACT
    try {
        runAssembler(assembly, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("Running "));
    ASSERT_TRUE(error.contains("failed: [exitCode: "));
    ASSERT_TRUE(std::filesystem::exists(sampleAsm));
}

TEST_F(AssemblerTest, assemble) {
    // ARRANGE
    const auto assembly = sampleAsm;
    auto args = DriverArgs{false, false, false, false, false, sampleSource};
    const auto platform = wacc::utils::Platform{};
    string main = platform.isMacOS() ? "_main" : "main";
    const auto lines = vector<string>{
        std::format("    .globl {}", main),
        std::format("{}:", main),
        std::format("    movl    $42, %eax"),
        std::format("    ret"),
    };
    writeToAsmFile(lines);
    string error{};

    // ACT
    try {
        runAssembler(assembly, args);
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(std::filesystem::exists(sampleBin));
}