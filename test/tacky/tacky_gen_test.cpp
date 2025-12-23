#include "ast.hpp"
#include "base_test.hpp"
#include "matchers.hpp"
#include "tacky_ast.hpp"
#include "tacky_gen.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>

using namespace wacc::front::ast;
using namespace wacc::tacky::ast;
using namespace wacc::test::match;

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
    auto first = generator.generate();
    auto second = generator.generate();

    // ASSERT
    ASSERT_STREQ(first.c_str(), "S1A2B.MAIN.TEMP.0");
    ASSERT_STREQ(second.c_str(), "S1A2B.MAIN.TEMP.1");
}

TEST(VariableGeneratorTest, testGenerateRandom) {
    // ARRANGE
    const auto function = "main";
    auto generator = VariableGenerator{};
    generator.resetFunction(function);

    // ACT
    auto first = generator.generate();
    auto second = generator.generate();

    // ASSERT
    ASSERT_TRUE(first.starts_with("S"));
    ASSERT_TRUE(first.ends_with("MAIN.TEMP.0"));
    ASSERT_TRUE(second.starts_with("S"));
    ASSERT_TRUE(second.ends_with("MAIN.TEMP.1"));
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

    matchTackyReturn(body[0], [](auto& val) { matchTackyConstant(val, 15); });
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

    matchTackyUnary(body[0], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, TackyUnaryOpType::UNARY_COMPLEMENT);
        matchTackyConstant(src, 22);
        matchTackyVariable(dest, ".MAIN.TEMP.0");
    });
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

    matchTackyUnary(body[0], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, TackyUnaryOpType::UNARY_NEGATE);
        matchTackyConstant(src, 38);
        matchTackyVariable(dest, ".START.TEMP.0");
    });

    matchTackyUnary(body[1], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, TackyUnaryOpType::UNARY_COMPLEMENT);
        matchTackyVariable(src, ".START.TEMP.0");
        matchTackyVariable(dest, ".START.TEMP.1");
    });

    matchTackyReturn(body[2], [](auto& val) {
        matchTackyVariable(val, ".START.TEMP.1");
    });
}