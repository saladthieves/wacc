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
    asmFun.instructions.swap(fixed);

    return std::move(ast);
}

void AsmInstrFixPass::runPass(AsmInstrPtrs& instructions) {
    fixed.reserve(instructions.size());

    genAsmAllocStack();

    for (auto& instruction : instructions) {
        const auto& type = instruction->type;
        switch (type) {
            case INSTR_MOV: {
                fixAsmMov(std::move(instruction));
                continue;
            }
            case INSTR_IDIV: {
                fixAsmIdiv(std::move(instruction));
                continue;
            }
            case INSTR_BINARY: {
                fixAsmBinary(std::move(instruction));
                continue;
            }
            case INSTR_CMP: {
                fixAsmCmp(std::move(instruction));
                continue;
            }
            default: {
                fixed.push_back(std::move(instruction));
            }
        }
    }
}

void AsmInstrFixPass::genAsmAllocStack() {
    // sub $value, %rsp
    fixed.emplace_back(std::make_unique<AsmAllocStack>(stackOffset));
}

void AsmInstrFixPass::fixAsmMov(AsmInstrPtr ptr) {
    auto& mov = static_cast<AsmMov&>(*ptr);

    // mov [-x(%rbp)], [-y(%rbp)]
    if (mov.src->type == OP_STACK && mov.dest->type == OP_STACK) {
        const auto reg = AsmReg::Type::R10;
        // mov src, %r10d
        auto movToR10 = std::make_unique<AsmMov>(std::move(mov.src),
                                                 std::make_unique<AsmReg>(reg));
        // mov %r10d, dest
        auto movFromR10 = std::make_unique<AsmMov>(
            std::make_unique<AsmReg>(reg), std::move(mov.dest));

        fixed.push_back(std::move(movToR10));
        fixed.push_back(std::move(movFromR10));
    } else {
        fixed.push_back(std::move(ptr));
    }
}

void AsmInstrFixPass::fixAsmIdiv(AsmInstrPtr ptr) {
    const auto& idiv = static_cast<AsmIdiv&>(*ptr);

    // mov [$x]
    if (idiv.operand->type == OP_IMM) {
        const auto value = static_cast<AsmImm&>(*idiv.operand).value;
        const auto reg = AsmReg::Type::R10;
        // mov $x, %r10d
        auto mov = std::make_unique<AsmMov>(std::make_unique<AsmImm>(value),
                                            std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(mov));

        // idiv %r10d
        auto idivNew = std::make_unique<AsmIdiv>(std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(idivNew));
    } else {
        fixed.push_back(std::move(ptr));
    }
}

void AsmInstrFixPass::fixAsmBinary(AsmInstrPtr ptr) {
    // idiv [-x(%rbp)]
    const auto& bin = static_cast<AsmBinary&>(*ptr);
    using enum AsmBinary::Type;
    if (bin.op == BINARY_MULT) {
        return fixAsmBinaryMult(std::move(ptr));
    }

    if (bin.op == BINARY_BIT_LSH || bin.op == BINARY_BIT_RSH) {
        return fixAsmBinaryShift(std::move(ptr));
    }

    if (bin.op == BINARY_BIT_AND || bin.op == BINARY_BIT_XOR ||
        bin.op == BINARY_BIT_OR) {
        return fixAsmBinaryAndXorOr(std::move(ptr));
    }

    // [add|sub] [-x(%rbp)], [-y(%rbp)]
    if (bin.src->type == OP_STACK && bin.dest->type == OP_STACK) {
        // copy all needed by value first
        const auto src = static_cast<AsmStack&>(*bin.src).value;
        const auto dest = static_cast<AsmStack&>(*bin.dest).value;
        const auto reg = AsmReg::Type::R10;
        const auto op = bin.op;

        // mov -x(%rbp), %r10d
        auto fixedMov = std::make_unique<AsmMov>(
            std::make_unique<AsmStack>(src), std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(fixedMov));

        // [add|sub] %r10d, -y(%rbp) |or| -y(%rbp) = -y(%rbp) [add|sub] %r10d
        auto fixedBin =
            std::make_unique<AsmBinary>(op, std::make_unique<AsmReg>(reg),
                                        std::make_unique<AsmStack>(dest));
        fixed.push_back(std::move(fixedBin));
    } else {
        fixed.push_back(std::move(ptr));
    }
}

void AsmInstrFixPass::fixAsmBinaryMult(AsmInstrPtr ptr) {
    // imul src, [-x(%dest)]
    auto& bin = static_cast<AsmBinary&>(*ptr);

    if (bin.dest->type == OP_STACK) {
        const auto stackValue = static_cast<AsmStack&>(*bin.dest).value;
        const auto op = bin.op;
        const auto reg = AsmReg::Type::R11;
        // mov -x(%dest), %r11d
        auto mov1 =
            std::make_unique<AsmMov>(std::make_unique<AsmStack>(stackValue),
                                     std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(mov1));

        // imul src, %r11d |or| %r11d = %r11d x src
        auto mul = std::make_unique<AsmBinary>(op, std::move(bin.src),
                                               std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(mul));

        // mov %r11d, -x(%dest)
        auto mov2 =
            std::make_unique<AsmMov>(std::make_unique<AsmReg>(reg),
                                     std::make_unique<AsmStack>(stackValue));
        fixed.push_back(std::move(mov2));
    } else {
        fixed.push_back(std::move(ptr));
    }
}

void AsmInstrFixPass::fixAsmBinaryShift(AsmInstrPtr ptr) {
    // [sar|sal] ![$imm], dest
    auto& bin = static_cast<AsmBinary&>(*ptr);

    if (bin.src->type != OP_IMM) {
        const auto reg = AsmReg::Type::CX;

        // mov src, %ecx
        auto mov = std::make_unique<AsmMov>(std::move(bin.src),
                                            std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(mov));

        // [sar|sal] %cl, dest
        auto shift = std::make_unique<AsmBinary>(
            bin.op, std::make_unique<AsmReg>(reg, AsmReg::Size::BYTE),
            std::move(bin.dest));
        fixed.push_back(std::move(shift));
    } else {
        fixed.push_back(std::move(ptr));
    }
}

void AsmInstrFixPass::fixAsmBinaryAndXorOr(AsmInstrPtr ptr) {
    // [and|xor|or] [-x(%src)] [-y(%dest)]
    const auto& bin = static_cast<AsmBinary&>(*ptr);

    if (bin.src->type == OP_STACK && bin.dest->type == OP_STACK) {
        // copy all needed by value first
        const auto src = static_cast<AsmStack&>(*bin.src).value;
        const auto dest = static_cast<AsmStack&>(*bin.dest).value;
        const auto reg = AsmReg::Type::R10;
        const auto op = bin.op;

        // mov -x(%rbp), %r10d
        auto fixedMov = std::make_unique<AsmMov>(
            std::make_unique<AsmStack>(src), std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(fixedMov));

        // [binop] %r10d, -y(%rbp) |or| -y(%rbp) = -y(%rbp) [binop] %r10d
        auto fixedBin =
            std::make_unique<AsmBinary>(op, std::make_unique<AsmReg>(reg),
                                        std::make_unique<AsmStack>(dest));
        fixed.push_back(std::move(fixedBin));
    } else {
        fixed.push_back(std::move(ptr));
    }
}

void AsmInstrFixPass::fixAsmCmp(AsmInstrPtr ptr) {
    auto& cmp = static_cast<AsmCmp&>(*ptr);

    // cmp [-x(%left)] [-y(%right)]
    if (cmp.left->type == OP_STACK && cmp.right->type == OP_STACK) {
        // copy all needed by value first
        const auto left = static_cast<AsmStack&>(*cmp.left).value;
        const auto right = static_cast<AsmStack&>(*cmp.right).value;
        const auto reg = AsmReg::Type::R10;

        // mov -x(%left), %r10d
        auto fixedMov = std::make_unique<AsmMov>(
            std::make_unique<AsmStack>(left), std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(fixedMov));

        // cmp %r10, -y(%right)
        auto fixedCmpl = std::make_unique<AsmCmp>(
            std::make_unique<AsmReg>(reg), std::make_unique<AsmStack>(right));
        fixed.push_back(std::move(fixedCmpl));
        return;
    }

    // cmp [-x(%left)], $right
    if (cmp.right->type == OP_IMM) {
        const auto value = static_cast<AsmImm&>(*cmp.right).value;
        const auto reg = AsmReg::Type::R11;

        // mov $right, %r11d
        auto fixedMov = std::make_unique<AsmMov>(
            std::make_unique<AsmImm>(value), std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(fixedMov));

        // cmp -x(%left), %r11d
        auto fixedCmpl = std::make_unique<AsmCmp>(
            std::move(cmp.left), std::make_unique<AsmReg>(reg));
        fixed.push_back(std::move(fixedCmpl));
        return;
    }

    fixed.push_back(std::move(ptr));
}
} // namespace wacc::back::pass