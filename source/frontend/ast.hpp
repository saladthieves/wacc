#pragma once

#include "token.hpp"

#include <format>
#include <vector>

namespace wacc {
namespace front {
namespace ast {
enum class AstNodeType : unsigned {
    CONST_INTEGER = 1,
    UNARY,
    IDENTIFIER,
    RETURN,
    FUNCTION,
    PROGRAM
};

enum class AstUnaryOpType : unsigned {
    UNARY_COMPLEMENT = 1,
    UNARY_NEGATE,
};

// Forward Declarations
class AstNode;
class AstExpr;
class AstConstInt;
class AstUnary;
class AstIdent;
class AstStmt;
class AstReturn;
class AstFun;
class AstProg;

// Aliases
using AstNodePtr = std::unique_ptr<AstNode>;
using AstExprPtr = std::unique_ptr<AstExpr>;
using AstConstIntPtr = std::unique_ptr<AstConstInt>;
using AstUnaryPtr = std::unique_ptr<AstUnary>;
using AstIdentPtr = std::unique_ptr<AstIdent>;
using AstStmtPtr = std::unique_ptr<AstStmt>;
using AstReturnPtr = std::unique_ptr<AstReturn>;
using AstFunPtr = std::unique_ptr<AstFun>;
using AstProgPtr = std::unique_ptr<AstProg>;

namespace {
using Token = token::Token;
using enum AstNodeType;
} // namespace

// CLASS DECLARATIONS
// AstNode
class AstNode {
public:
    virtual AstNodeType type() const = 0;

    virtual ~AstNode() = default;
};

// AstExpr
class AstExpr : public AstNode {
public:
    virtual AstNodeType type() const override = 0;
};

// AstConstInt
class AstConstInt : public AstExpr {
public:
    AstConstInt(Token token, int value);

    virtual AstNodeType type() const override { return CONST_INTEGER; };

    Token token;
    int value;
};

// AstUnary
class AstUnary : public AstExpr {
public:
    AstUnary(AstUnaryOpType op, AstExprPtr expr);

    virtual AstNodeType type() const override { return UNARY; };

    AstUnaryOpType op;
    AstExprPtr expr;
};

// AstIdent
class AstIdent : public AstNode {
public:
    AstIdent(Token token, std::string_view value);

    virtual AstNodeType type() const override { return IDENTIFIER; };

    Token token;
    std::string_view value;
};

// AstStmt
class AstStmt : public AstNode {
public:
    virtual AstNodeType type() const override = 0;
};

// AstReturn
class AstReturn : public AstStmt {
public:
    AstReturn(AstExprPtr expr);

    virtual AstNodeType type() const override { return RETURN; };

    AstExprPtr expr;
};

// AstFun
class AstFun : public AstNode {
public:
    AstFun(AstIdentPtr name, AstStmtPtr body);

    virtual AstNodeType type() const override { return FUNCTION; };

    AstIdentPtr name;
    AstStmtPtr body;
};

// AstProg
class AstProg : public AstNode {
public:
    AstProg(AstFunPtr function);

    virtual AstNodeType type() const override { return PROGRAM; };

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
using wacc::front::ast::AstNodeType;
}

template <>
class formatter<AstNodeType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AstNodeType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AstNodeType;
            case CONST_INTEGER: value = "CONST_INTEGER"; break;
            case IDENTIFIER:    value = "IDENTIFIER"; break;
            case RETURN:        value = "RETURN"; break;
            case FUNCTION:      value = "FUNCTION"; break;
            case PROGRAM:       value = "PROGRAM"; break;
            default:
                throw std::format_error(
                    "Unhandled front::ast::AstNodeType enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::front::ast::AstUnaryOpType;
}

template <>
class formatter<AstUnaryOpType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AstUnaryOpType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AstUnaryOpType;
            case UNARY_COMPLEMENT: value = "UNARY_COMPLEMENT"; break;
            case UNARY_NEGATE:     value = "UNARY_NEGATE"; break;
            default:
                throw std::format_error(
                    "Unhandled front::ast::AstUnaryOpType enum");
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

        const auto& type = node.type();
        const auto in = indent(level);
        switch (type) {
            using enum AstNodeType;
            case CONST_INTEGER: {
                const auto& integer = static_cast<const AstConstInt&>(node);
                return std::format("{}AstInt [value = '{}']\n", in,
                                   integer.value);
            }
            case UNARY: {
                const auto& unary = static_cast<const AstUnary&>(node);
                auto output =
                    std::format("{}AstUnary [operator = {}]\n", in, unary.op);
                output += formatNode(*unary.expr, ++level);
                output += in + "}\n";
                return output;
            }
            case IDENTIFIER: {
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

            default: throw std::runtime_error("Unhandled ast::AstNode type.");
        }
    }

    auto format(const AstNodePtr& ptr, format_context& context) const {
        return std::format_to(context.out(), "{}", formatNode(*ptr, 0));
    }
};
} // namespace std