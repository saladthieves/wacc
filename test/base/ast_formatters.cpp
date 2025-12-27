#include "ast_formatters.hpp"

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

namespace {
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
} // namespace
} // namespace wacc::test::fmt