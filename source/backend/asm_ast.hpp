#pragma once

#include <format>
#include <memory>
#include <vector>

namespace wacc {
namespace back {
namespace ast {
enum class AsmNodeType : unsigned {
    OP_IMM = 1,
    OP_REG,
    OP_PSEUDO,
    OP_STACK,
    INSTR_MOV,
    INSTR_UNARY,
    INSTR_ALLOC,
    INSTR_RET,
    FUNCTION,
    PROGRAM,
};

enum class AsmUnaryOpType : unsigned {
    UNARY_NEGATE = 1,
    UNARY_NOT,
};

enum class AsmRegisterType : unsigned {
    AX = 1,
    R10,
};

// Forward declarations
class AsmNode;
class AsmOperand;
class AsmImm;
class AsmReg;
class AsmPseudo;
class AsmStack;
class AsmInstr;
class AsmMov;
class AsmUnary;
class AsmAllocStack;
class AsmRet;
class AsmFun;
class AsmProg;

// Aliases
using AsmNodePtr = std::unique_ptr<AsmNode>;
using AsmOperandPtr = std::unique_ptr<AsmOperand>;
using AsmImmPtr = std::unique_ptr<AsmImm>;
using AsmRegPtr = std::unique_ptr<AsmReg>;
using AsmPseudoPtr = std::unique_ptr<AsmPseudo>;
using AsmStackPtr = std::unique_ptr<AsmStack>;
using AsmInstrPtr = std::unique_ptr<AsmInstr>;
using AsmMovPtr = std::unique_ptr<AsmMov>;
using AsmUnaryPtr = std::unique_ptr<AsmUnary>;
using AsmAllocStackPtr = std::unique_ptr<AsmAllocStack>;
using AsmRetPtr = std::unique_ptr<AsmRet>;
using AsmFunPtr = std::unique_ptr<AsmFun>;
using AsmProgPtr = std::unique_ptr<AsmProg>;

using AsmInstrPtrs = std::vector<AsmInstrPtr>;

namespace {
using enum AsmNodeType;
}

// AsmNode
class AsmNode {
public:
    virtual AsmNodeType type() const = 0;

    virtual ~AsmNode() = default;
};

// AsmOperand
class AsmOperand : public AsmNode {
public:
    virtual AsmNodeType type() const override = 0;
};

// AsmImm
class AsmImm : public AsmOperand {
public:
    AsmImm(int value);

    virtual AsmNodeType type() const override { return OP_IMM; };

    int value;
};

// AsmReg
class AsmReg : public AsmOperand {
public:
    AsmReg(AsmRegisterType reg);

    virtual AsmNodeType type() const override { return OP_REG; };

    AsmRegisterType reg;
};

// AsmPseudo
class AsmPseudo : public AsmOperand {
public:
    AsmPseudo(std::string identifier);

    virtual AsmNodeType type() const override { return OP_PSEUDO; }

    std::string identifier;
};

// AsmStack
class AsmStack : public AsmOperand {
public:
    AsmStack(signed value);

    virtual AsmNodeType type() const override { return OP_STACK; }

    signed value;
};

// AsmInstr
class AsmInstr : public AsmNode {
public:
    virtual AsmNodeType type() const override = 0;
};

// AsmMov
class AsmMov : public AsmInstr {
public:
    AsmMov(AsmOperandPtr src, AsmOperandPtr dest);

    virtual AsmNodeType type() const override { return INSTR_MOV; };

    AsmOperandPtr src;
    AsmOperandPtr dest;
};

// AsmUnary
class AsmUnary : public AsmInstr {
public:
    AsmUnary(AsmUnaryOpType op, AsmOperandPtr operand);

    virtual AsmNodeType type() const override { return INSTR_UNARY; };

    AsmUnaryOpType op;
    AsmOperandPtr operand;
};

// AsmAllocStack
class AsmAllocStack : public AsmInstr {
public:
    AsmAllocStack(unsigned value);

    virtual AsmNodeType type() const override { return INSTR_ALLOC; };

    unsigned value;
};

// AsmRet
class AsmRet : public AsmInstr {
public:
    virtual AsmNodeType type() const override { return INSTR_RET; };
};

// AsmFun
class AsmFun : public AsmNode {
public:
    AsmFun(std::string name, AsmInstrPtrs instructions);

    virtual AsmNodeType type() const override { return FUNCTION; };

    std::string name;
    AsmInstrPtrs instructions;
};

// AsmProg
class AsmProg : public AsmNode {
public:
    AsmProg(AsmFunPtr function);

    virtual AsmNodeType type() const override { return PROGRAM; };

    AsmFunPtr function;
};
} // namespace ast
} // namespace back
} // namespace wacc

namespace std {
namespace {
using wacc::back::ast::AsmNodeType;
}

template <>
class formatter<AsmNodeType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AsmNodeType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AsmNodeType;
            case OP_IMM:      value = "OP_IMM"; break;
            case OP_REG:      value = "OP_REG"; break;
            case OP_PSEUDO:   value = "OP_PSEUDO"; break;
            case OP_STACK:    value = "OP_STACK"; break;
            case INSTR_MOV:   value = "INSTR_MOV"; break;
            case INSTR_UNARY: value = "INSTR_UNARY"; break;
            case INSTR_ALLOC: value = "INSTR_ALLOC"; break;
            case INSTR_RET:   value = "INSTR_RET"; break;
            case FUNCTION:    value = "FUNCTION"; break;
            case PROGRAM:     value = "PROGRAM"; break;
            default:
                throw std::format_error(
                    "Unhandled back::ast::AsmNodeType enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::back::ast::AsmUnaryOpType;
}

template <>
class formatter<AsmUnaryOpType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AsmUnaryOpType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AsmUnaryOpType;
            case UNARY_NEGATE: value = "UNARY_NEGATE"; break;
            case UNARY_NOT:    value = "UNARY_NOT"; break;
            default:
                throw std::format_error(
                    "Unhandled back::ast::AsmUnaryOpType enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std