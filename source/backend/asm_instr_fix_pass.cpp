#include "asm_instr_fix_pass.hpp"
#include "asm_ast.hpp"

#include <vector>

namespace wacc::back::pass {
AsmInstrFixPass::AsmInstrFixPass(AsmNodePtr ptr, unsigned int stackOffset) :
    ast{std::move(ptr)}, stackOffset{stackOffset} {
}

AsmNodePtr AsmInstrFixPass::run() {
    if (!ast) fail("Pseudo-passed AsmNodePtr root is null.");

    auto& asmProg = static_cast<AsmProg&>(*ast);
    auto& asmFun = *asmProg.function;

    runPass(asmFun.instructions);

    return std::move(ast);
}

void AsmInstrFixPass::runPass(AsmInstrPtrs& instructions) { // TODO: Optimize
    using enum AsmNodeType;

    const auto getFixableInstr = [&]() -> StackPos {
        auto pos = instructions.begin();
        for (; pos != instructions.end(); ++pos) {
            if (isFixable(*pos)) return pos;
        }

        return pos;
    };

    StackPos pos{};
    while ((pos = getFixableInstr()) != instructions.end()) {
        const auto& type = pos->get()->type();
        if (type == INSTR_MOV) {
            fixAsmMov(pos, instructions);
            continue;
        }

        if (type == INSTR_IDIV) {
            fixAsmIdiv(pos, instructions);
            continue;
        }

        if (type == INSTR_BINARY) {
            fixAsmBinary(pos, instructions);
            continue;
        }
    }

    genAsmAllocStack(instructions);
}

void AsmInstrFixPass::genAsmAllocStack(AsmInstrPtrs& instructions) {
    auto alloc = std::make_unique<AsmAllocStack>(stackOffset);
    instructions.insert(instructions.begin(), std::move(alloc));
}

void AsmInstrFixPass::fixAsmMov(StackPos pos, AsmInstrPtrs& instructions) {
    auto& mov = static_cast<AsmMov&>(*pos->get());
    const auto reg = AsmRegisterType::R10;
    auto movToR10 = std::make_unique<AsmMov>(std::move(mov.src),
                                             std::make_unique<AsmReg>(reg));
    auto movFromR10 = std::make_unique<AsmMov>(std::make_unique<AsmReg>(reg),
                                               std::move(mov.dest));
    *pos++ = std::move(movToR10);

    instructions.insert(pos, std::move(movFromR10));
}

void AsmInstrFixPass::fixAsmIdiv(StackPos pos, AsmInstrPtrs& instructions) {
    auto& idiv = static_cast<AsmIdiv&>(*pos->get());
    const auto value = static_cast<AsmImm&>(*idiv.operand).value;
    const auto reg = AsmRegisterType::R10;
    *pos++ = std::make_unique<AsmMov>(std::make_unique<AsmImm>(value),
                                      std::make_unique<AsmReg>(reg));

    auto idivNew = std::make_unique<AsmIdiv>(std::make_unique<AsmReg>(reg));
    instructions.insert(pos, std::move(idivNew));
}

void AsmInstrFixPass::fixAsmBinary(StackPos pos, AsmInstrPtrs& instructions) {
    auto& binary = static_cast<AsmBinary&>(*pos->get());
    if (binary.op == AsmBinaryOpType::BINARY_MULT) {
        fixAsmBinaryMult(pos, binary, instructions);
    } else {
        const auto srcValue = static_cast<AsmStack&>(*binary.src).value;
        const auto destValue = static_cast<AsmStack&>(*binary.dest).value;
        const auto reg = AsmRegisterType::R10;
        const auto op = binary.op;
        *pos++ = std::make_unique<AsmMov>(std::make_unique<AsmStack>(srcValue),
                                          std::make_unique<AsmReg>(reg));
        auto binary2 =
            std::make_unique<AsmBinary>(op, std::make_unique<AsmReg>(reg),
                                        std::make_unique<AsmStack>(destValue));
        instructions.insert(pos, std::move(binary2));
    }
}

void AsmInstrFixPass::fixAsmBinaryMult(StackPos pos, AsmBinary& binary,
                                       AsmInstrPtrs& instructions) {
    const auto stackValue = static_cast<AsmStack&>(*binary.dest).value;
    auto src = std::move(binary.src);
    const auto op = binary.op;
    const auto reg = AsmRegisterType::R11;

    auto mov1 = std::make_unique<AsmMov>(std::make_unique<AsmStack>(stackValue),
                                         std::make_unique<AsmReg>(reg));
    auto mul = std::make_unique<AsmBinary>(op, std::move(src),
                                           std::make_unique<AsmReg>(reg));
    auto mov2 = std::make_unique<AsmMov>(
        std::make_unique<AsmReg>(reg), std::make_unique<AsmStack>(stackValue));

    *pos++ = std::move(mov1);
    pos = instructions.insert(pos, std::move(mul));
    instructions.insert(pos + 1, std::move(mov2));
}

bool AsmInstrFixPass::isFixable(const AsmInstrPtr& ptr) const {
    switch (ptr->type()) {
        using enum AsmNodeType;
        case INSTR_MOV: {
            auto& mov = static_cast<AsmMov&>(*ptr);
            return mov.src->type() == OP_STACK && mov.dest->type() == OP_STACK;
        }
        case INSTR_IDIV: {
            auto& idiv = static_cast<AsmIdiv&>(*ptr);
            return idiv.operand->type() == OP_IMM;
        }
        case INSTR_BINARY: {
            auto& binary = static_cast<AsmBinary&>(*ptr);
            if (binary.op == AsmBinaryOpType::BINARY_MULT) {
                return binary.dest->type() == OP_STACK;
            } else {
                return binary.src->type() == OP_STACK &&
                       binary.dest->type() == OP_STACK;
            }
        }

        default: return false;
    }
}
} // namespace wacc::back::pass