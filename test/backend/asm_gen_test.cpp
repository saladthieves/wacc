#include "asm_ast.hpp"
#include "asm_gen.hpp"
#include "base_test.hpp"
#include "matchers.hpp"
#include "tacky_ast.hpp"

#include <cstddef>
#include <gtest/gtest.h>

using namespace wacc::back::ast;
using namespace wacc::tacky::ast;
using namespace wacc::test::match;

using wacc::back::gen::AsmGenerator;

using std::string;

class AsmGeneratorTest :
    public testing::Test,
    public wacc::test::base::BaseTest {};

TEST_F(AsmGeneratorTest, throwOnNull) {
    // ARRANGE
    auto generator = AsmGenerator{nullptr};
    string error{};

    // ACT
    try {
        generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("root tree is null."));
}

TEST_F(AsmGeneratorTest, generateNegate) {
    // ARRANGE
    const auto src = "int main(void) { return -42; }";
    auto generator = getAsmGenerator(src);

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 4);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 42);
        matchAsmPseudo(dest, "MAIN.TEMP.0");
    });

    matchAsmUnary(body[1], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmPseudo(operand, "MAIN.TEMP.0");
    });

    matchAsmMov(body[2], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.0");
        matchAsmReg(dest, AsmRegisterType::AX);
    });

    matchAsmRet(body[3]);
}

TEST_F(AsmGeneratorTest, generate) {
    // ARRANGE
    auto generator = getAsmGenerator();

    // ACT
    auto ast = generator.generate();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 8);

    matchAsmMov(body[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 25);
        matchAsmPseudo(dest, "MAIN.TEMP.0");
    });

    matchAsmUnary(body[1], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NOT);
        matchAsmPseudo(operand, "MAIN.TEMP.0");
    });

    matchAsmMov(body[2], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.0");
        matchAsmPseudo(dest, "MAIN.TEMP.1");
    });

    matchAsmUnary(body[3], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmPseudo(operand, "MAIN.TEMP.1");
    });

    matchAsmMov(body[4], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.1");
        matchAsmPseudo(dest, "MAIN.TEMP.2");
    });

    matchAsmUnary(body[5], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NOT);
        matchAsmPseudo(operand, "MAIN.TEMP.2");
    });

    matchAsmMov(body[6], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.TEMP.2");
        matchAsmReg(dest, AsmRegisterType::AX);
    });

    matchAsmRet(body[7]);
}