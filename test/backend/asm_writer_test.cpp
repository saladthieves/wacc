#include "asm_writer.hpp"
#include "test_utils.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

using wacc::back::write::AsmWriter;

using wacc::test::utils::samples::cleanUpSamples;
using wacc::test::utils::samples::sampleAsm;

using std::make_unique;
using std::string;
using std::vector;

class AsmWriterTest : public testing::Test {
protected:
    static void TearDownTestSuite() {
        cleanUpSamples([](const auto& path) { return !path.ends_with(".c"); });
    }
};

TEST_F(AsmWriterTest, writeThrowOnNull) {
    // ARRANGE
    auto writer = AsmWriter{nullptr, ""};
    string error{};

    // ACT
    try {
        writer.write();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("LinesPtr is null"));
}

TEST_F(AsmWriterTest, writeThrowOnEmptyLines) {
    // ARRANGE
    auto lines = make_unique<vector<string>>();
    auto writer = AsmWriter{std::move(lines), ""};
    string error{};

    // ACT
    try {
        writer.write();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("LinesPtr is empty"));
}

TEST_F(AsmWriterTest, writeThrowOnEmptyPath) {
    // ARRANGE
    auto lines = make_unique<vector<string>>();
    lines->emplace_back("");
    auto writer = AsmWriter{std::move(lines), ""};
    string error{};

    // ACT
    try {
        writer.write();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("No output file provided"));
}

TEST_F(AsmWriterTest, write) {
    // ARRANGE
    auto lines = make_unique<vector<string>>();
    lines->push_back("    .glob _main");
    lines->push_back("_main:");
    lines->push_back("    movl    $10, %eax");
    lines->push_back("    ret");
    auto writer = AsmWriter{std::move(lines), sampleAsm};
    string error{};

    // ACT
    try {
        writer.write();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    auto content = wacc::utils::readFile(sampleAsm);

    // ASSERT
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(content.contains("    .glob _main\n"));
    ASSERT_TRUE(content.contains("_main:\n"));
    ASSERT_TRUE(content.contains("    movl    $10, %eax\n"));
    ASSERT_TRUE(content.contains("    ret\n"));
}