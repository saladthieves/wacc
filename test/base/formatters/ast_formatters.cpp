#include "ast_formatters.hpp"
#include "ast.hpp"

namespace wacc::test::fmt {
std::string formatAstExpr(const AstExprPtr& ptr) {
    const auto& type = ptr->type;
    using enum AstNode::Type;
    switch (type) {
        case LITERAL_INT: {
            auto& ast = static_cast<const AstLitInt&>(*ptr);
            return formatAstLitInt(ast);
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
std::string formatAstLitInt(const AstLitInt& ast) {
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

std::string formatAstUnaryOp(const AstUnary::Type& type) {
    switch (type) {
        using enum AstUnary::Type;
        case UNARY_COMPLEMENT: return "~";
        case UNARY_NEGATE:     return "-";
        case UNARY_NOT:        return "!";
    }
}

std::string formatAstBinaryOp(const AstBinary::Type& type) {
    switch (type) {
        using enum AstBinary::Type;
        case BINARY_ADD:           return "+";
        case BINARY_SUBTRACT:      return "-";
        case BINARY_MULTIPLY:      return "*";
        case BINARY_DIVIDE:        return "/";
        case BINARY_REMAINDER:     return "%";
        case BINARY_BIT_AND:       return "&";
        case BINARY_BIT_OR:        return "|";
        case BINARY_BIT_XOR:       return "^";
        case BINARY_BIT_LSH:       return "<<";
        case BINARY_BIT_RSH:       return ">>";
        case BINARY_LOG_AND:       return "&&";
        case BINARY_LOG_OR:        return "||";
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