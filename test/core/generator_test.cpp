#include "assembly.hpp"
#include "ast.hpp"
#include "generator.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace wacc::core::gen;
using wacc::core::gen::Generator;
using wacc::core::lex::Lexer;
using wacc::core::parse::Parser;
using wacc::test::utils::as;

using std::string;

TEST(GeneratorTest, throwOnNull) {
    // ARRANGE
    auto generator = Generator{nullptr};
    string error{};

    // ACT
    try {
        generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("Root AstNodePtr is null"));
}

TEST(GeneratorTest, throwInvalidRootAst) {
    // ARRANGE
    const auto source =
        R"(
        int main(void) {
            return 15;
        }
        )";

    auto lexer = Lexer{source};
    auto parser = Parser{lexer.scan()};
    auto ast = parser.parse();
    auto program = as<wacc::core::ast::AstProg>(ast);
    auto generator = Generator{std::move(program->function)};
    string error{};

    // ACT
    try {
        generator.generate();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_FALSE(error.empty());
    ASSERT_TRUE(error.contains("Root AstNodePtr is not an AstProg"));
}

TEST(GeneratorTest, testGenerator) {
    // ARRANGE
    const auto source =
        R"(
        int main(void) {
            return 15;
        }
        )";

    auto lexer = Lexer{source};
    auto parser = Parser{lexer.scan()};
    auto generator = Generator{parser.parse()};
    
    // ACT
    auto generated = generator.generate();
    
    // ASSERT
    ASSERT_NE(generated, nullptr);
    auto program = as<AsmProg>(generated);
    
    auto function = as<AsmFun>(program->function);
    ASSERT_TRUE(function->name == "main");
    
    const auto& instructions = function->instructions;
    ASSERT_EQ(instructions.size(), 2);
    
    auto mov = as<AsmMov>(instructions.front());
    auto src = as<AsmImm>(mov->src);
    ASSERT_EQ(src->value, 15);
    as<AsmReg>(mov->dest);
    
    as<AsmRet>(instructions.back());
}