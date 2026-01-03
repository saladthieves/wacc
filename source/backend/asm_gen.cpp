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
        const auto& type = tacky->type;
        switch (type) {
            using enum TackyNode::Type;
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

            case INSTR_BINARY: {
                auto& tackyBinary = static_cast<const TackyBinary&>(*tacky);
                genForTackyBinary(tackyBinary, asmBody);
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
    auto dest = genAsmReg(AsmReg::Type::AX);
    // mov src, dest
    asmBody.emplace_back(
        std::make_unique<AsmMov>(std::move(src), std::move(dest)));
    // ret
    asmBody.emplace_back(std::make_unique<AsmRet>());
}

void AsmGenerator::genForTackyUnary(const TackyUnary& tacky,
                                    AsmInstrPtrs& asmBody) const {
    // mov src, dest
    asmBody.emplace_back(std::make_unique<AsmMov>(genForTackyVal(*tacky.src),
                                                  genForTackyVal(*tacky.dest)));
    // unop dest
    asmBody.emplace_back(std::make_unique<AsmUnary>(
        genForTackyUnaryOp(tacky.op), genForTackyVal(*tacky.dest)));
}

void AsmGenerator::genForTackyBinary(const TackyBinary& tacky,
                                     AsmInstrPtrs& asmBody) const {
    using enum TackyBinary::Type;
    const auto& op = tacky.op;
    if (op == BINARY_DIVIDE || op == BINARY_REMAINDER) {
        return genForTackyDivRem(tacky, asmBody);
    }

    // add, sub, mult, bitwise (AND, OR, XOR, left shift, right shift)
    // mov src1, dest
    asmBody.emplace_back(std::make_unique<AsmMov>(genForTackyVal(*tacky.src1),
                                                  genForTackyVal(*tacky.dest)));

    // binop src2, dest |or| dest = dest binop src2
    asmBody.emplace_back(std::make_unique<AsmBinary>(
        genForTackyBinaryOp(tacky.op), genForTackyVal(*tacky.src2),
        genForTackyVal(*tacky.dest)));
}

void AsmGenerator::genForTackyDivRem(const TackyBinary& tacky,
                                     AsmInstrPtrs& asmBody) const {
    // mov src1, %eax
    asmBody.emplace_back(std::make_unique<AsmMov>(genForTackyVal(*tacky.src1),
                                                  genAsmReg(AsmReg::Type::AX)));
    // cdq
    asmBody.emplace_back(std::make_unique<AsmCdq>());

    // idiv src2
    asmBody.emplace_back(
        std::make_unique<AsmIdiv>(genForTackyVal(*tacky.src2)));

    auto mov2Src = genAsmReg(tacky.op == TackyBinary::Type::BINARY_DIVIDE
                                 ? AsmReg::Type::AX
                                 : AsmReg::Type::DX);
    // mov [%eax | %edx], dest
    asmBody.emplace_back(std::make_unique<AsmMov>(std::move(mov2Src),
                                                  genForTackyVal(*tacky.dest)));
}

AsmRegPtr AsmGenerator::genAsmReg(AsmReg::Type type) const {
    return std::make_unique<AsmReg>(type);
}

AsmOperandPtr AsmGenerator::genForTackyVal(const TackyVal& tacky) const {
    const auto& type = tacky.type;
    switch (type) {
        using enum TackyNode::Type;
        case LITERAL_INT: {
            auto& tackyConst = static_cast<const TackyLitInt&>(tacky);
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

AsmUnary::Type
AsmGenerator::genForTackyUnaryOp(const TackyUnary::Type& type) const {
    switch (type) {
        using enum TackyUnary::Type;
        case UNARY_COMPLEMENT: return AsmUnary::Type::UNARY_NOT;
        case UNARY_NEGATE:     return AsmUnary::Type::UNARY_NEGATE;
    }
}

AsmBinary::Type
AsmGenerator::genForTackyBinaryOp(const TackyBinary::Type& type) const {
    switch (type) {
        using enum TackyBinary::Type;
        case BINARY_ADD:       return AsmBinary::Type::BINARY_ADD;
        case BINARY_SUBTRACT:  return AsmBinary::Type::BINARY_SUB;
        case BINARY_MULTIPLY:  return AsmBinary::Type::BINARY_MULT;
        case BINARY_BIT_AND:   return AsmBinary::Type::BINARY_BIT_AND;
        case BINARY_BIT_OR:    return AsmBinary::Type::BINARY_BIT_OR;
        case BINARY_BIT_XOR:   return AsmBinary::Type::BINARY_BIT_XOR;
        case BINARY_BIT_LSH:   return AsmBinary::Type::BINARY_BIT_LSH;
        case BINARY_BIT_RSH:   return AsmBinary::Type::BINARY_BIT_RSH;
        case BINARY_DIVIDE:
        case BINARY_REMAINDER: {
            fail("Division and remainder should be handled with AsmIdiv.");
        }
    }
}

} // namespace wacc::back::gen