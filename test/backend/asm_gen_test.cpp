#include "asm_gen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "test_utils.hpp"

#include <gtest/gtest.h>

using namespace wacc::back::ast;

using wacc::back::gen::AsmGenerator;
using wacc::front::lex::Lexer;
using wacc::front::parse::Parser;
using wacc::front::src::Source;
using wacc::test::utils::as;

TEST(AsmGeneratorTest, generate) {
    // ARRANGE
    const auto src =
        R"(
        int main(void) {
            return 15;
        }
        )";

    auto source = Source{src};
    auto lexer = Lexer{source};
    auto parser = Parser{lexer.scan(), source};
    auto generator = AsmGenerator{parser.parse()};
    
    // ACT
    auto ast = generator.generate();

    // ASSERT
    ASSERT_NE(ast, nullptr);
    auto prog = as<AsmProg>(ast);
    
    auto fun = as<AsmFun>(prog->function);
    ASSERT_STREQ(fun->name.c_str(), "main");
    
    auto& instructions = fun->instructions;
    ASSERT_EQ(instructions.size(), 2);
    auto mov = as<AsmMov>(instructions.front());
    auto movSrc = as<AsmImm>(mov->src);
    ASSERT_EQ(movSrc->value, 15);
    auto movDest = as<AsmReg>(mov->dest);
    
    auto ret = as<AsmRet>(instructions.back());
}