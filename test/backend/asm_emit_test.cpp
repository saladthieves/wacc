#include "asm_emit.hpp"
#include "base_test.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <vector>

using wacc::back::emit::AsmEmitter;
using wacc::utils::Platform;

using std::string;
using std::vector;

class AsmEmitterTest :
    public testing::Test,
    public wacc::test::base::BaseTest {};

TEST_F(AsmEmitterTest, emitThrowOnUnknownPlatform) {
    // ARRANGE
    const auto code = "int main(void) { return -15 * ~32; }";
    auto emitter = getAsmEmitter(getUnknownPlatform(), code);
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
    const auto code = "int main(void) { return -15 * ~32; }";
    auto emitter = getAsmEmitter(getLinuxPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 19);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(), "    .globl main");
    ASSERT_STREQ(lines[1].c_str(), "main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(), "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(), "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(), "    subq    $12, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(), "    movl    $15, -4(%rbp)");
    ASSERT_STREQ(lines[6].c_str(), "    negl    -4(%rbp)");

    ASSERT_STREQ(lines[7].c_str(), "    movl    $32, -8(%rbp)");
    ASSERT_STREQ(lines[8].c_str(), "    notl    -8(%rbp)");
    
    ASSERT_STREQ(lines[9].c_str(), "    movl    -4(%rbp), %r10d");
    ASSERT_STREQ(lines[10].c_str(), "    movl    %r10d, -12(%rbp)");
    ASSERT_STREQ(lines[11].c_str(), "    movl    -12(%rbp), %r11d");
    ASSERT_STREQ(lines[12].c_str(), "    imull    -8(%rbp), %r11d");
    ASSERT_STREQ(lines[13].c_str(), "    movl    %r11d, -12(%rbp)");
    ASSERT_STREQ(lines[14].c_str(), "    movl    -12(%rbp), %eax");
    // Epilogue
    ASSERT_STREQ(lines[15].c_str(), "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[16].c_str(), "    popq    %rbp");
    ASSERT_STREQ(lines[17].c_str(), "    ret");

    ASSERT_STREQ(lines[18].c_str(), R"(    .section .note.GNU-stack,"",@progbits)");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitMacOS) {
    // ARRANGE
    const auto code = "int main(void) { return -15 * ~32; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 18);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(), "    .globl _main");
    ASSERT_STREQ(lines[1].c_str(), "_main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(), "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(), "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(), "    subq    $12, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(), "    movl    $15, -4(%rbp)");
    ASSERT_STREQ(lines[6].c_str(), "    negl    -4(%rbp)");

    ASSERT_STREQ(lines[7].c_str(), "    movl    $32, -8(%rbp)");
    ASSERT_STREQ(lines[8].c_str(), "    notl    -8(%rbp)");
    
    ASSERT_STREQ(lines[9].c_str(), "    movl    -4(%rbp), %r10d");
    ASSERT_STREQ(lines[10].c_str(), "    movl    %r10d, -12(%rbp)");
    ASSERT_STREQ(lines[11].c_str(), "    movl    -12(%rbp), %r11d");
    ASSERT_STREQ(lines[12].c_str(), "    imull    -8(%rbp), %r11d");
    ASSERT_STREQ(lines[13].c_str(), "    movl    %r11d, -12(%rbp)");
    ASSERT_STREQ(lines[14].c_str(), "    movl    -12(%rbp), %eax");
    // Epilogue
    ASSERT_STREQ(lines[15].c_str(), "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[16].c_str(), "    popq    %rbp");
    ASSERT_STREQ(lines[17].c_str(), "    ret");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitInstrIndent) {
    // ARRANGE
    const auto code = "int main(void) { return -15 * ~32; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);
    auto locations = vector<std::size_t>{};

    // ACT
    auto ptr = emitter.emit();
    const auto& lines = *ptr;

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