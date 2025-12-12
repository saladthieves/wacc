#include "asm_emit.hpp"
#include "asm_gen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>
#include <vector>

using wacc::back::emit::AsmEmitter;
using wacc::back::gen::AsmGenerator;
using wacc::front::lex::Lexer;
using wacc::front::parse::Parser;
using wacc::front::src::Source;
using wacc::utils::Platform;
using wacc::utils::PlatformType;

using std::string;
using std::vector;

class AsmEmitterTest : public testing::Test {
protected:
    static constexpr auto testSrc = R"(
    int main(void) {
        return 25;
    }
    )";

    Source testSource{testSrc};
    Lexer testLexer{testSource};
    Parser testParser{testLexer.scan(), testSource};
    AsmGenerator testGenerator{testParser.parse()};
};

TEST_F(AsmEmitterTest, emitThrowOnUnknownPlatform) {
    // ARRANGE
    const auto platform = PlatformType{PlatformType::UNKNOWN};
    auto emitter = AsmEmitter{testGenerator.generate(), platform};
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
    auto emitter = AsmEmitter{testGenerator.generate(), platform};

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 5);

    ASSERT_TRUE(lines[0] == "    .globl main");
    ASSERT_TRUE(lines[1] == "main:");
    ASSERT_TRUE(lines[2] == "    movl    $25, %eax");
    ASSERT_TRUE(lines[3] == "    ret");
    ASSERT_TRUE(lines[4] == R"(    .section .note.GNU-stack,"",@progbits)");
}

TEST_F(AsmEmitterTest, emitMacOS) {
    // ARRANGE
    auto platform = Platform{PlatformType::MACOS};
    auto emitter = AsmEmitter{testGenerator.generate(), platform};

    // ACT
    auto ptr = emitter.emit();

    // ASSERT
    auto lines = *ptr;
    ASSERT_EQ(lines.size(), 4);

    ASSERT_TRUE(lines[0] == "    .globl _main");
    ASSERT_TRUE(lines[1] == "_main:");
    ASSERT_TRUE(lines[2] == "    movl    $25, %eax");
    ASSERT_TRUE(lines[3] == "    ret");
}

TEST_F(AsmEmitterTest, emitInstrIndent) {
    // ARRANGE
    auto platform = Platform{PlatformType::MACOS};
    auto emitter = AsmEmitter{testGenerator.generate(), platform};
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