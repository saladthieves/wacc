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

void VariableGenerator::resetSession(std::string_view session) {
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

void VariableGenerator::resetFunction(std::string_view function) {
    std::string upper{};
    for (const auto& c : function) upper += std::toupper(c);
    this->function = upper;
}

std::string VariableGenerator::generate() {
    return std::format("{}.{}.TEMP.{}", session, function, count++);
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
    return std::make_unique<TackyProg>(genForAstFun(*obj.function));
}

TackyFunPtr TackyGenerator::genForAstFun(const AstFun& obj) const {
    auto identifier = std::string{obj.name->value};
    generator.reset();
    generator.resetFunction(identifier);
    return std::make_unique<TackyFun>(identifier, genForAstStmt(*obj.body));
}

TackyInstrPtrs TackyGenerator::genForAstStmt(const AstStmt& obj) const {
    const auto& type = obj.type;
    auto body = TackyInstrPtrs{};
    switch (type) {
        using enum AstNode::Type;
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
                                     TackyInstrPtrs& body) const {
    auto val = genForAstExpr(*obj.expr, body);
    body.emplace_back(std::make_unique<TackyReturn>(std::move(val)));
}

TackyValPtr TackyGenerator::genForAstExpr(const AstExpr& obj,
                                          TackyInstrPtrs& body) const {
    const auto& type = obj.type;
    switch (type) {
        using enum AstNode::Type;
        case LITERAL_INT: {
            auto& it = static_cast<const AstLitInt&>(obj);
            return std::make_unique<TackyLitInt>(genForAstLitInt(it));
        }
        case UNARY: {
            auto& unary = static_cast<const AstUnary&>(obj);
            return genForAstUnary(unary, body);
        }

        case BINARY: {
            auto& binary = static_cast<const AstBinary&>(obj);
            return genForAstBinary(binary, body);
        }

        default:
            fail("Could not generate TackyVal for AstNodeType::[{}]", type);
    }
}

TackyValPtr TackyGenerator::genForAstUnary(const AstUnary& obj,
                                           TackyInstrPtrs& body) const {
    auto op = genForAstUnaryOp(obj.op);
    auto src = genForAstExpr(*obj.expr, body);
    auto name = generator.generate();
    auto dest = std::make_unique<TackyVariable>(name);
    auto unary =
        std::make_unique<TackyUnary>(op, std::move(src), std::move(dest));
    body.push_back(std::move(unary));

    return std::make_unique<TackyVariable>(name);
}

TackyValPtr TackyGenerator::genForAstBinary(const AstBinary& obj,
                                            TackyInstrPtrs& body) const {
    auto src1 = genForAstExpr(*obj.left, body);
    auto src2 = genForAstExpr(*obj.right, body);
    auto op = genForAstBinaryOp(obj.op);
    auto name = generator.generate();
    auto dest = std::make_unique<TackyVariable>(name);
    auto binary = std::make_unique<TackyBinary>(
        op, std::move(src1), std::move(src2), std::move(dest));
    body.push_back(std::move(binary));

    return std::make_unique<TackyVariable>(name);
}

TackyLitInt TackyGenerator::genForAstLitInt(const AstLitInt& obj)  {
    return TackyLitInt{obj.value};
}

TackyUnary::Type
TackyGenerator::genForAstUnaryOp(const AstUnary::Type& type)  {
    switch (type) {
        using enum AstUnary::Type;
        case UNARY_COMPLEMENT: return TackyUnary::Type::UNARY_COMPLEMENT;
        case UNARY_NEGATE:     return TackyUnary::Type::UNARY_NEGATE;
        default:
            fail("Conversion from AstUnaryOpType::[{}] to TackyUnaryOpType "
                 "failed:",
                 type);
    }
}

TackyBinary::Type
TackyGenerator::genForAstBinaryOp(const AstBinary::Type& type)  {
    switch (type) {
        using enum AstBinary::Type;
        case BINARY_ADD:       return TackyBinary::Type::BINARY_ADD;
        case BINARY_SUBTRACT:  return TackyBinary::Type::BINARY_SUBTRACT;
        case BINARY_MULTIPLY:  return TackyBinary::Type::BINARY_MULTIPLY;
        case BINARY_DIVIDE:    return TackyBinary::Type::BINARY_DIVIDE;
        case BINARY_REMAINDER: return TackyBinary::Type::BINARY_REMAINDER;
        case BINARY_BIT_AND:   return TackyBinary::Type::BINARY_BIT_AND;
        case BINARY_BIT_OR:    return TackyBinary::Type::BINARY_BIT_OR;
        case BINARY_BIT_XOR:   return TackyBinary::Type::BINARY_BIT_XOR;
        case BINARY_BIT_LSH:   return TackyBinary::Type::BINARY_BIT_LSH;
        case BINARY_BIT_RSH:   return TackyBinary::Type::BINARY_BIT_RSH;
        default:
            fail("Conversion from AstBinaryOpType::[{}] to TackyBinaryOpType "
                 "failed:",
                 type);
    }
}

} // namespace wacc::tacky::gen