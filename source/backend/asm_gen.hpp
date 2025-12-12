#pragma once

#include "asm_ast.hpp"
#include "ast.hpp"

namespace wacc {
namespace back {
namespace gen {
namespace {
using namespace wacc::front::ast;
}

class AsmGenerator {
    class Result {
    public:
        ast::AsmNodePtr tree;
    };

public:
    Result generate();

    ast::AsmProg genForAstProg(const AstProg&);
};
} // namespace gen
} // namespace back
} // namespace wacc