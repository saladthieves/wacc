#include "asm_emit.hpp"
#include "asm_ast.hpp"
#include "utils.hpp"

#include <string>

namespace wacc::back::emit {
AsmEmitter::AsmEmitter(AsmNodePtr ptr, utils::Platform platform) :
    ast{std::move(ptr)}, lines{std::make_unique<Lines>()}, platform{platform} {
}

LinesPtr AsmEmitter::emit() {
    if (platform.isUnknown()) {
        fail("Failed to emit: unknown platform.");
    }

    if (!ast) {
        fail("Failed to emit: AsmNode tree is null");
    }

    const auto& prog = static_cast<const AsmProg&>(*ast);
    emitAsmProg(prog);

    return std::move(lines);
}

void AsmEmitter::emitAsmProg(const AsmProg& obj) {
    auto& fun = static_cast<const AsmFun&>(*obj.function);
    emitAsmFun(fun);

    if (platform.isLinux()) {
        pushLine("{}{}", INDENT, R"(.section .note.GNU-stack,"",@progbits)");
    }
}

void AsmEmitter::emitAsmFun(const AsmFun& obj) {
    std::string name{};
    if (platform.isMacOS()) {
        name = std::format("_{}", obj.name);
    }

    if (platform.isLinux()) {
        name = obj.name;
    }

    pushLine("{}.globl {}", INDENT, name);
    pushLine("{}:", name);

    for (const auto& ptr : obj.instructions) {
        emitAsmInstr(*ptr);
    }
}

void AsmEmitter::emitAsmInstr(const AsmInstr& obj) {
    const auto& type = obj.type();
    switch (type) {
        using enum AsmNodeType;
        case INSTR_MOV: {
            auto& mov = static_cast<const AsmMov&>(obj);
            emitAsmMov(mov);
            break;
        }
        case INSTR_RET: {
            auto& ret = static_cast<const AsmRet&>(obj);
            emitAsmRet(ret);
            break;
        }

        default: fail("Failed to emit AsmInst::[type = {}]", type);
    }
}

void AsmEmitter::emitAsmMov(const AsmMov& obj) {
    pushLine("{0}movl{0}", INDENT);

    const auto src = formatAsmOperand(*obj.src);
    const auto dest = formatAsmOperand(*obj.dest);

    appendLine("{}, {}", src, dest);
}

void AsmEmitter::emitAsmRet(const AsmRet& obj) {
    pushLine("{}ret", INDENT);
}

std::string AsmEmitter::formatAsmOperand(const AsmOperand& obj) const {
    const auto& type = obj.type();
    switch (type) {
        using enum AsmNodeType;
        case OP_IMM: {
            auto& imm = static_cast<const AsmImm&>(obj);
            return std::format("${}", imm.value);
        }
        case OP_REG: {
            return std::format("%eax");
        }

        default: {
            fail("Failed to format AsmOperand::[type = {}]", type);
        }
    }
}
} // namespace wacc::back::emit