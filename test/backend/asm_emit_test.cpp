#include "asm_emit.hpp"
#include "base_test.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <vector>

using wacc::back::emit::AsmEmitter;
using wacc::utils::Platform;
using wacc::utils::PlatformType;

using std::string;
using std::vector;

class AsmEmitterTest :
    public testing::Test,
    public wacc::test::base::BaseTest {};

TEST_F(AsmEmitterTest, emitThrowOnUnknownPlatform) {
    // ARRANGE
    auto emitter = getAsmEmitter(getUnknownPlatform());
    string error{};

    // ACT
    try {
        emitter.emit();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("Failed to emit"));
    ASSERT_TRUE(error.contains("unknown platform"));
}

TEST_F(AsmEmitterTest, emitThrowOnNull) {
    // ARRANGE
    auto emitter = AsmEmitter{nullptr, Platform{}};
    string error{};

    // ACT
    try {
        emitter.emit();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("Failed to emit"));
    ASSERT_TRUE(error.contains("tree is null"));
}

TEST_F(AsmEmitterTest, emitLinux) {
    // ARRANGE
    auto platform = Platform{PlatformType::LINUX};
    auto emitter = getAsmEmitter(getLinuxPlatform());

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 18);

    // clang-format off
    ASSERT_TRUE(lines[0]  == "    .globl main");
    ASSERT_TRUE(lines[1]  == "main:");
    // Prologue
    ASSERT_TRUE(lines[2]  == "    pushq    %rbp");
    ASSERT_TRUE(lines[3]  == "    movq    %rsp, %rbp");
    ASSERT_TRUE(lines[4]  == "    subq    $12, %rsp");
    // Instructions
    ASSERT_TRUE(lines[5]  == "    movl    $25, -4(%rbp)");
    ASSERT_TRUE(lines[6]  == "    notl    -4(%rbp)");
    ASSERT_TRUE(lines[7]  == "    movl    -4(%rbp), %r10d");
    ASSERT_TRUE(lines[8]  == "    movl    %r10d, -8(%rbp)");
    ASSERT_TRUE(lines[9]  == "    negl    -8(%rbp)");
    ASSERT_TRUE(lines[10] == "    movl    -8(%rbp), %r10d");
    ASSERT_TRUE(lines[11] == "    movl    %r10d, -12(%rbp)");
    ASSERT_TRUE(lines[12] == "    notl    -12(%rbp)");
    ASSERT_TRUE(lines[13] == "    movl    -12(%rbp), %eax");
    // Epilogue
    ASSERT_TRUE(lines[14] == "    movq    %rbp, %rsp");
    ASSERT_TRUE(lines[15] == "    popq    %rbp");
    ASSERT_TRUE(lines[16] == "    ret");

    ASSERT_TRUE(lines[17] == R"(    .section .note.GNU-stack,"",@progbits)");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitMacOS) {
    // ARRANGE
    auto emitter = getAsmEmitter(getMacOSPlatform());

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 17);

    // clang-format off
    ASSERT_TRUE(lines[0]  == "    .globl _main");
    ASSERT_TRUE(lines[1]  == "_main:");
    // Prologue
    ASSERT_TRUE(lines[2]  == "    pushq    %rbp");
    ASSERT_TRUE(lines[3]  == "    movq    %rsp, %rbp");
    ASSERT_TRUE(lines[4]  == "    subq    $12, %rsp");
    // Instructions
    ASSERT_TRUE(lines[5]  == "    movl    $25, -4(%rbp)");
    ASSERT_TRUE(lines[6]  == "    notl    -4(%rbp)");
    ASSERT_TRUE(lines[7]  == "    movl    -4(%rbp), %r10d");
    ASSERT_TRUE(lines[8]  == "    movl    %r10d, -8(%rbp)");
    ASSERT_TRUE(lines[9]  == "    negl    -8(%rbp)");
    ASSERT_TRUE(lines[10] == "    movl    -8(%rbp), %r10d");
    ASSERT_TRUE(lines[11] == "    movl    %r10d, -12(%rbp)");
    ASSERT_TRUE(lines[12] == "    notl    -12(%rbp)");
    ASSERT_TRUE(lines[13] == "    movl    -12(%rbp), %eax");
    // Epilogue
    ASSERT_TRUE(lines[14] == "    movq    %rbp, %rsp");
    ASSERT_TRUE(lines[15] == "    popq    %rbp");
    ASSERT_TRUE(lines[16] == "    ret");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitInstrIndent) {
    // ARRANGE
    auto emitter = getAsmEmitter(getMacOSPlatform());
    auto locations = vector<std::size_t>{};

    // ACT
    auto ptr = emitter.emit();
    auto lines = *ptr;

    for (std::size_t i = 0; i != lines.size(); ++i) {
        const auto& line = lines[i];
        if (line.starts_with("_") && line.ends_with(":")) {
            locations.push_back(i);
        }
    }

    const auto validate = [&](auto& start, const int& stop) {
        for (auto i = start + 1; i < stop; ++i) {
            const auto& line = lines[i];
            ASSERT_TRUE(line.starts_with("    "));
        }
    };

    // ASSERT
    ASSERT_FALSE(locations.empty());

    for (auto i = 0; i < locations.size(); ++i) {
        auto& start = locations[i];
        auto stop = i == locations.size() - 1 ? lines.size() : locations[i + 1];
        validate(start, stop);
    }
}
