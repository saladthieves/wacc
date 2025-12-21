#include "asm_ast.hpp"
#include "asm_gen.hpp"
#include "base_test.hpp"
#include "tacky_ast.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>

using namespace wacc::back::ast;
using namespace wacc::tacky::ast;

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

    auto asm1 = as<AsmMov>(asmBody.front());
    auto asm1Src = as<AsmImm>(asm1->src);
    ASSERT_EQ(asm1Src->value, 42);
    auto asm1Dest = as<AsmPseudo>(asm1->dest);
    ASSERT_TRUE(asm1Dest->identifier.ends_with("MAIN.UNARY_NEGATE.TEMP.0"));

    auto asm2 = as<AsmUnary>(asmBody[1]);
    ASSERT_EQ(asm2->op, AsmUnaryOpType::UNARY_NEGATE);
    auto asm2Dest = as<AsmPseudo>(asm2->operand);
    ASSERT_TRUE(asm2Dest->identifier.ends_with("MAIN.UNARY_NEGATE.TEMP.0"));

    auto asm3 = as<AsmMov>(asmBody[2]);
    auto asm3Src = as<AsmPseudo>(asm3->src);
    ASSERT_TRUE(asm3Src->identifier.ends_with("MAIN.UNARY_NEGATE.TEMP.0"));
    auto asm3Dest = as<AsmReg>(asm3->dest);
    ASSERT_EQ(asm3Dest->reg, AsmRegisterType::AX);

    auto asm4 = as<AsmRet>(asmBody.back());
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

    auto asm1 = as<AsmMov>(asmBody.front());
    auto asm1Src = as<AsmImm>(asm1->src);
    ASSERT_EQ(asm1Src->value, 25);
    auto asm1Dest = as<AsmPseudo>(asm1->dest);
    ASSERT_TRUE(asm1Dest->identifier.ends_with("MAIN.UNARY_NEGATE.TEMP.0"));

    auto asm2 = as<AsmUnary>(asmBody[1]);
    ASSERT_EQ(asm2->op, AsmUnaryOpType::UNARY_NEGATE);
    auto asm2Dest = as<AsmPseudo>(asm2->operand);
    ASSERT_TRUE(asm2Dest->identifier.ends_with("MAIN.UNARY_NEGATE.TEMP.0"));

    auto asm3 = as<AsmMov>(asmBody[2]);
    auto asm3Src = as<AsmPseudo>(asm3->src);
    ASSERT_TRUE(asm3Src->identifier.ends_with("MAIN.UNARY_NEGATE.TEMP.0"));
    auto asm3Dest = as<AsmPseudo>(asm3->dest);
    ASSERT_TRUE(asm3Dest->identifier.ends_with("MAIN.UNARY_COMPLEMENT.TEMP.1"));

    auto asm4 = as<AsmUnary>(asmBody[3]);
    ASSERT_EQ(asm4->op, AsmUnaryOpType::UNARY_NOT);
    auto asm4Dest = as<AsmPseudo>(asm4->operand);
    ASSERT_TRUE(asm4Dest->identifier.ends_with("MAIN.UNARY_COMPLEMENT.TEMP.1"));

    auto asm5 = as<AsmMov>(asmBody[4]);
    auto asm5Src = as<AsmPseudo>(asm5->src);
    ASSERT_TRUE(asm5Src->identifier.ends_with("MAIN.UNARY_COMPLEMENT.TEMP.1"));
    auto asm5Dest = as<AsmReg>(asm5->dest);
    ASSERT_EQ(asm5Dest->reg, AsmRegisterType::AX);

    auto asm6 = as<AsmRet>(asmBody.back());
}