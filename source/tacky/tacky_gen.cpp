#include "tacky_gen.hpp"
#include "ast.hpp"
#include "tacky_ast.hpp"
#include "utils.hpp"

#include <memory>
#include <stdexcept>

namespace wacc::tacky::gen {
// IdentifierGenerator
IdentifierGenerator::IdentifierGenerator() {
    reset();
}

void IdentifierGenerator::reset() {
    resetSession();
    variableCount = 0;
    labelCount = 0;
}

void IdentifierGenerator::resetSession(std::string_view session) {
    this->session = session.empty() ? generate(4, "S") : session;
}

void IdentifierGenerator::resetFunction(std::string_view function) {
    std::string upper{};
    for (const auto& c : function) upper += std::toupper(c);
    this->function = upper;
}

std::string IdentifierGenerator::generateVariable() {
    return std::format("{}.{}.TEMP.{}", session, function, variableCount++);
}

std::string IdentifierGenerator::generateLabel(const TackyBinary::Type& op,
                                               std::string_view extra) {
    // S1A2B.MAIN.BINARY_LOG_AND.0.L_FALSE
    return std::format("{}.{}.{}.{}.{}", session, function, op, labelCount++,
                       extra);
}

std::string IdentifierGenerator::generate(unsigned int length,
                                          std::string_view pre,
                                          std::string_view post) {
    if (length == 0) {
        throw std::runtime_error(
            "IdentifierGenerator: Cannot generate with a length of zero.");
    }

    constexpr auto letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string value{};
    for (unsigned int i = 0; i < length; ++i) {
        value += letters[utils::generateRandom(0, 35)];
    }

    return std::format("{}{}{}", pre, value, post);
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
    auto name = generator.generateVariable();
    auto dest = std::make_unique<TackyVariable>(name);
    auto unary =
        std::make_unique<TackyUnary>(op, std::move(src), std::move(dest));
    body.push_back(std::move(unary));

    return std::make_unique<TackyVariable>(name);
}

TackyValPtr TackyGenerator::genForAstBinary(const AstBinary& obj,
                                            TackyInstrPtrs& body) const {
    using enum AstBinary::Type;

    if (obj.op == BINARY_LOG_AND || obj.op == BINARY_LOG_OR) {
        return genForAstBinaryLogAndOr(obj, body);
    }

    auto src1 = genForAstExpr(*obj.left, body);
    auto src2 = genForAstExpr(*obj.right, body);
    auto op = genForAstBinaryOp(obj.op);
    auto name = generator.generateVariable();
    auto dest = std::make_unique<TackyVariable>(name);
    auto binary = std::make_unique<TackyBinary>(
        op, std::move(src1), std::move(src2), std::move(dest));
    body.push_back(std::move(binary));

    return std::make_unique<TackyVariable>(name);
}

TackyValPtr
TackyGenerator::genForAstBinaryLogAndOr(const AstBinary& obj,
                                        TackyInstrPtrs& body) const {
    const auto op = genForAstBinaryOp(obj.op);
    const auto id = generator.generate(6);

    if (obj.op == AstBinary::Type::BINARY_LOG_AND) {
        const auto lFalse = generator.generateLabel(op, labelFalse);
        const auto lEnd = generator.generateLabel(op, labelEnd);

        auto src1 = genForAstExpr(*obj.left, body);
        tackyJumpZero(std::move(src1), lFalse, body);

        auto src2 = genForAstExpr(*obj.right, body);
        tackyJumpZero(std::move(src2), lFalse, body);

        const auto result = generator.generateVariable();
        tackyCopy(std::make_unique<TackyLitInt>(1),
                  std::make_unique<TackyVariable>(result), body);
        tackyJump(lEnd, body);

        tackyLabel(lFalse, body);
        tackyCopy(std::make_unique<TackyLitInt>(0),
                  std::make_unique<TackyVariable>(result), body);
        tackyLabel(lEnd, body);
        return std::make_unique<TackyVariable>(result);
    } else {
        const auto lTrue = generator.generateLabel(op, labelTrue);
        const auto lEnd = generator.generateLabel(op, labelEnd);

        auto src1 = genForAstExpr(*obj.left, body);
        tackyJumpNotZero(std::move(src1), lTrue, body);

        auto src2 = genForAstExpr(*obj.right, body);
        tackyJumpNotZero(std::move(src2), lTrue, body);

        const auto result = generator.generateVariable();
        tackyCopy(std::make_unique<TackyLitInt>(0),
                  std::make_unique<TackyVariable>(result), body);
        tackyJump(lEnd, body);

        tackyLabel(lTrue, body);
        tackyCopy(std::make_unique<TackyLitInt>(1),
                  std::make_unique<TackyVariable>(result), body);
        tackyLabel(lEnd, body);
        return std::make_unique<TackyVariable>(result);
    }
}

void TackyGenerator::tackyJumpZero(TackyValPtr val, std::string_view target,
                                   TackyInstrPtrs& body) const {
    body.emplace_back(std::make_unique<TackyJumpZero>(std::move(val), target));
}

void TackyGenerator::tackyJumpNotZero(TackyValPtr val, std::string_view target,
                                      TackyInstrPtrs& body) const {
    body.emplace_back(
        std::make_unique<TackyJumpNotZero>(std::move(val), target));
}

void TackyGenerator::tackyJump(std::string_view label,
                               TackyInstrPtrs& body) const {
    body.emplace_back(std::make_unique<TackyJump>(label));
}

void TackyGenerator::tackyCopy(TackyValPtr src, TackyValPtr dest,
                               TackyInstrPtrs& body) const {
    body.emplace_back(
        std::make_unique<TackyCopy>(std::move(src), std::move(dest)));
}

void TackyGenerator::tackyLabel(std::string_view identifier,
                                TackyInstrPtrs& body) const {
    body.emplace_back(std::make_unique<TackyLabel>(identifier));
}

TackyLitInt TackyGenerator::genForAstLitInt(const AstLitInt& obj) {
    return TackyLitInt{obj.value};
}

TackyUnary::Type TackyGenerator::genForAstUnaryOp(const AstUnary::Type& type) {
    using Ast = AstUnary::Type;
    using Tacky = TackyUnary::Type;

    switch (type) {
        case Ast::UNARY_COMPLEMENT: return Tacky::UNARY_COMPLEMENT;
        case Ast::UNARY_NEGATE:     return Tacky::UNARY_NEGATE;
        case Ast::UNARY_NOT:        return Tacky::UNARY_NOT;
        default:
            fail("Conversion from AstUnaryOpType::[{}] to TackyUnaryOpType "
                 "failed:",
                 type);
    }
}

TackyBinary::Type
TackyGenerator::genForAstBinaryOp(const AstBinary::Type& type) {
    using Ast = AstBinary::Type;
    using Tacky = TackyBinary::Type;

    switch (type) {
        case Ast::BINARY_ADD:           return Tacky::BINARY_ADD;
        case Ast::BINARY_SUBTRACT:      return Tacky::BINARY_SUBTRACT;
        case Ast::BINARY_MULTIPLY:      return Tacky::BINARY_MULTIPLY;
        case Ast::BINARY_DIVIDE:        return Tacky::BINARY_DIVIDE;
        case Ast::BINARY_REMAINDER:     return Tacky::BINARY_REMAINDER;
        case Ast::BINARY_BIT_AND:       return Tacky::BINARY_BIT_AND;
        case Ast::BINARY_BIT_OR:        return Tacky::BINARY_BIT_OR;
        case Ast::BINARY_BIT_XOR:       return Tacky::BINARY_BIT_XOR;
        case Ast::BINARY_BIT_LSH:       return Tacky::BINARY_BIT_LSH;
        case Ast::BINARY_BIT_RSH:       return Tacky::BINARY_BIT_RSH;
        case Ast::BINARY_LOG_AND:       return Tacky::BINARY_LOG_AND;
        case Ast::BINARY_LOG_OR:        return Tacky::BINARY_LOG_OR;
        case Ast::BINARY_EQUAL:         return Tacky::BINARY_EQUAL;
        case Ast::BINARY_NOT_EQUAL:     return Tacky::BINARY_NOT_EQUAL;
        case Ast::BINARY_LESS:          return Tacky::BINARY_LESS;
        case Ast::BINARY_LESS_EQUAL:    return Tacky::BINARY_LESS_EQUAL;
        case Ast::BINARY_GREATER:       return Tacky::BINARY_GREATER;
        case Ast::BINARY_GREATER_EQUAL: return Tacky::BINARY_GREATER_EQUAL;
        default:
            fail("Conversion from AstBinaryOpType::[{}] to TackyBinaryOpType "
                 "failed:",
                 type);
    }
}

} // namespace wacc::tacky::gen