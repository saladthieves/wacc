#include "asm_pseudo_pass.hpp"
#include "asm_ast.hpp"

#include <cstdlib>
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

unsigned int AsmPseudoPass::getAbsoluteOffset() const {
    return std::abs(offset);
}

void AsmPseudoPass::runPass(AsmInstrPtrs& instructions) {
    for (auto& instr : instructions) {
        const auto& type = instr->type;
        switch (instr->type) {
            using enum AsmInstr::Type;
            case INSTR_MOV: {
                auto& mov = static_cast<AsmMov&>(*instr);
                replace(mov.src);
                replace(mov.dest);
                continue;
            }
            case INSTR_UNARY: {
                auto& unary = static_cast<AsmUnary&>(*instr);
                replace(unary.operand);
                continue;
            }
            case INSTR_BINARY: {
                auto& binary = static_cast<AsmBinary&>(*instr);
                replace(binary.src);
                replace(binary.dest);
                continue;
            }
            case INSTR_CMP: {
                auto& cmp = static_cast<AsmCmp&>(*instr);
                replace(cmp.left);
                replace(cmp.right);
                continue;
            }
            case INSTR_IDIV: {
                auto& idiv = static_cast<AsmIdiv&>(*instr);
                replace(idiv.operand);
                continue;
            }
            case INSTR_SET_COND: {
                auto& set = static_cast<AsmSetCond&>(*instr);
                replace(set.operand);
                continue;
            }
            default: continue;
        }
    }
}

void AsmPseudoPass::replace(AsmOperandPtr& ptr) {
    if (ptr->type != AsmNode::Type::OP_PSEUDO) return;

    auto& obj = static_cast<AsmPseudo&>(*ptr);
    signed value{0};
    const auto entry = stacks.find(obj.identifier);
    if (entry == stacks.end()) {
        value = getAdjustedOffset();
        stacks[obj.identifier] = value;
    } else {
        value = entry->second;
    }

    ptr = std::make_unique<AsmStack>(value);
}

signed int AsmPseudoPass::getAdjustedOffset() {
    offset -= 4;
    return offset;
}
} // namespace wacc::back::pass