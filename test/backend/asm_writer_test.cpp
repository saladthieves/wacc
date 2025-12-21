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
    auto lines = make_unique<vector<string>>();
    lines->push_back("    .globl main");
    lines->push_back("main:");
    lines->push_back("    pushq    %rbp");
    lines->push_back("    movq    %rsp, %rbp");
    lines->push_back("    subq    $-8, %rsp");
    lines->push_back("    movl    $25, -4(%rbp)");
    lines->push_back("    negl    -4(%rbp)");
    lines->push_back("    movl    -4(%rbp), %r10d");
    lines->push_back("    movl    %r10d, -8(%rbp)");
    lines->push_back("    notl    -8(%rbp)");
    lines->push_back("    movl    -8(%rbp), %eax");
    lines->push_back("    movq    %rbp, %rsp");
    lines->push_back("    popq    %rbp");
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
    
    ASSERT_TRUE(content.contains("    .globl main\n"));
    ASSERT_TRUE(content.contains("main:\n"));
    ASSERT_TRUE(content.contains("    pushq    %rbp\n"));
    ASSERT_TRUE(content.contains("    movq    %rsp, %rbp\n"));
    ASSERT_TRUE(content.contains("    subq    $-8, %rsp\n"));
    ASSERT_TRUE(content.contains("    movl    $25, -4(%rbp)\n"));
    ASSERT_TRUE(content.contains("    negl    -4(%rbp)\n"));
    ASSERT_TRUE(content.contains("    movl    -4(%rbp), %r10d\n"));
    ASSERT_TRUE(content.contains("    movl    %r10d, -8(%rbp)\n"));
    ASSERT_TRUE(content.contains("    notl    -8(%rbp)\n"));
    ASSERT_TRUE(content.contains("    movl    -8(%rbp), %eax\n"));
    ASSERT_TRUE(content.contains("    movq    %rbp, %rsp\n"));
    ASSERT_TRUE(content.contains("    popq    %rbp\n"));
    ASSERT_TRUE(content.contains("    ret\n"));
}