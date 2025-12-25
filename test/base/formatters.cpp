#include "formatters.hpp"
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

} // namespace
} // namespace wacc::test::fmt