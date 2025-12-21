#include "asm_ast.hpp"
#include "asm_gen.hpp"
#include "base_test.hpp"
#include "matchers.hpp"
#include "tacky_ast.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>

using namespace wacc::back::ast;
using namespace wacc::tacky::ast;
using namespace wacc::test::match;

using wacc::back::gen::AsmGenerator;

using wacc::test::utils::as;

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
    auto asmAst = generator.generate();

    // ASSERT
    auto asmProg = as<AsmProg>(asmAst);
    auto asmFun = as<AsmFun>(asmProg->function);

    auto& asmBody = asmFun->instructions;
    ASSERT_EQ(asmBody.size(), 4);

    matchAsmMov(asmBody[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 42);
        matchAsmPseudo(dest, "MAIN.UNARY_NEGATE.TEMP.0");
    });

    matchAsmUnary(asmBody[1], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmPseudo(operand, "MAIN.UNARY_NEGATE.TEMP.0");
    });

    matchAsmMov(asmBody[2], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.UNARY_NEGATE.TEMP.0");
        matchAsmReg(dest, AsmRegisterType::AX);
    });

    matchAsmRet(asmBody[3]);
}

TEST_F(AsmGeneratorTest, generate) {
    // ARRANGE
    auto generator = getAsmGenerator();

    // ACT
    auto asmAst = generator.generate();

    // ASSERT
    auto asmProg = as<AsmProg>(asmAst);
    auto asmFun = as<AsmFun>(asmProg->function);

    auto& asmBody = asmFun->instructions;
    ASSERT_EQ(asmBody.size(), 6);

    matchAsmMov(asmBody[0], [](auto& src, auto& dest) {
        matchAsmImm(src, 25);
        matchAsmPseudo(dest, "MAIN.UNARY_NEGATE.TEMP.0");
    });

    matchAsmUnary(asmBody[1], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NEGATE);
        matchAsmPseudo(operand, "MAIN.UNARY_NEGATE.TEMP.0");
    });

    matchAsmMov(asmBody[2], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.UNARY_NEGATE.TEMP.0");
        matchAsmPseudo(dest, "MAIN.UNARY_COMPLEMENT.TEMP.1");
    });

    matchAsmUnary(asmBody[3], [](auto& op, auto& operand) {
        ASSERT_EQ(op, AsmUnaryOpType::UNARY_NOT);
        matchAsmPseudo(operand, "MAIN.UNARY_COMPLEMENT.TEMP.1");
    });

    matchAsmMov(asmBody[4], [](auto& src, auto& dest) {
        matchAsmPseudo(src, "MAIN.UNARY_COMPLEMENT.TEMP.1");
        matchAsmReg(dest, AsmRegisterType::AX);
    });

    matchAsmRet(asmBody[5]);
}