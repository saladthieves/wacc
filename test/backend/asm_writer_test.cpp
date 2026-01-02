#include "asm_writer.hpp"
#include "base_test.hpp"
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

class AsmWriterTest : public testing::Test, public wacc::test::base::BaseTest {
protected:
    void TearDown() override {
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
    const auto code = "int main(void) { return 15 + -30; }";
    auto emitter = getAsmEmitter(code);
    auto lines = *emitter.emit();
    auto copy = make_unique<decltype(lines)>(lines);
    auto writer = AsmWriter{std::move(copy), sampleAsm};
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

    for (const auto& line : lines) {
        ASSERT_TRUE(content.contains(line));
    }
}