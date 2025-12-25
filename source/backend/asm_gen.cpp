#include "asm_gen.hpp"
#include "asm_ast.hpp"
#include "tacky_ast.hpp"

namespace wacc::back::gen {
AsmGenerator::AsmGenerator(TackyNodePtr ptr) : ast{std::move(ptr)} {
}

AsmNodePtr AsmGenerator::generate() const {
    if (!ast) fail("The AstNode root tree is null.");

    auto& prog = static_cast<const TackyProg&>(*ast);
    return genForTackyProg(prog);
}

AsmProgPtr AsmGenerator::genForTackyProg(const TackyProg& obj) const {
    return std::make_unique<AsmProg>(genForTackyFun(*obj.function));
}

AsmFunPtr AsmGenerator::genForTackyFun(const TackyFun& obj) const {
    auto name = std::string{obj.identifier};
    auto body = genForTackyInstrs(obj.body);
    return std::make_unique<AsmFun>(name, std::move(body));
}

AsmInstrPtrs
AsmGenerator::genForTackyInstrs(const TackyInstrPtrs& tackyBody) const {
    auto asmBody = AsmInstrPtrs{};

    for (const auto& tacky : tackyBody) {
        const auto& type = tacky->type();
        switch (type) {
            using enum TackyNodeType;
            case INSTR_RETURN: {
                auto& tackyRet = static_cast<const TackyReturn&>(*tacky);
                genForTackyReturn(tackyRet, asmBody);
                continue;
            }

            case INSTR_UNARY: {
                auto& tackyUnary = static_cast<const TackyUnary&>(*tacky);
                genForTackyUnary(tackyUnary, asmBody);
                continue;
            }
            default: {
                fail("Unhandled conversion from TackyInstr::[{}] to AsmInstr",
                     type);
            }
        }
    }

    return asmBody;
}

void AsmGenerator::genForTackyReturn(const TackyReturn& tacky,
                                     AsmInstrPtrs& asmBody) const {
    auto src = genForTackyVal(*tacky.val);
    auto dest = genAsmReg(AsmRegisterType::AX);
    asmBody.emplace_back(
        std::make_unique<AsmMov>(std::move(src), std::move(dest)));
    asmBody.emplace_back(std::make_unique<AsmRet>());
}

void AsmGenerator::genForTackyUnary(const TackyUnary& tacky,
                                    AsmInstrPtrs& asmBody) const {
    auto movSrc = genForTackyVal(*tacky.src);
    auto movDest = genForTackyVal(*tacky.dest);
    asmBody.emplace_back(
        std::make_unique<AsmMov>(std::move(movSrc), std::move(movDest)));

    auto unaryOp = genForTackyUnaryOp(tacky.op);
    auto unaryDest = genForTackyVal(*tacky.dest);
    asmBody.emplace_back(
        std::make_unique<AsmUnary>(std::move(unaryOp), std::move(unaryDest)));
}

AsmRegPtr AsmGenerator::genAsmReg(AsmRegisterType type) const {
    return std::make_unique<AsmReg>(type);
}

AsmOperandPtr AsmGenerator::genForTackyVal(const TackyVal& tacky) const {
    const auto& type = tacky.type();
    switch (type) {
        using enum TackyNodeType;
        case CONSTANT: {
            auto& tackyConst = static_cast<const TackyConstant&>(tacky);
            return std::make_unique<AsmImm>(tackyConst.value);
        }
        case VARIABLE: {
            auto& tackyVar = static_cast<const TackyVariable&>(tacky);
            return std::make_unique<AsmPseudo>(tackyVar.identifier);
        }

        default: {
            fail("Unhandled conversion from TackyVal::[{}] to AsmOperand:",
                 type);
        }
    }
}

AsmUnaryOpType
AsmGenerator::genForTackyUnaryOp(const TackyUnaryOpType& type) const {
    switch (type) {
        using enum TackyUnaryOpType;
        case UNARY_COMPLEMENT: return AsmUnaryOpType::UNARY_NOT;
        case UNARY_NEGATE:     return AsmUnaryOpType::UNARY_NEGATE;
    }
}

} // namespace wacc::back::gen