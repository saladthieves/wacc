#include "asm_emit.hpp"
#include "asm_gen.hpp"
#include "asm_instr_fix_pass.hpp"
#include "asm_pseudo_pass.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "tacky_gen.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <vector>

using wacc::back::emit::AsmEmitter;
using wacc::back::gen::AsmGenerator;
using wacc::back::pass::AsmInstrFixPass;
using wacc::back::pass::AsmPseudoPass;
using wacc::front::lex::Lexer;
using wacc::front::parse::Parser;
using wacc::front::src::Source;
using wacc::tacky::gen::TackyGenerator;
using wacc::utils::Platform;
using wacc::utils::PlatformType;

using std::string;
using std::vector;

class AsmEmitterTest : public testing::Test {
protected:
    static constexpr auto testSrc = R"(
    int main(void) {
        return ~(-25);
    }
    )";

    Source testSource{testSrc};
    Lexer testLexer{testSource};
    Parser testParser{testLexer.scan(), testSource};
    TackyGenerator testTacky{testParser.parse()};
    AsmGenerator testGenerator{testTacky.generate()};
    AsmPseudoPass testPseudoPass{testGenerator.generate()};
    AsmInstrFixPass testFixPass{testPseudoPass.run(),
                                testPseudoPass.getOffset()};
};

TEST_F(AsmEmitterTest, emitThrowOnUnknownPlatform) {
    // ARRANGE
    const auto platform = PlatformType{PlatformType::UNKNOWN};
    auto emitter = AsmEmitter{testFixPass.run(), platform};
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
    auto emitter = AsmEmitter{testFixPass.run(), platform};

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 15);

    // clang-format off
    ASSERT_TRUE(lines[0]  == "    .globl main");
    ASSERT_TRUE(lines[1]  == "main:");
    // Prologue
    ASSERT_TRUE(lines[2]  == "    pushq    %rbp");
    ASSERT_TRUE(lines[3]  == "    movq    %rsp, %rbp");
    ASSERT_TRUE(lines[4]  == "    subq    $-8, %rsp");
    // Instructions
    ASSERT_TRUE(lines[5]  == "    movl    $25, -4(%rbp)");
    ASSERT_TRUE(lines[6]  == "    negl    -4(%rbp)");
    ASSERT_TRUE(lines[7]  == "    movl    -4(%rbp), %r10d");
    ASSERT_TRUE(lines[8]  == "    movl    %r10d, -8(%rbp)");
    ASSERT_TRUE(lines[9]  == "    notl    -8(%rbp)");
    ASSERT_TRUE(lines[10] == "    movl    -8(%rbp), %eax");
    // Epilogue
    ASSERT_TRUE(lines[11] == "    movq    %rbp, %rsp");
    ASSERT_TRUE(lines[12] == "    popq    %rbp");
    ASSERT_TRUE(lines[13] == "    ret");
    
    ASSERT_TRUE(lines[14] == R"(    .section .note.GNU-stack,"",@progbits)");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitMacOS) {
    // ARRANGE
    auto platform = Platform{PlatformType::MACOS};
    auto emitter = AsmEmitter{testFixPass.run(), platform};

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 14);

    // clang-format off
    ASSERT_TRUE(lines[0]  == "    .globl _main");
    ASSERT_TRUE(lines[1]  == "_main:");
    // Prologue
    ASSERT_TRUE(lines[2]  == "    pushq    %rbp");
    ASSERT_TRUE(lines[3]  == "    movq    %rsp, %rbp");
    ASSERT_TRUE(lines[4]  == "    subq    $-8, %rsp");
    // Instructions
    ASSERT_TRUE(lines[5]  == "    movl    $25, -4(%rbp)");
    ASSERT_TRUE(lines[6]  == "    negl    -4(%rbp)");
    ASSERT_TRUE(lines[7]  == "    movl    -4(%rbp), %r10d");
    ASSERT_TRUE(lines[8]  == "    movl    %r10d, -8(%rbp)");
    ASSERT_TRUE(lines[9]  == "    notl    -8(%rbp)");
    ASSERT_TRUE(lines[10] == "    movl    -8(%rbp), %eax");
    // Epilogue
    ASSERT_TRUE(lines[11] == "    movq    %rbp, %rsp");
    ASSERT_TRUE(lines[12] == "    popq    %rbp");
    ASSERT_TRUE(lines[13] == "    ret");
    // clang-format on
}

TEST_F(AsmEmitterTest, emitInstrIndent) {
    // ARRANGE
    auto platform = Platform{PlatformType::MACOS};
    auto emitter = AsmEmitter{testFixPass.run(), platform};
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