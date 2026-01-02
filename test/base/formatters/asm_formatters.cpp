#include "asm_formatters.hpp"

namespace wacc::test::fmt {
std::string formatAsmInstr(const AsmInstrPtr& ptr) {
    const auto& type = ptr->type;
    switch (type) {
        using enum AsmNode::Type;
        case INSTR_MOV: {
            auto& mov = static_cast<AsmMov&>(*ptr);
            return formatAsmMov(mov);
        }
        case INSTR_UNARY: {
            auto& unary = static_cast<AsmUnary&>(*ptr);
            return formatAsmUnary(unary);
        }
        case INSTR_BINARY: {
            auto& binary = static_cast<AsmBinary&>(*ptr);
            return formatAsmBinary(binary);
        }
        case INSTR_IDIV: {
            auto& idiv = static_cast<AsmIdiv&>(*ptr);
            return formatAsmIdiv(idiv);
        }
        case INSTR_CDQ: {
            return formatAsmCdq();
        }
        case INSTR_ALLOC: {
            auto& alloc = static_cast<AsmAllocStack&>(*ptr);
            return formatAsmAllocStack(alloc);
        }
        case INSTR_RET: {
            return formatAsmRet();
        }
        default:
            throw std::runtime_error(
                std::format("Unimplemented formatAsmInstr: [{}]", type));
    }
}

namespace {
std::string formatAsmOperand(const AsmOperand& op) {
    const auto& type = op.type;
    switch (type) {
        using enum AsmNode::Type;
        case OP_IMM: {
            auto imm = static_cast<const AsmImm&>(op);
            return std::to_string(imm.value);
        }
        case OP_REG: {
            auto reg = static_cast<const AsmReg&>(op);
            return formatAsmRegister(reg.reg);
        }
        case OP_PSEUDO: {
            auto pseudo = static_cast<const AsmPseudo&>(op);
            return pseudo.identifier.substr(6);
        }
        case OP_STACK: {
            auto stack = static_cast<const AsmStack&>(op);
            return std::to_string(stack.value);
        }
        default: {
            throw std::runtime_error("Unimplemented formatAstOperand");
        }
    }
}

std::string formatAsmRegister(const AsmReg::Type& type) {
    switch (type) {
        using enum AsmReg::Type;
        case AX:  return "AX";
        case DX:  return "DX";
        case R10: return "R10";
        case R11: return "R11";
        default:  throw std::runtime_error("Unimplemented formatAsmRegister");
    }
}

std::string formatAsmMov(const AsmMov& mov) {
    auto src = formatAsmOperand(*mov.src);
    auto dest = formatAsmOperand(*mov.dest);
    return std::format("<MOV S:{} D:{}>", src, dest);
}

std::string formatAsmUnary(const AsmUnary& unary) {
    auto op = formatAsmUnaryOp(unary.op);
    auto operand = formatAsmOperand(*unary.operand);
    return std::format("<UNARY O:{} O:{}>", op, operand);
}

std::string formatAsmBinary(const AsmBinary& binary) {
    auto op = formatAsmBinaryOp(binary.op);
    auto src = formatAsmOperand(*binary.src);
    auto dest = formatAsmOperand(*binary.dest);
    return std::format("<BINARY S:{} O:{} D:{}>", src, op, dest);
}

std::string formatAsmIdiv(const AsmIdiv& idiv) {
    auto operand = formatAsmOperand(*idiv.operand);
    return std::format("<IDIV O:{}>", operand);
}

std::string formatAsmCdq() {
    return "<CDQ>";
}

std::string formatAsmAllocStack(const AsmAllocStack& stack) {
    return std::format("<ALLOC O:{} >", stack.value);
}

std::string formatAsmRet() {
    return "<RET>";
}

std::string formatAsmUnaryOp(const AsmUnary::Type& type) {
    switch (type) {
        using enum AsmUnary::Type;
        case UNARY_NEGATE: return "NEG";
        case UNARY_NOT:    return "NOT";
        default:           throw std::runtime_error("Unimplemented formatAsmUnaryOp");
    }
}

std::string formatAsmBinaryOp(const AsmBinary::Type& type) {
    switch (type) {
        using enum AsmBinary::Type;
        case BINARY_ADD:  return "ADD";
        case BINARY_SUB:  return "SUB";
        case BINARY_MULT: return "MULT";
        default:          throw std::runtime_error("Unimplemented formatAsmBinaryOp");
    }
}
} // namespace
} // namespace wacc::test::fmt