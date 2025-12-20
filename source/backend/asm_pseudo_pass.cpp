#include "asm_pseudo_pass.hpp"

#include <memory>

namespace wacc::back::pass {
AsmPseudoPass::AsmPseudoPass(AsmNodePtr ptr) : ast{std::move(ptr)} {
}

AsmNodePtr AsmPseudoPass::run() {
    if (!ast) fail("Generated AsmNodePtr root is null.");

    auto& asmProg = static_cast<AsmProg&>(*ast);
    auto& asmFun = *asmProg.function;

    runPass(asmFun.instructions);

    return std::move(ast);
}

void AsmPseudoPass::runPass(AsmInstrPtrs& instructions) {
    for (auto& instr : instructions) {
        const auto& type = instr->type();
        switch (type) {
            using enum AsmNodeType;
            case INSTR_MOV: {
                auto& mov = static_cast<AsmMov&>(*instr);
                runAsmMovPass(mov);
                continue;
            }
            case INSTR_UNARY: {
                auto& unary = static_cast<AsmUnary&>(*instr);
                runAsmUnaryPass(unary);
                continue;
            }

            default: {
                fail("Unhandled TackyInstr::[{}] pass ", type);
            }
        }
    }
}

void AsmPseudoPass::runAsmMovPass(AsmMov& instr) {
    if (instr.src->type() == AsmNodeType::OP_PSEUDO) {
        instr.src = replace(instr.src);
    }

    if (instr.dest->type() == AsmNodeType::OP_PSEUDO) {
        instr.dest = replace(instr.dest);
    }
}

void AsmPseudoPass::runAsmUnaryPass(AsmUnary& instr) {
    if (instr.operand->type() == AsmNodeType::OP_PSEUDO) {
        instr.operand = replace(instr.operand);
    }
}

AsmStackPtr AsmPseudoPass::replace(AsmOperandPtr& ptr) {
    auto& obj = static_cast<AsmPseudo&>(*ptr);
    signed value{0};
    auto entry = stacks.find(obj.identifier);
    if (entry == stacks.end()) {
        value = getOffset();
        stacks[obj.identifier] = offset;
    } else {
        value = entry->second;
    }

    return std::make_unique<AsmStack>(value);
}

signed int AsmPseudoPass::getOffset() {
    offset -= 4;
    return offset;
}
} // namespace wacc::back::pass