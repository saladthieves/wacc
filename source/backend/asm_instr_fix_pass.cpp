#include "asm_instr_fix_pass.hpp"
#include "asm_ast.hpp"

#include <vector>

namespace wacc::back::pass {
AsmInstrFixPass::AsmInstrFixPass(AsmNodePtr ptr, signed int stackOffset) :
    ast{std::move(ptr)}, stackOffset{stackOffset} {
}

AsmNodePtr AsmInstrFixPass::run() {
    if (!ast) fail("Pseudo-passed AsmNodePtr root is null.");

    auto& asmProg = static_cast<AsmProg&>(*ast);
    auto& asmFun = *asmProg.function;

    runPass(asmFun.instructions);

    return std::move(ast);
}

void AsmInstrFixPass::runPass(AsmInstrPtrs& instructions) {
    using enum AsmNodeType;
    const auto hasStacks = [](AsmOperandPtr& src, AsmOperandPtr& dest) -> bool {
        return src->type() == OP_STACK && dest->type() == OP_STACK;
    };

    std::vector<StackPos> positions{};

    for (auto pos = instructions.begin(); pos != instructions.end(); ++pos) {
        auto& instr = *pos;
        const auto& type = instr->type();
        if (type == INSTR_MOV) {
            auto& movInstr = static_cast<AsmMov&>(*instr);
            if (hasStacks(movInstr.src, movInstr.dest)) {
                positions.push_back(pos);
            }
            continue;
        }
    }

    for (auto& pos : positions) {
        fixAsmMov(pos, instructions);
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
    *pos = std::move(movToR10);
    
    instructions.insert(pos + 1, std::move(movFromR10));
}
} // namespace wacc::back::pass