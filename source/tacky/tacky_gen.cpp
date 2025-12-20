#include "tacky_gen.hpp"
#include "ast.hpp"
#include "tacky_ast.hpp"

#include <memory>

namespace wacc::tacky::gen {
// VariableGenerator
VariableGenerator::VariableGenerator() {
    reset();
}

void VariableGenerator::reset() {
    resetSession();
    count = 0;
}

void VariableGenerator::resetSession(std::string session) {
    std::string value{"S"};
    if (session.empty()) {
        constexpr auto letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (int i = 0; i < 4; ++i) {
            value += letters[std::rand() % 36];
        }
    } else {
        value = session;
    }

    this->session = value;
}

void VariableGenerator::resetFunction(std::string function) {
    std::string upper{};
    for (const auto& c : function) upper += std::toupper(c);
    this->function = upper;
}

std::string VariableGenerator::generate(TackyUnaryOpType type) {
    return std::format("{}.{}.{}.TEMP.{}", session, function, type, count++);
}

// TackyGenerator
TackyGenerator::TackyGenerator(AstNodePtr ptr) : ast{std::move(ptr)} {
}

TackyNodePtr TackyGenerator::generate() const {
    if (!ast) fail("The AstNode root tree is null");

    auto& prog = static_cast<const AstProg&>(*ast);
    return genForAstProg(prog);
}

TackyProgPtr TackyGenerator::genForAstProg(const AstProg& obj) const {
    auto& fun = static_cast<const AstFun&>(*obj.function);
    return std::make_unique<TackyProg>(genForAstFun(fun));
}

TackyFunPtr TackyGenerator::genForAstFun(const AstFun& obj) const {
    auto identifier = std::string{obj.name->value};
    generator.reset();
    generator.resetFunction(identifier);
    auto& stmt = static_cast<const AstStmt&>(*obj.body);
    return std::make_unique<TackyFun>(identifier, genForAstStmt(stmt));
}

TackyInstrs TackyGenerator::genForAstStmt(const AstStmt& obj) const {
    const auto& type = obj.type();
    auto body = TackyInstrs{};
    switch (type) {
        using enum AstNodeType;
        case RETURN: {
            auto& retObj = static_cast<const AstReturn&>(obj);
            genForAstReturn(retObj, body);
            break;
        }

        default: {
            fail("Failed to generate instruction from AstStmt::[type = {}].",
                 type);
        }
    }

    return body;
}

void TackyGenerator::genForAstReturn(const AstReturn& obj,
                                     TackyInstrs& body) const {
    auto val = genForAstExpr(*obj.expr, body);
    body.emplace_back(std::make_unique<TackyReturn>(std::move(val)));
}

TackyValPtr TackyGenerator::genForAstExpr(const AstExpr& obj,
                                          TackyInstrs& body) const {
    const auto& type = obj.type();
    switch (type) {
        using enum AstNodeType;
        case CONST_INTEGER: {
            auto& it = static_cast<const AstConstInt&>(obj);
            return std::make_unique<TackyConstant>(genForAstConstInt(it));
        }
        case UNARY: {
            auto& unary = static_cast<const AstUnary&>(obj);
            return genForAstUnary(unary, body);
        }

        default:
            fail("Could not generate TackyVal for AstNodeType::[{}]", type);
    }
}

TackyValPtr TackyGenerator::genForAstUnary(const AstUnary& obj,
                                           TackyInstrs& body) const {
    auto op = genForAstUnaryOp(obj.op);
    auto src = genForAstExpr(*obj.expr, body);
    auto name = generator.generate(op);
    auto dest = std::make_unique<TackyVariable>(name);
    auto unary =
        std::make_unique<TackyUnary>(op, std::move(src), std::move(dest));
    body.push_back(std::move(unary));

    return std::make_unique<TackyVariable>(name);
}

TackyConstant TackyGenerator::genForAstConstInt(const AstConstInt& obj) const {
    return {obj.value};
}

TackyUnaryOpType
TackyGenerator::genForAstUnaryOp(const AstUnaryOpType& type) const {
    switch (type) {
        using enum AstUnaryOpType;
        case UNARY_COMPLEMENT: return TackyUnaryOpType::UNARY_COMPLEMENT;
        case UNARY_NEGATE:     return TackyUnaryOpType::UNARY_NEGATE;
        default:
            fail("Conversion from AstUnaryOpType::[{}] to TackyUnaryOpType "
                 "failed:",
                 type);
    }
}

} // namespace wacc::tacky::gen