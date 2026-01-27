#include "asm_ast.hpp"
#include "asm_emit.hpp"
#include "base_test.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <vector>

using wacc::back::emit::AsmEmitter;
using wacc::utils::Platform;
using wacc::utils::PlatformType;

using namespace wacc::back::ast;

using std::make_unique;
using std::pair;
using std::string;
using std::vector;

class AsmEmitterTest :
    public testing::Test,
    public AsmEmitter,
    public wacc::test::base::BaseTest {};

TEST_F(AsmEmitterTest, emitThrowOnUnknownPlatform) {
    // ARRANGE
    const auto code = "int main(void) { return 42; }";
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

TEST_F(AsmEmitterTest, emitProgLinux) {
    // ARRANGE
    const auto code = "int main(void) { return 15; }";
    auto emitter = getAsmEmitter(getLinuxPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;

    // clang-format off
    ASSERT_STREQ(lines[0].c_str(),  "    .globl  main");
    ASSERT_STREQ(lines[1].c_str(),  "main:");
    // # Prologue
    ASSERT_STREQ(lines[3].c_str(),  "    pushq   %rbp");
    ASSERT_STREQ(lines[4].c_str(),  "    movq    %rsp,        %rbp");
    ASSERT_STREQ(lines[5].c_str(),  "    subq    $0,          %rsp");
    // Empty line
    // # Instructions
    // ...
    // Epilogue
    ASSERT_STREQ(lines[11].c_str(), "    movq    %rbp,        %rsp");
    ASSERT_STREQ(lines[12].c_str(), "    popq    %rbp");
    ASSERT_STREQ(lines[13].c_str(), "    ret");
    ASSERT_STREQ(lines.back().c_str(), R"(    .section .note.GNU-stack,"",@progbits)");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitProgMacOS) {
    // ARRANGE
    const auto code = "int main(void) { return 21; }";
    auto emitter = getAsmEmitter(getMacOSPlatform(), code);

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;

    ASSERT_STREQ(lines[0].c_str(),  "    .globl  _main");
    ASSERT_STREQ(lines[1].c_str(),  "_main:");
    // # Prologue
    ASSERT_STREQ(lines[3].c_str(),  "    pushq   %rbp");
    ASSERT_STREQ(lines[4].c_str(),  "    movq    %rsp,        %rbp");
    ASSERT_STREQ(lines[5].c_str(),  "    subq    $0,          %rsp");
    // Empty line
    // # Instructions
    // ...
    // Epilogue
    ASSERT_STREQ(lines[11].c_str(), "    movq    %rbp,        %rsp");
    ASSERT_STREQ(lines[12].c_str(), "    popq    %rbp");
    ASSERT_STREQ(lines.back().c_str(), "    ret");
}

TEST_F(AsmEmitterTest, emitAsmAllocStack) {
    // ARRANGE
    const auto value = 16;
    const auto alloc = AsmAllocStack(value);

    // ACT
    emitAsmInstr(alloc);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    subq    $16,         %rsp");
}

TEST_F(AsmEmitterTest, emitAsmMov) {
    // ARRANGE
    auto mov = AsmMov(make_unique<AsmStack>(-4),
                      make_unique<AsmReg>(AsmReg::Type::AX));

    // ACT
    emitAsmInstr(mov);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    movl    -4(%rbp),    %eax");
}

TEST_F(AsmEmitterTest, emitAsmRet) {
    // ARRANGE
    auto ret = AsmRet();

    // ACT
    emitAsmInstr(ret);

    // ASSERT
    // Empty line
    // # Epilogue
    ASSERT_STREQ((*lines)[2].c_str(), "    movq    %rbp,        %rsp");
    ASSERT_STREQ((*lines)[3].c_str(), "    popq    %rbp");
    ASSERT_STREQ((*lines)[4].c_str(), "    ret");
}

TEST_F(AsmEmitterTest, emitAsmUnary) {
    // ARRANGE
    auto unary = AsmUnary(AsmUnary::Type::UNARY_COMPLEMENT, //
                          make_unique<AsmStack>(-8));

    // ACT
    emitAsmInstr(unary);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    notl    -8(%rbp)");
}

TEST_F(AsmEmitterTest, emitAsmBinary) {
    // ARRANGE
    auto binary = AsmBinary(AsmBinary::Type::BINARY_BIT_LSH,
                            make_unique<AsmReg>(AsmReg::Type::CX),
                            make_unique<AsmStack>(-8));

    // ACT
    emitAsmInstr(binary);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    sall    %ecx,        -8(%rbp)");
}

TEST_F(AsmEmitterTest, emitAsmIdiv) {
    // ARRANGE
    auto idiv = AsmIdiv(make_unique<AsmStack>(-16));

    // ACT
    emitAsmInstr(idiv);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    idivl   -16(%rbp)");
}

TEST_F(AsmEmitterTest, emitAsmCdq) {
    // ARRANGE
    auto cdq = AsmCdq();

    // ACT
    emitAsmInstr(cdq);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    cdq");
}

TEST_F(AsmEmitterTest, emitAsmCmp) {
    // ARRANGE
    auto cmp = AsmCmp(make_unique<AsmReg>(AsmReg::Type::AX),
                      make_unique<AsmReg>(AsmReg::Type::CX));

    // ACT
    emitAsmInstr(cmp);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    cmpl    %eax,        %ecx");
}

TEST_F(AsmEmitterTest, emitAsmJmp) {
    // ARRANGE
    const auto label = "BINARY_AND.0.L_END";
    auto jmp = AsmJmp(label);

    // ACT
    emitAsmInstr(jmp);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    jmp     LBINARY_AND.0.L_END");
}

TEST_F(AsmEmitterTest, emitAsmJmpCond) {
    // ARRANGE
    const auto label = "BINARY_OR.0.L_TRUE";
    auto jmpCond = AsmJmpCond(AsmJmpCond::Code::EQUAL, label);

    // ACT
    emitAsmInstr(jmpCond);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    je      LBINARY_OR.0.L_TRUE");
}

TEST_F(AsmEmitterTest, emitAsmSetCond) {
    // ARRANGE
    auto cond = AsmSetCond(AsmSetCond::Code::LESS_EQUAL,
                           make_unique<AsmReg>(AsmReg::Type::CX));

    // ACT
    emitAsmInstr(cond);

    // ASSERT
    ASSERT_STREQ(lines->front().c_str(), "    setle   %ecx");
}

TEST_F(AsmEmitterTest, emitAsmLabel) {
    // ARRANGE
    const auto value = "BINARY_OR.0.L_TRUE";
    auto label = AsmLabel(value);

    // ACT
    emitAsmInstr(label);

    // ASSERT
    // Empty line
    ASSERT_STREQ((*lines)[1].c_str(), "LBINARY_OR.0.L_TRUE:");
}

TEST_F(AsmEmitterTest, formatLabelLinux) {
    // ARRANGE
    platform = Platform{PlatformType::LINUX};
    const auto label = "SOME_LABEL";

    // ACT
    auto formatted = formatLabel(label);

    // ASSERT
    ASSERT_STREQ(formatted.c_str(), ".LSOME_LABEL");
}

TEST_F(AsmEmitterTest, formatLabelMacOS) {
    // ARRANGE
    platform = Platform{PlatformType::MACOS};
    const auto label = "SOME_LABEL";

    // ACT
    auto formatted = formatLabel(label);

    // ASSERT
    ASSERT_STREQ(formatted.c_str(), "LSOME_LABEL");
}

TEST_F(AsmEmitterTest, formatAsmOperand) {
    // ARRANGE
    auto reg = AsmReg(AsmReg::Type::R11);
    auto stack = AsmStack(-16);
    auto imm = AsmImm(29);

    // ACT
    auto regStr = formatAsmOperand(reg);
    auto stackStr = formatAsmOperand(stack);
    auto immStr = formatAsmOperand(imm);

    // ASSERT
    ASSERT_STREQ(regStr.c_str(), "%r11d");
    ASSERT_STREQ(stackStr.c_str(), "-16(%rbp)");
    ASSERT_STREQ(immStr.c_str(), "$29");
}

TEST_F(AsmEmitterTest, formatAsmReg) {
    // ARRANGE
    using enum AsmReg::Size;
    using enum AsmReg::Type;

    const auto types = vector<AsmReg::Type>{AX, CX, DX, R10, R11};
    const auto sizes = vector<AsmReg::Size>{BYTE, WORD, DOUBLE_WORD, QUAD_WORD};

    vector<AsmReg> registers{};
    vector<string> output{};

    for (const auto& type : types) {
        for (const auto& size : sizes) {
            registers.emplace_back(AsmReg(type, size));
        }
    }

    // ACT
    for (const auto& reg : registers) {
        output.emplace_back(formatAsmReg(reg));
    }

    // ASSERT
    // clang-format off
    // AX
    ASSERT_STREQ(output[0].c_str(),  "%al");
    ASSERT_STREQ(output[1].c_str(),  "%ax");
    ASSERT_STREQ(output[2].c_str(),  "%eax");
    ASSERT_STREQ(output[3].c_str(),  "%rax");
    // CX
    ASSERT_STREQ(output[4].c_str(),  "%cl");
    ASSERT_STREQ(output[5].c_str(),  "%cx");
    ASSERT_STREQ(output[6].c_str(),  "%ecx");
    ASSERT_STREQ(output[7].c_str(),  "%rcx");
    // DX
    ASSERT_STREQ(output[8].c_str(),  "%dl");
    ASSERT_STREQ(output[9].c_str(),  "%dx");
    ASSERT_STREQ(output[10].c_str(), "%edx");
    ASSERT_STREQ(output[11].c_str(), "%rdx");
    // R10
    ASSERT_STREQ(output[12].c_str(), "%r10b");
    ASSERT_STREQ(output[13].c_str(), "%r10w");
    ASSERT_STREQ(output[14].c_str(), "%r10d");
    ASSERT_STREQ(output[15].c_str(), "%r10");
    // R11
    ASSERT_STREQ(output[16].c_str(), "%r11b");
    ASSERT_STREQ(output[17].c_str(), "%r11w");
    ASSERT_STREQ(output[18].c_str(), "%r11d");
    ASSERT_STREQ(output[19].c_str(), "%r11");
    // clang-format on
}

TEST_F(AsmEmitterTest, formatAsmUnaryOp) {
    // ARRANGE
    const auto tests = vector<pair<AsmUnary::Type, string>>{
        {AsmUnary::Type::UNARY_NEGATE,     "negl"},
        {AsmUnary::Type::UNARY_COMPLEMENT, "notl"},
    };

    // ACT
    for (const auto& test : tests) {
        auto expected = std::get<string>(test);
        auto actual = formatAsmUnaryOp(std::get<0>(test));

        // ASSERT
        ASSERT_STREQ(expected.c_str(), actual.c_str());
    }
}

TEST_F(AsmEmitterTest, formatAsmBinaryOp) {
    // ARRANGE
    const auto tests = vector<pair<AsmBinary::Type, string>>{
        {AsmBinary::Type::BINARY_ADD,     "addl" },
        {AsmBinary::Type::BINARY_SUB,     "subl" },
        {AsmBinary::Type::BINARY_MULT,    "imull"},
        {AsmBinary::Type::BINARY_BIT_LSH, "sall" },
        {AsmBinary::Type::BINARY_BIT_RSH, "sarl" },
        {AsmBinary::Type::BINARY_BIT_AND, "andl" },
        {AsmBinary::Type::BINARY_BIT_XOR, "xorl" },
        {AsmBinary::Type::BINARY_BIT_OR,  "orl"  },
    };

    for (const auto& test : tests) {
        auto expected = std::get<string>(test);
        auto actual = formatAsmBinaryOp(std::get<0>(test));

        // ASSERT
        ASSERT_STREQ(expected.c_str(), actual.c_str());
    }
}

TEST_F(AsmEmitterTest, formatAsmCondCode) {
    // ARRANGE
    const auto tests = vector<pair<CondCode, string>>{
        {CondCode::EQUAL,         "e" },
        {CondCode::NOT_EQUAL,     "ne"},
        {CondCode::LESS,          "l" },
        {CondCode::LESS_EQUAL,    "le"},
        {CondCode::GREATER,       "g" },
        {CondCode::GREATER_EQUAL, "ge"},
    };

    for (const auto& test : tests) {
        auto expected = std::get<string>(test);
        auto actual = formatAsmCondCode(std::get<0>(test));

        // ASSERT
        ASSERT_STREQ(expected.c_str(), actual.c_str());
    }
}