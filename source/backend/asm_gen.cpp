#include "asm_gen.hpp"
#include "asm_ast.hpp"
#include "ast.hpp"

namespace wacc::back::gen {
AsmGenerator::AsmGenerator(AstNodePtr ptr) : ast{std::move(ptr)} {
}

AsmNodePtr AsmGenerator::generate() const {
    if (!ast) fail("The AstNode root tree is null.");

    auto& prog = static_cast<const AstProg&>(*ast);
    return genForAstProg(prog);
}

AsmProgPtr AsmGenerator::genForAstProg(const AstProg& obj) const {
    auto& fun = static_cast<const AstFun&>(*obj.function);
    return std::make_unique<AsmProg>(genForAstFun(fun));
}

AsmFunPtr AsmGenerator::genForAstFun(const AstFun& obj) const {
    auto name = std::string{obj.name->value};
    auto& stmt = static_cast<const AstStmt&>(*obj.body);
    return std::make_unique<AsmFun>(name, genForAstStmt(stmt));
}

AsmInstrPtrs AsmGenerator::genForAstStmt(const AstStmt& obj) const {
    const auto& type = obj.type();
    switch (type) {
        using enum AstNodeType;
        case RETURN: {
            const auto& retObj = static_cast<const AstReturn&>(obj);
            return genForAstReturn(retObj);
        }

        default: {
            fail("Failed to generate instruction from AstStmt::[type = {}].",
                 type);
        }
    }
}

AsmInstrPtrs AsmGenerator::genForAstReturn(const AstReturn& obj) const {
    auto instructions = AsmInstrPtrs{};
    auto& expr = static_cast<const AstExpr&>(*obj.expression);
    instructions.emplace_back(genAsmMov(expr));
    instructions.emplace_back(genAsmRet());

    return instructions;
}

AsmMovPtr AsmGenerator::genAsmMov(const AstExpr& obj) const {
    const auto& type = obj.type();
    AsmOperandPtr src{nullptr};
    switch (type) {
        using enum AstNodeType;
        case CONST_INTEGER: {
            auto& intObj = static_cast<const AstConstInt&>(obj);
            src = genForAstConstInt(intObj);
            break;
        }

        default: {
            fail("Failed to generate AsmMov from AstExpr::[type = {}].", type);
        }
    }
    auto dest = genAsmReg();

    return std::make_unique<AsmMov>(std::move(src), std::move(dest));
}

AsmRetPtr AsmGenerator::genAsmRet() const {
    return std::make_unique<AsmRet>();
}

AsmImmPtr AsmGenerator::genForAstConstInt(const AstConstInt& obj) const {
    return std::make_unique<AsmImm>(obj.value);
}

AsmRegPtr AsmGenerator::genAsmReg() const {
    return std::make_unique<AsmReg>();
}

} // namespace wacc::back::gen