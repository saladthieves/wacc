#include "tacky_formatters.hpp"
#include "tacky_ast.hpp"
#include <stdexcept>

namespace wacc::test::fmt {
std::string formatTackyInstr(const TackyInstrPtr& ptr) {
    const auto& type = ptr->type;
    switch (type) {
        using enum TackyNode::Type;
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
        case INSTR_COPY: {
            auto& copy = static_cast<TackyCopy&>(*ptr);
            return formatTackyCopy(copy);
        }
        case INSTR_JUMP: {
            auto& jump = static_cast<TackyJump&>(*ptr);
            return formatTackyJump(jump);
        }
        case INSTR_JUMP_ZERO: {
            auto& jumpZero = static_cast<TackyJumpZero&>(*ptr);
            return formatTackyJumpZero(jumpZero);
        }
        case INSTR_JUMP_NOT_ZERO: {
            auto& jumpNotZero = static_cast<TackyJumpNotZero&>(*ptr);
            return formatTackyJumpNotZero(jumpNotZero);
        }
        case INSTR_LABEL: {
            auto& label = static_cast<TackyLabel&>(*ptr);
            return formatTackyLabel(label);
        }
        default:
            throw std::runtime_error(
                std::format("Unimplemented formatTackyInstr: [{}]", type));
    }
}

namespace {
std::string formatTackyReturn(const TackyReturn& ast) {
    auto val = formatTackyVal(*ast.val);
    return std::format("[R:{}]", val);
}

std::string formatTackyUnary(const TackyUnary& ast) {
    auto op = formatTackyUnaryOp(ast.op);
    auto src = formatTackyVal(*ast.src);
    auto dest = formatTackyVal(*ast.dest);
    return std::format("[{}{} -> {}]", op, src, dest);
}

std::string formatTackyBinary(const TackyBinary& ast) {
    auto op = formatTackyBinaryOp(ast.op);
    auto src1 = formatTackyVal(*ast.src1);
    auto src2 = formatTackyVal(*ast.src2);
    auto dest = formatTackyVal(*ast.dest);
    return std::format("[{} {} {} -> {}]", src1, op, src2, dest);
}

std::string formatTackyCopy(const TackyCopy& ast) {
    auto src = formatTackyVal(*ast.src);
    auto dest = formatTackyVal(*ast.dest);
    return std::format("[{} -> {}]", src, dest);
}

std::string formatTackyJump(const TackyJump& ast) {
    auto target = ast.target.substr(11);
    return std::format("[JMP {}]", target);
}

std::string formatTackyJumpZero(const TackyJumpZero& ast) {
    auto condition = formatTackyVal(*ast.condition);
    auto target = ast.target.substr(11);
    return std::format("[JMPZ {} {}]", condition, target);
}

std::string formatTackyJumpNotZero(const TackyJumpNotZero& ast) {
    auto condition = formatTackyVal(*ast.condition);
    auto target = ast.target.substr(11);
    return std::format("[JMPNZ {} {}]", condition, target);
}

std::string formatTackyLabel(const TackyLabel& ast) {
    auto identifier = ast.identifier.substr(11);
    return std::format("[LBL {}]", identifier);
}

std::string formatTackyVal(const TackyVal& ast) {
    const auto& type = ast.type;
    switch (type) {
        case LITERAL_INT: {
            auto& con = static_cast<const TackyLitInt&>(ast);
            return std::to_string(con.value);
        }
        case VARIABLE: {
            auto& var = static_cast<const TackyVariable&>(ast);
            return var.identifier.substr(6);
        }
        default: {
            throw std::runtime_error(
                std::format("Unimplemented formatTackyVal: [{}]", type));
        }
    }
}

std::string formatTackyUnaryOp(const TackyUnary::Type& type) {
    switch (type) {
        using enum TackyUnary::Type;
        case UNARY_COMPLEMENT: return "~";
        case UNARY_NEGATE:     return "-";
        case UNARY_NOT:        return "!";
    }
}

std::string formatTackyBinaryOp(const TackyBinary::Type& type) {
    switch (type) {
        using enum TackyBinary::Type;
        case BINARY_ADD:       return "+";
        case BINARY_SUBTRACT:  return "-";
        case BINARY_MULTIPLY:  return "*";
        case BINARY_DIVIDE:    return "/";
        case BINARY_REMAINDER: return "%";
        case BINARY_BIT_AND:   return "&";
        case BINARY_BIT_OR:    return "|";
        case BINARY_BIT_XOR:   return "^";
        case BINARY_BIT_LSH:   return "<<";
        case BINARY_BIT_RSH:   return ">>";
        case BINARY_LOG_AND:
        case BINARY_LOG_OR:    {
            throw std::runtime_error("BINARY_LOG_AND / BINARY_LOG_OR used "
                                     "directly in a TackyBinary.");
        }
        case BINARY_EQUAL:         return "==";
        case BINARY_NOT_EQUAL:     return "!=";
        case BINARY_LESS:          return "<";
        case BINARY_LESS_EQUAL:    return "<=";
        case BINARY_GREATER:       return ">";
        case BINARY_GREATER_EQUAL: return ">=";
    }
}
} // namespace
} // namespace wacc::test::fmt