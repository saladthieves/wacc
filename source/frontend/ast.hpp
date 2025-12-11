#pragma once

#include "token.hpp"

#include <format>

namespace wacc {
namespace front {
namespace ast {
enum class AstNodeType {
    EXPRESSION,
    INTEGER,
    IDENTIFIER,
    STATEMENT,
    RETURN,
    FUNCTION,
    PROGRAM
};

// Forward Declarations
class AstNode;
class AstExpr;
class AstInt;
class AstIdent;
class AstStmt;
class AstReturn;
class AstFun;
class AstProg;

// Aliases
using AstNodePtr = std::unique_ptr<AstNode>;
using AstExprPtr = std::unique_ptr<AstExpr>;
using AstIntPtr = std::unique_ptr<AstInt>;
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
    virtual AstNodeType type() const override { return EXPRESSION; };
};

// AstInt
class AstInt : public AstExpr {
public:
    AstInt(Token token, int value);

    virtual AstNodeType type() const override { return INTEGER; };

    Token token;
    int value;
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
    virtual AstNodeType type() const override { return STATEMENT; };
};

// AstReturn
class AstReturn : public AstStmt {
public:
    AstReturn(AstExprPtr expression);

    virtual AstNodeType type() const override { return RETURN; };

    AstExprPtr expression;
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

            case EXPRESSION: value = "EXPRESSION"; break;
            case INTEGER:    value = "INTEGER"; break;
            case IDENTIFIER: value = "IDENTIFIER"; break;
            case STATEMENT:  value = "STATEMENT"; break;
            case RETURN:     value = "RETURN"; break;
            case FUNCTION:   value = "FUNCTION"; break;
            case PROGRAM:    value = "PROGRAM"; break;
            default:         throw std::format_error("Unhandled ast::AstNodeType enum");
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

    auto format(const AstNodePtr& ptr, format_context& context) const {
        const auto indent = [](unsigned level) {
            std::string output = "";
            for (auto i = 0; i < level; ++i) output += " ";
            return output;
        };

        const auto fmt = [&indent](this const auto& self, const AstNode& node,
                                   unsigned level) -> std::string {
            const auto& type = node.type();
            switch (type) {
                using enum AstNodeType;
                case INTEGER: {
                    const auto& integer = static_cast<const AstInt&>(node);
                    auto in = indent(level);
                    return std::format("{}AstInt [value = '{}']\n", in,
                                       integer.value);
                }
                case IDENTIFIER: {
                    const auto& ident = static_cast<const AstIdent&>(node);
                    auto in = indent(level);
                    std::string output = in + "AstIdent {\n";
                    output += std::format("{}{}{}", in, in, ident.value);
                    output += in + "}\n";
                    return output;
                }
                case RETURN: {
                    const auto& ret = static_cast<const AstReturn&>(node);
                    auto in = indent(level);
                    std::string output = in + "AstReturn {\n";
                    output += self(*ret.expression, level + 1);
                    output += in + "}\n";
                    return output;
                }
                case FUNCTION: {
                    const auto& fun = static_cast<const AstFun&>(node);
                    auto in = indent(level);
                    auto output = std::format("{}AstFun name='{}', body = {{\n",
                                              in, fun.name->value);
                    output += self(*fun.body, level + 1);
                    output += in + "}\n";
                    return output;
                }
                case PROGRAM: {
                    const auto& prog = static_cast<const AstProg&>(node);
                    auto in = indent(level);
                    std::string output = in + "AstProg {\n";
                    output += self(*prog.function, level + 1);
                    output += in + "}\n";
                    return output;
                }
                default:
                    throw std::runtime_error("Unhandled ast::AstNode type.");
            }
        };

        const auto value = fmt(*ptr, 0);
        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std