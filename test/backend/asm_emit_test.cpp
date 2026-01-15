#include "asm_ast.hpp"
#include "asm_emit.hpp"
#include "base_test.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <tuple>
#include <vector>

using wacc::back::emit::AsmEmitter;
using wacc::utils::Platform;

using wacc::back::ast::AsmFun;
using wacc::back::ast::AsmInstrPtrs;
using wacc::back::ast::AsmProg;
using wacc::back::ast::AsmReg;
using wacc::back::ast::AsmUnary;

using std::make_unique;
using std::string;
using std::tuple;
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
    const auto code = "int main(void) { return 15; }";
    auto emitter = getAsmEmitter(getLinuxPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 10);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(), "    .globl main");
    ASSERT_STREQ(lines[1].c_str(), "main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(), "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(), "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(), "    subq    $0, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(), "    movl    $15, %eax");
    // Epilogue
    ASSERT_STREQ(lines[6].c_str(), "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[7].c_str(), "    popq    %rbp");
    ASSERT_STREQ(lines[8].c_str(), "    ret");

    ASSERT_STREQ(lines[9].c_str(), R"(    .section .note.GNU-stack,"",@progbits)");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitMacOS) {
    // ARRANGE
    const auto code = "int main(void) { return 21; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 9);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(), "    .globl _main");
    ASSERT_STREQ(lines[1].c_str(), "_main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(), "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(), "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(), "    subq    $0, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(), "    movl    $21, %eax");
    // Epilogue
    ASSERT_STREQ(lines[6].c_str(), "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[7].c_str(), "    popq    %rbp");
    ASSERT_STREQ(lines[8].c_str(), "    ret");
    // clang-format on
}

TEST_F(AsmEmitterTest, formatAsmReg) {
    // ARRANGE
    using enum AsmReg::Size;
    using enum AsmReg::Type;

    const auto sizes = vector<AsmReg::Size>{BYTE, WORD, DOUBLE_WORD, QUAD_WORD};
    const auto registers = vector<AsmReg::Type>{AX, CX, DX, R10, R11};

    auto instructions = AsmInstrPtrs{};

    const auto addInstr = [&](auto op, auto reg) {
        for (const auto& size : sizes) {
            instructions.emplace_back(
                make_unique<AsmUnary>(op, std::make_unique<AsmReg>(reg, size)));
        }
    };

    const auto op = AsmUnary::Type::UNARY_NEGATE;

    for (const auto& reg : registers) {
        addInstr(op, reg);
    }

    auto function =
        make_unique<AsmFun>(AsmFun("main", std::move(instructions)));
    auto program = make_unique<AsmProg>(AsmProg(std::move(function)));

    auto emitter = AsmEmitter{std::move(program), getPlatform()};

    // ACT
    auto lines = *emitter.emit();

    // clang-format off
    // ASSERT
    // AX
    ASSERT_STREQ(lines[4].c_str(),  "    negl    %al");
    ASSERT_STREQ(lines[5].c_str(),  "    negl    %ax");
    ASSERT_STREQ(lines[6].c_str(),  "    negl    %eax");
    ASSERT_STREQ(lines[7].c_str(),  "    negl    %rax");
    // CX
    ASSERT_STREQ(lines[8].c_str(),  "    negl    %cl");
    ASSERT_STREQ(lines[9].c_str(),  "    negl    %cx");
    ASSERT_STREQ(lines[10].c_str(), "    negl    %ecx");
    ASSERT_STREQ(lines[11].c_str(), "    negl    %rcx");
    // DX
    ASSERT_STREQ(lines[12].c_str(), "    negl    %dl");
    ASSERT_STREQ(lines[13].c_str(), "    negl    %dx");
    ASSERT_STREQ(lines[14].c_str(), "    negl    %edx");
    ASSERT_STREQ(lines[15].c_str(), "    negl    %rdx");
    // R10
    ASSERT_STREQ(lines[16].c_str(), "    negl    %r10b");
    ASSERT_STREQ(lines[17].c_str(), "    negl    %r10w");
    ASSERT_STREQ(lines[18].c_str(), "    negl    %r10d");
    ASSERT_STREQ(lines[19].c_str(), "    negl    %r10");
    // R11
    ASSERT_STREQ(lines[20].c_str(), "    negl    %r11b");
    ASSERT_STREQ(lines[21].c_str(), "    negl    %r11w");
    ASSERT_STREQ(lines[22].c_str(), "    negl    %r11d");
    ASSERT_STREQ(lines[23].c_str(), "    negl    %r11");
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

TEST_F(AsmEmitterTest, emitAsmMov) {
    // ARRANGE
    const auto code = "int main(void) { return 28; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 9);

    ASSERT_STREQ(lines[0].c_str(), "    .globl _main");
    ASSERT_STREQ(lines[1].c_str(), "_main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(), "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(), "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(), "    subq    $0, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(), "    movl    $28, %eax");
    // Epilogue
    ASSERT_STREQ(lines[6].c_str(), "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[7].c_str(), "    popq    %rbp");
    ASSERT_STREQ(lines[8].c_str(), "    ret");
}

TEST_F(AsmEmitterTest, emitAsmUnary) {
    // ARRANGE
    const auto code = "int main(void) { return -2; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 11);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(),  "    .globl _main");
    ASSERT_STREQ(lines[1].c_str(),  "_main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(),  "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(),  "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(),  "    subq    $4, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(),  "    movl    $2, -4(%rbp)");
    ASSERT_STREQ(lines[6].c_str(),  "    negl    -4(%rbp)");
    ASSERT_STREQ(lines[7].c_str(),  "    movl    -4(%rbp), %eax");
    // Epilogue
    ASSERT_STREQ(lines[8].c_str(),  "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[9].c_str(),  "    popq    %rbp");
    ASSERT_STREQ(lines[10].c_str(), "    ret");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitAsmBinaryMul) {
    // ARRANGE
    const auto code = "int main(void) { return 2 * 3; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 13);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(),   "    .globl _main");
    ASSERT_STREQ(lines[1].c_str(),   "_main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(),   "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(),   "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(),   "    subq    $4, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(),   "    movl    $2, -4(%rbp)");
    ASSERT_STREQ(lines[6].c_str(),   "    movl    -4(%rbp), %r11d");
    ASSERT_STREQ(lines[7].c_str(),   "    imull    $3, %r11d");
    ASSERT_STREQ(lines[8].c_str(),   "    movl    %r11d, -4(%rbp)");
    ASSERT_STREQ(lines[9].c_str(),   "    movl    -4(%rbp), %eax");
    // Epilogue
    ASSERT_STREQ(lines[10].c_str(),  "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[11].c_str(),  "    popq    %rbp");
    ASSERT_STREQ(lines[12].c_str(),  "    ret");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitAsmBinary) {
    // ARRANGE
    const auto tests = vector<tuple<string, string, string, string>>{
        {"{ return 1 + 2; }",   "$1",  "addl", "$2"},
        {"{ return 3 - 1; }",   "$3",  "subl", "$1"},
        {"{ return 4 << 1; }",  "$4",  "sall", "$1"},
        {"{ return 16 >> 2; }", "$16", "sarl", "$2"},
        {"{ return 0 & 1; }",   "$0",  "andl", "$1"},
        {"{ return 14 ^ 2; }",  "$14", "xorl", "$2"},
        {"{ return 3 | 3; }",   "$3",  "orl",  "$3"},
    };
    for (const auto& [main, src, op, dest] : tests) {
        const auto code = std::format("int main(void) {}", main);
        auto emitter = getAsmEmitter(getMacOSPlatform(), code);

        // ACT
        auto ptr = emitter.emit();

        // ASSERT
        auto lines = *ptr;
        ASSERT_EQ(lines.size(), 11);

        // clang-format off
        ASSERT_STREQ(lines[0].c_str(),  "    .globl _main");
        ASSERT_STREQ(lines[1].c_str(),  "_main:");
        // Prologue
        ASSERT_STREQ(lines[2].c_str(),  "    pushq    %rbp");
        ASSERT_STREQ(lines[3].c_str(),  "    movq    %rsp, %rbp");
        ASSERT_STREQ(lines[4].c_str(),  "    subq    $4, %rsp");
        // Instructions
        ASSERT_STREQ(lines[5].c_str(),  
                            std::format("    movl    {}, -4(%rbp)", 
                            src).c_str());
        ASSERT_STREQ(lines[6].c_str(),  
                            std::format("    {}    {}, -4(%rbp)", 
                            op, dest).c_str());
        ASSERT_STREQ(lines[7].c_str(),  "    movl    -4(%rbp), %eax");
        // Epilogue
        ASSERT_STREQ(lines[8].c_str(),  "    movq    %rbp, %rsp");
        ASSERT_STREQ(lines[9].c_str(),  "    popq    %rbp");
        ASSERT_STREQ(lines[10].c_str(), "    ret");
        // clang-format on
    }
}

TEST_F(AsmEmitterTest, emitAsmBinaryIdivDiv) {
    // ARRANGE
    const auto code = "int main(void) { return 5 / 2; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 14);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(),   "    .globl _main");
    ASSERT_STREQ(lines[1].c_str(),   "_main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(),   "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(),   "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(),   "    subq    $4, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(),   "    movl    $5, %eax");
    ASSERT_STREQ(lines[6].c_str(),   "    cdq");
    ASSERT_STREQ(lines[7].c_str(),   "    movl    $2, %r10d");
    ASSERT_STREQ(lines[8].c_str(),   "    idivl    %r10d");
    ASSERT_STREQ(lines[9].c_str(),   "    movl    %eax, -4(%rbp)");
    ASSERT_STREQ(lines[10].c_str(),  "    movl    -4(%rbp), %eax");
    // Epilogue
    ASSERT_STREQ(lines[11].c_str(),  "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[12].c_str(),  "    popq    %rbp");
    ASSERT_STREQ(lines[13].c_str(),  "    ret");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitAsmBinaryIdivMod) {
    // ARRANGE
    const auto code = "int main(void) { return 4 % 1; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 14);

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(),   "    .globl _main");
    ASSERT_STREQ(lines[1].c_str(),   "_main:");
    // Prologue
    ASSERT_STREQ(lines[2].c_str(),   "    pushq    %rbp");
    ASSERT_STREQ(lines[3].c_str(),   "    movq    %rsp, %rbp");
    ASSERT_STREQ(lines[4].c_str(),   "    subq    $4, %rsp");
    // Instructions
    ASSERT_STREQ(lines[5].c_str(),   "    movl    $4, %eax");
    ASSERT_STREQ(lines[6].c_str(),   "    cdq");
    ASSERT_STREQ(lines[7].c_str(),   "    movl    $1, %r10d");
    ASSERT_STREQ(lines[8].c_str(),   "    idivl    %r10d");
    ASSERT_STREQ(lines[9].c_str(),   "    movl    %edx, -4(%rbp)");
    ASSERT_STREQ(lines[10].c_str(),  "    movl    -4(%rbp), %eax");
    // Epilogue
    ASSERT_STREQ(lines[11].c_str(),  "    movq    %rbp, %rsp");
    ASSERT_STREQ(lines[12].c_str(),  "    popq    %rbp");
    ASSERT_STREQ(lines[13].c_str(),  "    ret");
    // clang-format on
}