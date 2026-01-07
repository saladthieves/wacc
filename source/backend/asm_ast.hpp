#pragma once

#include <format>
#include <memory>
#include <vector>

namespace wacc {
namespace back {
namespace ast {

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
class AsmBinary;
class AsmIdiv;
class AsmCdq;
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
using AsmBinaryPtr = std::unique_ptr<AsmBinary>;
using AsmIdivPtr = std::unique_ptr<AsmIdiv>;
using AsmCdqPtr = std::unique_ptr<AsmCdq>;
using AsmAllocStackPtr = std::unique_ptr<AsmAllocStack>;
using AsmRetPtr = std::unique_ptr<AsmRet>;
using AsmFunPtr = std::unique_ptr<AsmFun>;
using AsmProgPtr = std::unique_ptr<AsmProg>;

using AsmInstrPtrs = std::vector<AsmInstrPtr>;

// AsmNode
class AsmNode {
public:
    enum class Type : std::uint8_t {
        OP_IMM = 1,
        OP_REG,
        OP_PSEUDO,
        OP_STACK,
        INSTR_MOV,
        INSTR_UNARY,
        INSTR_BINARY,
        INSTR_IDIV,
        INSTR_CDQ,
        INSTR_ALLOC,
        INSTR_RET,
        FUNCTION,
        PROGRAM,
    };

    AsmNode(Type type);

    virtual ~AsmNode() = default;

    Type type;
};

namespace {
using enum AsmNode::Type;
}

// AsmOperand
class AsmOperand : public AsmNode {
public:
    AsmOperand(Type type);
};

// AsmImm
class AsmImm : public AsmOperand {
public:
    AsmImm(int value);

    int value;
};

// AsmReg
class AsmReg : public AsmOperand {
public:
    enum class Type : std::uint8_t {
        AX = 1,
        CX,
        DX,
        R10,
        R11,
    };

    enum class Size: std::uint8_t {
        BYTE = 1,
        WORD,
        DOUBLE_WORD,
        QUAD_WORD,
    };

    AsmReg(Type reg, Size size = Size::DOUBLE_WORD);

    Type reg;
    Size size;
};

// AsmPseudo
class AsmPseudo : public AsmOperand {
public:
    AsmPseudo(std::string identifier);

    std::string identifier;
};

// AsmStack
class AsmStack : public AsmOperand {
public:
    AsmStack(signed value);

    signed value;
};

// AsmInstr
class AsmInstr : public AsmNode {
public:
    AsmInstr(Type type);
};

// AsmMov
class AsmMov : public AsmInstr {
public:
    AsmMov(AsmOperandPtr src, AsmOperandPtr dest);

    AsmOperandPtr src;
    AsmOperandPtr dest;
};

// AsmUnary
class AsmUnary : public AsmInstr {
public:
    enum class Type : std::uint8_t {
        UNARY_NEGATE = 1,
        UNARY_NOT,
    };

    AsmUnary(Type op, AsmOperandPtr operand);

    Type op;
    AsmOperandPtr operand;
};

// AsmBinary
class AsmBinary : public AsmInstr {
public:
    enum class Type : std::uint8_t {
        BINARY_ADD = 1,
        BINARY_SUB,
        BINARY_MULT,
        BINARY_BIT_AND,
        BINARY_BIT_OR,
        BINARY_BIT_XOR,
        BINARY_BIT_LSH,
        BINARY_BIT_RSH,
    };

    AsmBinary(Type op, AsmOperandPtr src, AsmOperandPtr dest);

    Type op;
    AsmOperandPtr src;
    AsmOperandPtr dest;
};

// AsmIdiv
class AsmIdiv : public AsmInstr {
public:
    AsmIdiv(AsmOperandPtr operand);

    AsmOperandPtr operand;
};

// AsmCdq
class AsmCdq : public AsmInstr {
public:
    AsmCdq();
};

// AsmAllocStack
class AsmAllocStack : public AsmInstr {
public:
    AsmAllocStack(unsigned value);

    unsigned value;
};

// AsmRet
class AsmRet : public AsmInstr {
public:
    AsmRet();
};

// AsmFun
class AsmFun : public AsmNode {
public:
    AsmFun(std::string name, AsmInstrPtrs instructions);

    std::string name;
    AsmInstrPtrs instructions;
};

// AsmProg
class AsmProg : public AsmNode {
public:
    AsmProg(AsmFunPtr function);

    AsmFunPtr function;
};
} // namespace ast
} // namespace back
} // namespace wacc

namespace std {
namespace {
using wacc::back::ast::AsmNode;
}

template <>
class formatter<AsmNode::Type> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AsmNode::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AsmNode::Type;
            case OP_IMM:       value = "OP_IMM"; break;
            case OP_REG:       value = "OP_REG"; break;
            case OP_PSEUDO:    value = "OP_PSEUDO"; break;
            case OP_STACK:     value = "OP_STACK"; break;
            case INSTR_MOV:    value = "INSTR_MOV"; break;
            case INSTR_UNARY:  value = "INSTR_UNARY"; break;
            case INSTR_BINARY: value = "INSTR_BINARY"; break;
            case INSTR_IDIV:   value = "INSTR_IDIV"; break;
            case INSTR_CDQ:    value = "INSTR_CDQ"; break;
            case INSTR_ALLOC:  value = "INSTR_ALLOC"; break;
            case INSTR_RET:    value = "INSTR_RET"; break;
            case FUNCTION:     value = "FUNCTION"; break;
            case PROGRAM:      value = "PROGRAM"; break;
            default:           throw std::format_error("Unhandled AsmNode::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::back::ast::AsmUnary;
}

template <>
class formatter<AsmUnary::Type> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AsmUnary::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AsmUnary::Type;
            case UNARY_NEGATE: value = "UNARY_NEGATE"; break;
            case UNARY_NOT:    value = "UNARY_NOT"; break;
            default:           throw std::format_error("Unhandled AsmUnary::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::back::ast::AsmBinary;
}

template <>
class formatter<AsmBinary::Type> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const AsmBinary::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum AsmBinary::Type;
            case BINARY_ADD:     value = "BINARY_ADD"; break;
            case BINARY_SUB:     value = "BINARY_SUB"; break;
            case BINARY_MULT:    value = "BINARY_MULT"; break;
            case BINARY_BIT_AND: value = "BINARY_BIT_AND"; break;
            case BINARY_BIT_OR:  value = "BINARY_BIT_OR"; break;
            case BINARY_BIT_XOR: value = "BINARY_BIT_XOR"; break;
            case BINARY_BIT_LSH: value = "BINARY_BIT_LSH"; break;
            case BINARY_BIT_RSH: value = "BINARY_BIT_RSH"; break;
            default:             throw std::format_error("Unhandled AsmBinary::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std