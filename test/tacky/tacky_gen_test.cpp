#include "ast.hpp"
#include "base_test.hpp"
#include "tacky_ast.hpp"
#include "tacky_gen.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>

using namespace wacc::front::ast;
using namespace wacc::tacky::ast;
using wacc::tacky::ast::TackyUnaryOpType;
using wacc::tacky::gen::TackyGenerator;
using wacc::tacky::gen::VariableGenerator;

using wacc::test::utils::as;

using std::string;

class TackyGeneratorTest :
    public testing::Test,
    public wacc::test::base::BaseTest {};

TEST(VariableGeneratorTest, testGenerate) {
    // ARRANGE
    const auto session = "S1A2B";
    const auto function = "main";
    auto generator = VariableGenerator{};
    generator.resetSession(session);
    generator.resetFunction(function);

    // ACT
    auto first = generator.generate(TackyUnaryOpType::UNARY_COMPLEMENT);
    auto second = generator.generate(TackyUnaryOpType::UNARY_NEGATE);

    // ASSERT
    ASSERT_STREQ(first.c_str(), "S1A2B.MAIN.UNARY_COMPLEMENT.TEMP.0");
    ASSERT_STREQ(second.c_str(), "S1A2B.MAIN.UNARY_NEGATE.TEMP.1");
}

TEST(VariableGeneratorTest, testGenerateRandom) {
    // ARRANGE
    const auto function = "main";
    auto generator = VariableGenerator{};
    generator.resetFunction(function);

    // ACT
    auto first = generator.generate(TackyUnaryOpType::UNARY_COMPLEMENT);
    auto second = generator.generate(TackyUnaryOpType::UNARY_NEGATE);

    // ASSERT
    ASSERT_TRUE(first.starts_with("S"));
    ASSERT_TRUE(first.ends_with("MAIN.UNARY_COMPLEMENT.TEMP.0"));
    ASSERT_TRUE(second.starts_with("S"));
    ASSERT_TRUE(second.ends_with("MAIN.UNARY_NEGATE.TEMP.1"));
}

TEST_F(TackyGeneratorTest, throwOnNull) {
    // ARRANGE
    auto generator = TackyGenerator{nullptr};
    string error{};

    // ACT
    try {
        generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("TackyGeneratorError"));
    ASSERT_TRUE(error.contains("AstNode root tree is null"));
}

TEST_F(TackyGeneratorTest, generate) {
    // ARRANGE
    const auto source = "int main(void) { return 15; }";
    auto generator = getTackyGenerator(source);

    string error{};
    TackyNodePtr node{nullptr};

    // ACT
    try {
        node = generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    auto prog = as<TackyProg>(node);
    auto fun = as<TackyFun>(prog->function);
    ASSERT_TRUE(fun->identifier == "main");
    auto& body = fun->body;
    ASSERT_EQ(body.size(), 1);
    auto ret = as<TackyReturn>(body.front());
    auto val = as<TackyConstant>(ret->val);
    ASSERT_EQ(val->value, 15);
}

TEST_F(TackyGeneratorTest, generateComplement) {
    // ARRANGE
    const auto source = "int main(void) { return ~22; }";
    auto generator = getTackyGenerator(source);

    string error{};
    TackyNodePtr node{nullptr};

    // ACT
    try {
        node = generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    auto prog = as<TackyProg>(node);
    auto fun = as<TackyFun>(prog->function);
    ASSERT_TRUE(fun->identifier == "main");

    auto& body = fun->body;
    ASSERT_EQ(body.size(), 2);

    auto unary = as<TackyUnary>(body.front());
    ASSERT_EQ(unary->op, TackyUnaryOpType::UNARY_COMPLEMENT);
    auto unarySrc = as<TackyConstant>(unary->src);
    ASSERT_EQ(unarySrc->value, 22);
    auto unaryDest = as<TackyVariable>(unary->dest);
    ASSERT_TRUE(
        unaryDest->identifier.ends_with(".MAIN.UNARY_COMPLEMENT.TEMP.0"));
}

TEST_F(TackyGeneratorTest, generateNegate) {
    // ARRANGE
    const auto source = "int start(void) { return ~(-38); }";
    auto generator = getTackyGenerator(source);

    string error{};
    TackyNodePtr node{nullptr};

    // ACT
    try {
        node = generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.empty());
    auto prog = as<TackyProg>(node);
    auto fun = as<TackyFun>(prog->function);
    ASSERT_TRUE(fun->identifier == "start");

    auto& body = fun->body;
    ASSERT_EQ(body.size(), 3);

    auto unaryFirst = as<TackyUnary>(body.front());
    ASSERT_EQ(unaryFirst->op, TackyUnaryOpType::UNARY_NEGATE);
    auto unarySrc = as<TackyConstant>(unaryFirst->src);
    ASSERT_EQ(unarySrc->value, 38);
    auto unaryDest = as<TackyVariable>(unaryFirst->dest);
    ASSERT_TRUE(unaryDest->identifier.ends_with(".START.UNARY_NEGATE.TEMP.0"));

    auto unarySecond = as<TackyUnary>(body[1]);
    ASSERT_EQ(unarySecond->op, TackyUnaryOpType::UNARY_COMPLEMENT);
    auto unarySecondSrc = as<TackyVariable>(unarySecond->src);
    ASSERT_TRUE(
        unarySecondSrc->identifier.ends_with(".START.UNARY_NEGATE.TEMP.0"));
    auto unarySecondDest = as<TackyVariable>(unarySecond->dest);
    ASSERT_TRUE(unarySecondDest->identifier.ends_with(
        ".START.UNARY_COMPLEMENT.TEMP.1"));

    auto third = as<TackyReturn>(body.back());
    auto val = as<TackyVariable>(third->val);
    ASSERT_TRUE(val->identifier.ends_with(".START.UNARY_COMPLEMENT.TEMP.1"));
}