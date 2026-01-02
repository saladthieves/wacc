#pragma once

#include "token.hpp"

#include <cstdint>
#include <format>
#include <vector>

namespace wacc {
namespace front {
namespace ast {
// Forward Declarations
class AstNode;
class AstExpr;
class AstLitInt;
class AstUnary;
class AstBinary;
class AstIdent;
class AstStmt;
class AstReturn;
class AstFun;
class AstProg;

// Aliases
using AstNodePtr = std::unique_ptr<AstNode>;
using AstExprPtr = std::unique_ptr<AstExpr>;
using AstLitIntPtr = std::unique_ptr<AstLitInt>;
using AstUnaryPtr = std::unique_ptr<AstUnary>;
using AstBinaryPtr = std::unique_ptr<AstBinary>;
using AstIdentPtr = std::unique_ptr<AstIdent>;
using AstStmtPtr = std::unique_ptr<AstStmt>;
using AstReturnPtr = std::unique_ptr<AstReturn>;
using AstFunPtr = std::unique_ptr<AstFun>;
using AstProgPtr = std::unique_ptr<AstProg>;

// CLASS DECLARATIONS
// AstNode
class AstNode {
public:
    enum class Type : std::uint8_t {
        LITERAL_INT = 1,
        UNARY,
        BINARY,
        IDENT,
        RETURN,
        FUNCTION,
        PROGRAM
    };

    AstNode(Type type);

    virtual ~AstNode() = default;

    Type type;
};

namespace {
using Token = token::Token;
using enum AstNode::Type;
} // namespace

// AstExpr
class AstExpr : public AstNode {
public:
    AstExpr(Type type);
};

// AstLitInt
class AstLitInt : public AstExpr {
public:
    AstLitInt(Token token, int value);

    Token token;
    int value;
};

// AstUnary
class AstUnary : public AstExpr {
public:
    enum class Type : std::uint8_t {
        UNARY_COMPLEMENT = 1,
        UNARY_NEGATE,
    };

    AstUnary(Type op, AstExprPtr expr);

    Type op;
    AstExprPtr expr;
};

// AstBinary
class AstBinary : public AstExpr {
public:
    enum class Type : std::uint8_t {
        BINARY_ADD = 1,
        BINARY_SUBTRACT,
        BINARY_MULTIPLY,
        BINARY_DIVIDE,
        BINARY_REMAINDER,
    };

    AstBinary(Type op, AstExprPtr left, AstExprPtr right);

    Type op;
    AstExprPtr left;
    AstExprPtr right;
};

// AstIdent
class AstIdent : public AstNode {
public:
    AstIdent(Token token, std::string_view value);

    Token token;
    std::string_view value;
};

// AstStmt
class AstStmt : public AstNode {
public:
    AstStmt(Type type);
};

// AstReturn
class AstReturn : public AstStmt {
public:
    AstReturn(AstExprPtr expr);

    AstExprPtr expr;
};

// AstFun
class AstFun : public AstNode {
public:
    AstFun(AstIdentPtr name, AstStmtPtr body);

    AstIdentPtr name;
    AstStmtPtr body;
};

// AstProg
class AstProg : public AstNode {
public:
    AstProg(AstFunPtr function);

    AstFunPtr function;
};

// AstTree
class AstTree {
    using ConstIter = std::string_view::const_iterator;
    using Tokens = std::vector<token::Token>;
    using TokensPtr = std::unique_ptr<Tokens>;

public:
    AstTree(ConstIter begin, ConstIter end, TokensPtr root);

    ConstIter begin;
    ConstIter end;
    TokensPtr root;
};
} // namespace ast
} // namespace front
} // namespace wacc

namespace std {
namespace {
using wacc::front::ast::AstNode;
}

template <>
class formatter<AstNode::Type> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AstNode::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AstNode::Type;
            case LITERAL_INT: value = "LITERAL_INT"; break;
            case UNARY:       value = "UNARY"; break;
            case BINARY:      value = "BINARY"; break;
            case IDENT:       value = "IDENT"; break;
            case RETURN:      value = "RETURN"; break;
            case FUNCTION:    value = "FUNCTION"; break;
            case PROGRAM:     value = "PROGRAM"; break;
            default:          throw std::format_error("Unhandled AstNode::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::front::ast::AstUnary;
}

template <>
class formatter<AstUnary::Type> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AstUnary::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AstUnary::Type;
            case UNARY_COMPLEMENT: value = "UNARY_COMPLEMENT"; break;
            case UNARY_NEGATE:     value = "UNARY_NEGATE"; break;
            default:               throw std::format_error("Unhandled AstUnary::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::front::ast::AstBinary;
}

template <>
class formatter<AstBinary::Type> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AstBinary::Type& type, format_context& context) const {
        string value{};

        switch (type) {
            using enum AstBinary::Type;
            case BINARY_ADD:       value = "BINARY_ADD"; break;
            case BINARY_SUBTRACT:  value = "BINARY_SUBTRACT"; break;
            case BINARY_MULTIPLY:  value = "BINARY_MULTIPLY"; break;
            case BINARY_DIVIDE:    value = "BINARY_DIVIDE"; break;
            case BINARY_REMAINDER: value = "BINARY_REMAINDER"; break;
            default:               {
                throw std::format_error("Unhandled AstBinary::Type enum");
            }
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using namespace wacc::front::ast;
} // namespace

template <>
class formatter<AstNodePtr> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    std::string formatNode(const AstNode& node, unsigned level) const {
        const auto indent = [](unsigned level) -> std::string {
            std::string output = "";
            for (auto i = 0; i < level; ++i) output += "  ";
            return output;
        };

        const auto in = indent(level);
        switch (node.type) {
            using enum AstNode::Type;
            case LITERAL_INT: {
                const auto& integer = static_cast<const AstLitInt&>(node);
                return std::format("{}AstLitInt [value = '{}']\n", in,
                                   integer.value);
            }
            case UNARY: {
                const auto& unary = static_cast<const AstUnary&>(node);
                auto output =
                    std::format("{}AstUnary [operator = {}]\n", in, unary.op);
                output += formatNode(*unary.expr, level + 1);
                output += in + "}\n";
                return output;
            }
            case BINARY: {
                const auto& binary = static_cast<const AstBinary&>(node);
                auto output =
                    std::format("{}AstBinary [operator = {}]\n", in, binary.op);
                output += formatNode(*binary.left, level + 1);
                output += formatNode(*binary.right, level + 1);
                output += in + "}\n";
                return output;
            }
            case IDENT: {
                const auto& ident = static_cast<const AstIdent&>(node);
                std::string output = in + "AstIdent {\n";
                output += std::format("{}{}{}", in, in, ident.value);
                output += in + "}\n";
                return output;
            }
            case RETURN: {
                const auto& ret = static_cast<const AstReturn&>(node);
                std::string output = in + "AstReturn {\n";
                output += formatNode(*ret.expr, level + 1);
                output += in + "}\n";
                return output;
            }
            case FUNCTION: {
                const auto& fun = static_cast<const AstFun&>(node);
                auto output = std::format("{}AstFun name='{}', body = {{\n", in,
                                          fun.name->value);
                output += formatNode(*fun.body, level + 1);
                output += in + "}\n";
                return output;
            }
            case PROGRAM: {
                const auto& prog = static_cast<const AstProg&>(node);
                std::string output = in + "AstProg {\n";
                output += formatNode(*prog.function, level + 1);
                output += in + "}\n";
                return output;
            }

            default: throw std::runtime_error("Unhandled AstNode::Type type");
        }
    }

    auto format(const AstNodePtr& ptr, format_context& context) const {
        return std::format_to(context.out(), "{}", formatNode(*ptr, 0));
    }
};
} // namespace std