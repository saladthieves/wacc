#include "ast.hpp"

#include <format>
#include <string>

namespace wacc::core::ast {

namespace {
std::string indent(unsigned level) {
    std::string output = "";
    for (auto i = 0; i < level; ++i) output += INDENT;
    return output;
}
} // namespace

// AstInt
AstInt::AstInt(Token token, int value) : token{token}, value{value} {
}

std::string AstInt::format(unsigned level) const {
    auto in = indent(level);
    return std::format("{}AstInt [value = '{}']\n", in, value);
}

// AstIdent
AstIdent::AstIdent(Token token, std::string_view value) :
    token{token}, value{value} {
}

std::string AstIdent::format(unsigned level) const {
    auto in = indent(level);
    std::string output = in + "AstIdent {\n";
    output += std::format("{}{}{}", in, in, value);
    output += in + "}\n";

    return output;
}

// AstReturn
AstReturn::AstReturn(AstExprPtr expression) :
    expression{std::move(expression)} {
}

std::string AstReturn::format(unsigned level) const {
    auto in = indent(level);
    std::string output = in + "AstReturn {\n";
    output += expression->format(level + 1);
    output += in + "}\n";

    return output;
}

// AstFun
AstFun::AstFun(AstIdentPtr name, AstStmtPtr body) :
    name{std::move(name)}, body{std::move(body)} {
}

std::string AstFun::format(unsigned level) const {
    auto in = indent(level);
    auto output =
        std::format("{}AstFun name='{}', body = {{\n", in, name->value);
    output += body->format(level + 1);
    output += in + "}\n";

    return output;
}

// AstProg
AstProg::AstProg(AstFunPtr function) : function{std::move(function)} {
}

std::string AstProg::format(unsigned level) const {
    std::string output = "AstProg {\n";
    output += function->format(level + 1);
    output += "}\n";

    return output;
}

// namespace
} // namespace wacc::core::ast