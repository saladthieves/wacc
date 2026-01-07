#include "asm_emit.hpp"
#include "asm_ast.hpp"
#include "utils.hpp"

#include <string>
#include <utility>

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
    pushLine("{0}pushq{0}%rbp", INDENT);
    pushLine("{0}movq{0}%rsp, %rbp", INDENT);

    for (const auto& ptr : obj.instructions) {
        emitAsmInstr(*ptr);
    }
}

void AsmEmitter::emitAsmInstr(const AsmInstr& obj) {
    const auto& type = obj.type;
    switch (type) {
        using enum AsmNode::Type;
        case INSTR_ALLOC: {
            auto& alloc = static_cast<const AsmAllocStack&>(obj);
            emitAsmAllocStack(alloc);
            break;
        }
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

        case INSTR_UNARY: {
            auto& unary = static_cast<const AsmUnary&>(obj);
            emitAsmUnary(unary);
            break;
        }

        case INSTR_BINARY: {
            auto& binary = static_cast<const AsmBinary&>(obj);
            emitAsmBinary(binary);
            break;
        }

        case INSTR_IDIV: {
            auto& idiv = static_cast<const AsmIdiv&>(obj);
            emitAsmIdiv(idiv);
            break;
        }

        case INSTR_CDQ: {
            emitAsmCdq();
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
    pushLine("{0}movq{0}%rbp, %rsp", INDENT);
    pushLine("{0}popq{0}%rbp", INDENT);
    pushLine("{}ret", INDENT);
}

void AsmEmitter::emitAsmUnary(const AsmUnary& obj) {
    const auto op = formatAsmUnaryOp(obj.op);
    const auto operand = formatAsmOperand(*obj.operand);
    pushLine("{}{}{}{}", INDENT, op, INDENT, operand);
}

void AsmEmitter::emitAsmBinary(const AsmBinary& obj) {
    const auto op = formatAsmBinaryOp(obj.op);
    const auto src = formatAsmOperand(*obj.src);
    const auto dest = formatAsmOperand(*obj.dest);
    pushLine("{}{}{}{}, {}", INDENT, op, INDENT, src, dest);
}

void AsmEmitter::emitAsmIdiv(const AsmIdiv& obj) {
    const auto operand = formatAsmOperand(*obj.operand);
    pushLine("{}idivl{}{}", INDENT, INDENT, operand);
}

void AsmEmitter::emitAsmCdq() {
    pushLine("{}cdq", INDENT);
}

void AsmEmitter::emitAsmAllocStack(const AsmAllocStack& obj) {
    pushLine("{}subq{}${}, %rsp", INDENT, INDENT, obj.value);
}

std::string AsmEmitter::formatAsmOperand(const AsmOperand& obj) const {
    const auto& type = obj.type;
    switch (type) {
        using enum AsmNode::Type;
        case OP_REG: {
            auto& reg = static_cast<const AsmReg&>(obj);
            return formatAsmReg(reg);
        }
        case OP_STACK: {
            auto& stack = static_cast<const AsmStack&>(obj);
            return std::format("{}(%rbp)", stack.value);
        }
        case OP_IMM: {
            auto& imm = static_cast<const AsmImm&>(obj);
            return std::format("${}", imm.value);
        }

        default: {
            fail("Failed to format AsmOperand::[type = {}]", type);
        }
    }
}

std::string AsmEmitter::formatAsmReg(const AsmReg& obj) const {
    using enum AsmReg::Type;
    using enum AsmReg::Size;

    const auto& type = obj.reg;
    const auto& size = obj.size;

    if (type == AX) {
        switch (size) {
            case BYTE:        return "%al";
            case WORD:        return "%ax";
            case DOUBLE_WORD: return "%eax";
            case QUAD_WORD:   return "%rax";
        }
    }

    if (type == CX) {
        switch (size) {
            case BYTE:        return "%cl";
            case WORD:        return "%cx";
            case DOUBLE_WORD: return "%ecx";
            case QUAD_WORD:   return "%rcx";
        }
    }

    if (type == DX) {
        switch (size) {
            case BYTE:        return "%dl";
            case WORD:        return "%dx";
            case DOUBLE_WORD: return "%edx";
            case QUAD_WORD:   return "%rdx";
        }
    }

    if (type == R10) {
        switch (size) {
            case BYTE:        return "%r10b";
            case WORD:        return "%r10w";
            case DOUBLE_WORD: return "%r10d";
            case QUAD_WORD:   return "%r10";
        }
    }

    if (type == R11) {
        switch (size) {
            case BYTE:        return "%r11b";
            case WORD:        return "%r11w";
            case DOUBLE_WORD: return "%r11d";
            case QUAD_WORD:   return "%r11";
        }
    }

    fail("Failed to format AsmReg::Type::[{}]", std::to_underlying(type));
}

std::string AsmEmitter::formatAsmUnaryOp(const AsmUnary::Type& type) const {
    switch (type) {
        using enum AsmUnary::Type;
        case UNARY_NEGATE: return "negl";
        case UNARY_NOT:    return "notl";
        default:           fail("Failed to format AsmUnary::Type::[type = {}]", type);
    }
}

std::string AsmEmitter::formatAsmBinaryOp(const AsmBinary::Type& type) const {
    switch (type) {
        using enum AsmBinary::Type;
        case BINARY_ADD:     return "addl";
        case BINARY_SUB:     return "subl";
        case BINARY_MULT:    return "imull";
        case BINARY_BIT_LSH: return "sall";
        case BINARY_BIT_RSH: return "sarl";
        default:             fail("Failed to format AsmBinary::Type::[type = {}]", type);
    }
}
} // namespace wacc::back::emit