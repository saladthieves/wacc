#include "tacky_formatters.hpp"
#include "tacky_ast.hpp"

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

std::string formatTackyVal(const TackyVal& ast) {
    const auto& type = ast.type;
    switch (type) {
        case VARIABLE: {
            auto& var = static_cast<const TackyVariable&>(ast);
            return var.identifier.substr(6);
        }
        case LITERAL_INT: {
            auto& con = static_cast<const TackyLitInt&>(ast);
            return std::to_string(con.value);
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
        default:               throw std::runtime_error("Unimplemented formatTackyUnaryOp");
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

        default: throw std::runtime_error("Unimplemented formatTackyBinaryOp");
    }
}
} // namespace
} // namespace wacc::test::fmt