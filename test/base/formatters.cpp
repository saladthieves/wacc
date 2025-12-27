#include "formatters.hpp"
#include "asm_ast.hpp"
#include "ast.hpp"
#include "tacky_ast.hpp"

#include <format>
#include <stdexcept>
#include <string>

namespace wacc::test::fmt {
std::string formatAstExpr(const AstExprPtr& ptr) {
    const auto& type = ptr->type();
    using enum AstNodeType;
    switch (type) {
        case CONST_INTEGER: {
            auto& ast = static_cast<const AstConstInt&>(*ptr);
            return formatAstConstInt(ast);
        }
        case UNARY: {
            auto& ast = static_cast<const AstUnary&>(*ptr);
            return formatAstUnary(ast);
        }
        case BINARY: {
            auto& ast = static_cast<const AstBinary&>(*ptr);
            return formatAstBinary(ast);
        }

        default:
            throw std::runtime_error(
                std::format("Unimplemented formatAstExpr: [{}]", type));
    }
}

std::string formatTackyInstr(const TackyInstrPtr& ptr) {
    const auto& type = ptr->type();
    switch (type) {
        using enum TackyNodeType;
        case INSTR_RETURN: {
            auto& ast = static_cast<TackyReturn&>(*ptr);
            return formatTackyReturn(ast);
        }
        case INSTR_UNARY: {
            auto& unary = static_cast<TackyUnary&>(*ptr);
            return formatTackyUnary(unary);
        }
        case INSTR_BINARY: {
            auto& binary = static_cast<TackyBinary&>(*ptr);
            return formatTackyBinary(binary);
        }
        default:
            throw std::runtime_error(
                std::format("Unimplemented formatTackyInstr: [{}]", type));
    }
}

std::string formatAsmInstr(const AsmInstrPtr& ptr) {
    const auto& type = ptr->type();
    switch (type) {
        using enum AsmNodeType;
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
// Ast
std::string formatAstConstInt(const AstConstInt& ast) {
    return std::to_string(ast.value);
}

std::string formatAstUnary(const AstUnary& ast) {
    auto op = formatAstUnaryOp(ast.op);
    auto expr = formatAstExpr(ast.expr);
    return std::format("[{}{}]", op, expr);
}

std::string formatAstBinary(const AstBinary& ast) {
    auto left = formatAstExpr(ast.left);
    auto op = formatAstBinaryOp(ast.op);
    auto right = formatAstExpr(ast.right);
    return std::format("[{} {} {}]", left, op, right);
}

std::string formatAstUnaryOp(const AstUnaryOpType& type) {
    switch (type) {
        using enum AstUnaryOpType;
        case UNARY_COMPLEMENT: return "~";
        case UNARY_NEGATE:     return "-";
    }
}

std::string formatAstBinaryOp(const AstBinaryOpType& type) {
    switch (type) {
        using enum AstBinaryOpType;
        case BINARY_ADD:       return "+";
        case BINARY_SUBTRACT:  return "-";
        case BINARY_MULTIPLY:  return "*";
        case BINARY_DIVIDE:    return "/";
        case BINARY_REMAINDER: return "%";
    }
}

// Tacky
std::string formatTackyReturn(const TackyReturn& ast) {
    auto val = formatTackyVal(*ast.val);
    return std::format("[R:{}]", val);
}

std::string formatTackyUnary(const TackyUnary& ast) {
    auto op = formatTackyUnaryOp(ast.op);
    auto src = formatTackyVal(*ast.src);
    auto dest = formatTackyVal(*ast.dest);
    return std::format("[O:{} S:{} D:{}]", op, src, dest);
}

std::string formatTackyBinary(const TackyBinary& ast) {
    auto op = formatTackyBinaryOp(ast.op);
    auto src1 = formatTackyVal(*ast.src1);
    auto src2 = formatTackyVal(*ast.src2);
    auto dest = formatTackyVal(*ast.dest);
    return std::format("[S1:{} O:{} S2:{} D:{}]", src1, op, src2, dest);
}

std::string formatTackyVal(const TackyVal& ast) {
    const auto& type = ast.type();
    switch (type) {
        using enum AstNodeType;
        case VARIABLE: {
            auto& var = static_cast<const TackyVariable&>(ast);
            return var.identifier.substr(6);
        }
        case CONSTANT: {
            auto& con = static_cast<const TackyConstant&>(ast);
            return std::to_string(con.value);
        }
        default: {
            throw std::runtime_error(
                std::format("Unimplemented formatTackyVal: [{}]", type));
        }
    }
}

std::string formatTackyUnaryOp(const TackyUnaryOpType& type) {
    switch (type) {
        using enum TackyUnaryOpType;
        case UNARY_COMPLEMENT: return "~";
        case UNARY_NEGATE:     return "-";
        default:               throw std::runtime_error("Unimplemented formatTackyUnaryOp");
    }
}

std::string formatTackyBinaryOp(const TackyBinaryOpType& type) {
    switch (type) {
        using enum TackyBinaryOpType;
        case BINARY_ADD:       return "+";
        case BINARY_SUBTRACT:  return "-";
        case BINARY_MULTIPLY:  return "*";
        case BINARY_DIVIDE:    return "/";
        case BINARY_REMAINDER: return "%";
        default:               throw std::runtime_error("Unimplemented formatTackyBinaryOp");
    }
}

// Asm
std::string formatAsmOperand(const AsmOperand& op) {
    const auto& type = op.type();
    switch (type) {
        using enum AsmNodeType;
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

std::string formatAsmRegister(const AsmRegisterType& type) {
    switch (type) {
        using enum AsmRegisterType;
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

std::string formatAsmUnaryOp(const AsmUnaryOpType& type) {
    switch (type) {
        using enum AsmUnaryOpType;
        case UNARY_NEGATE: return "NEG";
        case UNARY_NOT:    return "NOT";
        default:           throw std::runtime_error("Unimplemented formatAsmUnaryOp");
    }
}

std::string formatAsmBinaryOp(const AsmBinaryOpType& type) {
    switch (type) {
        using enum AsmBinaryOpType;
        case BINARY_ADD:  return "ADD";
        case BINARY_SUB:  return "SUB";
        case BINARY_MULT: return "MULT";
        default:          throw std::runtime_error("Unimplemented formatAsmBinaryOp");
    }
}

} // namespace
} // namespace wacc::test::fmt