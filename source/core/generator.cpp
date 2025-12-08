#include "generator.hpp"
#include "assembly.hpp"
#include "ast.hpp"
#include "formatting.hpp"
#include "mem.hpp"

#include <format>
#include <stdexcept>
#include <string>

namespace wacc::core::gen {
namespace {
using wacc::utils::as;
}

Generator::Generator(AstNodePtr ptr) : ast{std::move(ptr)} {
}

AsmNodePtr Generator::generate() const {
    if (!ast) fail("Root AstNodePtr is null.");

    const auto program = as<AstProg>(ast);
    if (!program) fail("Root AstNodePtr is not an AstProg node type.");

    return generateProgram(*program);
}

AsmProgPtr Generator::generateProgram(const AstProg& obj) const {
    auto function = generateFunction(*obj.function);
    return std::make_unique<AsmProg>(std::move(function));
}

AsmFunPtr Generator::generateFunction(const AstFun& obj) const {
    auto name = std::string{obj.name->value};
    auto instructions = generateInstructions(*obj.body);
    return std::make_unique<AsmFun>(name, std::move(instructions));
}

AsmInstrPtrs Generator::generateInstructions(const AstStmt& obj) const {
    const auto type = obj.type();

    switch (type) {
        using enum AstNodeType;
        case RETURN: {
            const auto& returnObj = static_cast<const AstReturn&>(obj);
            return generateReturnInstructions(returnObj);
        }
        default: {
            fail(std::format("No instruction generator for AstNodeType::[{}]",
                             type));
        }
    }
}

AsmInstrPtrs Generator::generateReturnInstructions(const AstReturn& obj) const {
    auto instructions = AsmInstrPtrs{};
    instructions.emplace_back(generateMove(*obj.expression));
    instructions.emplace_back(generateReturn());

    return instructions;
}

AsmMovPtr Generator::generateMove(const AstExpr& obj) const {
    auto src = generateImmediate(obj);
    auto dest = generateRegister();
    return std::make_unique<AsmMov>(std::move(src), std::move(dest));
}

AsmRetPtr Generator::generateReturn() const {
    return std::make_unique<AsmRet>();
}

AsmImmPtr Generator::generateImmediate(const AstExpr& obj) const {
    const auto type = obj.type();
    switch (type) {
        using enum AstNodeType;
        case INTEGER: {
            const auto& intObj = static_cast<const AstInt&>(obj);
            return std::make_unique<AsmImm>(intObj.value);
        }
        default: {
            fail(std::format("Could not generate AsmImm for AsmImm::[{}]",
                             type));
        }
    }
}

AsmRegPtr Generator::generateRegister() const {
    return std::make_unique<AsmReg>();
}

[[noreturn]] void Generator::fail(std::string_view message) const {
    auto output = std::format("GeneratorError:\n  message: {}", message);
    throw std::runtime_error(output);
}
} // namespace wacc::core::gen