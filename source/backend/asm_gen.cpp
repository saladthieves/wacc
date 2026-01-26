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

Body AsmGenerator::genForTackyInstrs(const TackyInstrPtrs& tackyBody) const {
    auto body = Body{};

    for (const auto& tacky : tackyBody) {
        const auto& type = tacky->type;
        switch (type) {
            using enum TackyNode::Type;
            case INSTR_RETURN: {
                auto& tackyRet = static_cast<const TackyReturn&>(*tacky);
                genForTackyReturn(tackyRet, body);
                continue;
            }
            case INSTR_UNARY: {
                auto& tackyUnary = static_cast<const TackyUnary&>(*tacky);
                genForTackyUnary(tackyUnary, body);
                continue;
            }
            case INSTR_BINARY: {
                auto& tackyBinary = static_cast<const TackyBinary&>(*tacky);
                genForTackyBinary(tackyBinary, body);
                continue;
            }
            case INSTR_COPY: {
                auto& tackyCopy = static_cast<TackyCopy&>(*tacky);
                genForTackyCopy(tackyCopy, body);
                continue;
            }
            case INSTR_JUMP: {
                auto& tackyJump = static_cast<TackyJump&>(*tacky);
                genForTackyJump(tackyJump, body);
                continue;
            }
            case INSTR_JUMP_ZERO: {
                auto& zero = static_cast<TackyJumpZero&>(*tacky);
                genForTackyJumpZero(zero.condition, zero.target, false, body);
                continue;
            }
            case INSTR_JUMP_NOT_ZERO: {
                auto& zero = static_cast<TackyJumpNotZero&>(*tacky);
                genForTackyJumpZero(zero.condition, zero.target, true, body);
                continue;
            }
            case INSTR_LABEL: {
                auto& tackyLabel = static_cast<TackyLabel&>(*tacky);
                genForTackyLabel(tackyLabel, body);
                continue;
            }
            default: {
                fail("Unhandled conversion from TackyInstr::[{}] to AsmInstr",
                     type);
            }
        }
    }

    return body;
}

void AsmGenerator::genForTackyReturn(const TackyReturn& tacky,
                                     Body& body) const {

    auto src = genForTackyVal(*tacky.val);
    auto dest = genAsmReg(AsmReg::Type::AX);
    // mov src, dest
    body.emplace_back(
        std::make_unique<AsmMov>(std::move(src), std::move(dest)));
    // ret
    body.emplace_back(std::make_unique<AsmRet>());
}

void AsmGenerator::genForTackyUnary(const TackyUnary& tacky, Body& body) const {
    using enum TackyUnary::Type;
    const auto& op = tacky.op;
    if (op == UNARY_COMPLEMENT || op == UNARY_NEGATE) {
        // mov src, dest
        body.emplace_back(std::make_unique<AsmMov>(
            genForTackyVal(*tacky.src), genForTackyVal(*tacky.dest)));

        // unop dest
        body.emplace_back(std::make_unique<AsmUnary>(
            genForTackyUnaryOp(tacky.op), genForTackyVal(*tacky.dest)));
        return;
    }

    if (op == UNARY_NOT) {
        // mov 0, dest
        body.emplace_back(std::make_unique<AsmMov>(
            std::make_unique<AsmImm>(0), genForTackyVal(*tacky.dest)));

        // cmp 0, src
        body.emplace_back(std::make_unique<AsmCmp>(std::make_unique<AsmImm>(0),
                                                   genForTackyVal(*tacky.src)));

        // sete dest
        body.emplace_back(std::make_unique<AsmSetCond>(
            AsmSetCond::Code::EQUAL, genForTackyVal(*tacky.dest)));
        return;
    }
}

void AsmGenerator::genForTackyBinary(const TackyBinary& tacky,
                                     Body& body) const {
    // clang-format off
    switch (tacky.op) {
        using enum TackyBinary::Type;
        case BINARY_DIVIDE:     case BINARY_REMAINDER: {
            return genForTackyBinaryDivRem(tacky, body);
        }
        case BINARY_EQUAL:      case BINARY_NOT_EQUAL:
        case BINARY_LESS:       case BINARY_LESS_EQUAL:
        case BINARY_GREATER:    case BINARY_GREATER_EQUAL: {
            return genForTackyBinaryRelational(tacky, body);
        }
        case BINARY_ADD:        case BINARY_SUBTRACT:
        case BINARY_MULTIPLY:   case BINARY_BIT_AND:
        case BINARY_BIT_OR:     case BINARY_BIT_XOR:
        case BINARY_BIT_LSH:    case BINARY_BIT_RSH: 
        case BINARY_LOG_AND:    case BINARY_LOG_OR:  {
            return genForTackyBinaryOther(tacky, body);
        }
    }
    // clang-format on
}

void AsmGenerator::genForTackyBinaryDivRem(const TackyBinary& tacky,
                                           Body& body) const {
    // mov src1, %eax
    body.emplace_back(std::make_unique<AsmMov>(genForTackyVal(*tacky.src1),
                                               genAsmReg(AsmReg::Type::AX)));
    // cdq
    body.emplace_back(std::make_unique<AsmCdq>());

    // idiv src2
    body.emplace_back(std::make_unique<AsmIdiv>(genForTackyVal(*tacky.src2)));

    auto mov2Src = genAsmReg(tacky.op == TackyBinary::Type::BINARY_DIVIDE
                                 ? AsmReg::Type::AX
                                 : AsmReg::Type::DX);
    // mov [%eax | %edx], dest
    body.emplace_back(std::make_unique<AsmMov>(std::move(mov2Src),
                                               genForTackyVal(*tacky.dest)));
}

void AsmGenerator::genForTackyBinaryRelational(const TackyBinary& tacky,
                                               Body& body) const {
    const auto code = genForTackyBinaryRelationalOp(tacky.op);

    // mov 0, dest
    body.emplace_back(std::make_unique<AsmMov>(std::make_unique<AsmImm>(0),
                                               genForTackyVal(*tacky.dest)));

    // cmp src2, src1 |or| RFLAGS = src1 - src2
    body.emplace_back(std::make_unique<AsmCmp>(genForTackyVal(*tacky.src2),
                                               genForTackyVal(*tacky.src1)));

    // set[e|ne|l|le|g|ge] dest
    body.emplace_back(
        std::make_unique<AsmSetCond>(code, genForTackyVal(*tacky.dest)));
}

void AsmGenerator::genForTackyBinaryOther(const TackyBinary& tacky,
                                          Body& body) const {
    // mov src1, dest
    body.emplace_back(std::make_unique<AsmMov>(genForTackyVal(*tacky.src1),
                                               genForTackyVal(*tacky.dest)));

    // binop src2, dest |or| dest = dest binop src2
    body.emplace_back(std::make_unique<AsmBinary>(genForTackyBinaryOp(tacky.op),
                                                  genForTackyVal(*tacky.src2),
                                                  genForTackyVal(*tacky.dest)));
}

void AsmGenerator::genForTackyCopy(const TackyCopy& tacky, Body& body) const {
    // mov src, dest
    body.emplace_back(std::make_unique<AsmMov>(genForTackyVal(*tacky.src),
                                               genForTackyVal(*tacky.dest)));
}

void AsmGenerator::genForTackyJump(const TackyJump& tacky, Body& body) const {
    // jmp label
    body.emplace_back(std::make_unique<AsmJmp>(tacky.target));
}

void AsmGenerator::genForTackyJumpZero(const TackyValPtr& condition,
                                       std::string_view target, bool neg,
                                       Body& body) const {
    using enum AsmJmpCond::Code;

    // cmp 0, condition
    auto left = std::make_unique<AsmImm>(0);
    auto right = genForTackyVal(*condition);
    body.emplace_back(
        std::make_unique<AsmCmp>(std::move(left), std::move(right)));

    // jmpe target
    const auto code = neg ? NOT_EQUAL : EQUAL;
    body.emplace_back(std::make_unique<AsmJmpCond>(code, target));
}

void AsmGenerator::genForTackyLabel(const TackyLabel& tacky, Body& body) const {
    body.emplace_back(std::make_unique<AsmLabel>(tacky.identifier));
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

AsmUnary::Type AsmGenerator::genForTackyUnaryOp(const TackyUnary::Type& type) {
    using Tacky = TackyUnary::Type;
    using Asm = AsmUnary::Type;
    switch (type) {
        case Tacky::UNARY_COMPLEMENT: return Asm::UNARY_COMPLEMENT;
        case Tacky::UNARY_NEGATE:     return Asm::UNARY_NEGATE;
        case Tacky::UNARY_NOT:        {
            fail("Unsupported conversion from TackyUnary::[{}] to AsmUnary:",
                 type);
        }
    }
}

AsmBinary::Type
AsmGenerator::genForTackyBinaryOp(const TackyBinary::Type& type) {
    using Tacky = TackyBinary::Type;
    using Asm = AsmBinary::Type;
    switch (type) {
        case Tacky::BINARY_ADD:       return Asm::BINARY_ADD;
        case Tacky::BINARY_SUBTRACT:  return Asm::BINARY_SUB;
        case Tacky::BINARY_MULTIPLY:  return Asm::BINARY_MULT;
        case Tacky::BINARY_BIT_AND:   return Asm::BINARY_BIT_AND;
        case Tacky::BINARY_BIT_OR:    return Asm::BINARY_BIT_OR;
        case Tacky::BINARY_BIT_XOR:   return Asm::BINARY_BIT_XOR;
        case Tacky::BINARY_BIT_LSH:   return Asm::BINARY_BIT_LSH;
        case Tacky::BINARY_BIT_RSH:   return Asm::BINARY_BIT_RSH;
        case Tacky::BINARY_LOG_AND:   return Asm::BINARY_LOG_AND;
        case Tacky::BINARY_LOG_OR:    return Asm::BINARY_LOG_OR;
        case Tacky::BINARY_DIVIDE:
        case Tacky::BINARY_REMAINDER: {
            fail("Division and remainder should be handled with AsmIdiv.");
        }
        case Tacky::BINARY_EQUAL:
        case Tacky::BINARY_NOT_EQUAL:
        case Tacky::BINARY_LESS:
        case Tacky::BINARY_LESS_EQUAL:
        case Tacky::BINARY_GREATER:
        case Tacky::BINARY_GREATER_EQUAL: {
            fail("Use conditional instructions to handle relational ops.");
        }
    }
}

CondCode
AsmGenerator::genForTackyBinaryRelationalOp(const TackyBinary::Type& type) {
    using Tacky = TackyBinary::Type;
    switch (type) {
        using enum TackyBinary::Type;
        using enum AsmSetCond::Code;

        case BINARY_EQUAL:         return EQUAL;
        case BINARY_NOT_EQUAL:     return NOT_EQUAL;
        case BINARY_LESS:          return LESS;
        case BINARY_LESS_EQUAL:    return LESS_EQUAL;
        case BINARY_GREATER:       return GREATER;
        case BINARY_GREATER_EQUAL: return GREATER_EQUAL;
        default:                   {
            fail("Invalid conversion of relational op TackyBinary::Type[{}]",
                 type);
        }
    }
}

} // namespace wacc::back::gen