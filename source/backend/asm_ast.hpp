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
class AsmCmp;
class AsmIdiv;
class AsmCdq;
class AsmJmp;
class AsmJmpCond;
class AsmSetCond;
class AsmLabel;
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
using AsmCmpPtr = std::unique_ptr<AsmCmp>;
using AsmIdivPtr = std::unique_ptr<AsmIdiv>;
using AsmCdqPtr = std::unique_ptr<AsmCdq>;
using AsmJmpPtr = std::unique_ptr<AsmJmp>;
using AsmJmpCondPtr = std::unique_ptr<AsmJmpCond>;
using AsmSetCondPtr = std::unique_ptr<AsmSetCond>;
using AsmLabelPtr = std::unique_ptr<AsmLabel>;
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
        INSTR_CMP,
        INSTR_IDIV,
        INSTR_CDQ,
        INSTR_JMP,
        INSTR_JMP_COND,
        INSTR_SET_COND,
        INSTR_LABEL,
        INSTR_ALLOC,
        INSTR_RET,
        FUNCTION,
        PROGRAM,
    };

    explicit AsmNode(Type type);

    virtual ~AsmNode() = default;

    Type type;
};

namespace {
using enum AsmNode::Type;
} // namespace

enum class CondCode : std::uint8_t {
    EQUAL = 1,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
};

// AsmOperand
class AsmOperand : public AsmNode {
public:
    explicit AsmOperand(Type type);
};

// AsmImm
class AsmImm : public AsmOperand {
public:
    explicit AsmImm(int value);

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

    enum class Size : std::uint8_t {
        BYTE = 1,
        WORD = 2,
        DOUBLE_WORD = 4,
        QUAD_WORD = 8,
    };

    explicit AsmReg(Type reg, Size size = Size::DOUBLE_WORD);

    Type reg;
    Size size;
};

// AsmPseudo
class AsmPseudo : public AsmOperand {
public:
    explicit AsmPseudo(std::string_view identifier);

    std::string identifier;
};

// AsmStack
class AsmStack : public AsmOperand {
public:
    explicit AsmStack(signed value);

    signed value;
};

// AsmInstr
class AsmInstr : public AsmNode {
public:
    explicit AsmInstr(Type type);
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
        UNARY_COMPLEMENT,
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
        BINARY_LOG_AND,
        BINARY_LOG_OR,
    };

    AsmBinary(Type op, AsmOperandPtr src, AsmOperandPtr dest);

    Type op;
    AsmOperandPtr src;
    AsmOperandPtr dest;
};

// AsmCmp
class AsmCmp : public AsmInstr {
public:
    AsmCmp(AsmOperandPtr left, AsmOperandPtr right);

    AsmOperandPtr left;
    AsmOperandPtr right;
};

// AsmIdiv
class AsmIdiv : public AsmInstr {
public:
    explicit AsmIdiv(AsmOperandPtr operand);

    AsmOperandPtr operand;
};

// AsmCdq
class AsmCdq : public AsmInstr {
public:
    AsmCdq();
};

// AsmJmp
class AsmJmp : public AsmInstr {
public:
    explicit AsmJmp(std::string_view label);

    std::string label;
};

// AsmJmpCond
class AsmJmpCond : public AsmInstr {
public:
    using Code = CondCode;

    AsmJmpCond(Code condition, std::string_view label);

    Code condition;
    std::string label;
};

// AsmSetCond
class AsmSetCond : public AsmInstr {
public:
    using Code = CondCode;

    AsmSetCond(Code condition, AsmOperandPtr operand);

    Code condition;
    AsmOperandPtr operand;
};

// AsmLabel
class AsmLabel : public AsmInstr {
public:
    explicit AsmLabel(std::string_view value);

    std::string value;
};

// AsmAllocStack
class AsmAllocStack : public AsmInstr {
public:
    explicit AsmAllocStack(unsigned value);

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
    explicit AsmProg(AsmFunPtr function);

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
    static constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    static auto format(const AsmNode::Type& type, format_context& context) {
        std::string value{};

        switch (type) {
            using enum AsmNode::Type;
            case OP_IMM:         value = "OP_IMM"; break;
            case OP_REG:         value = "OP_REG"; break;
            case OP_PSEUDO:      value = "OP_PSEUDO"; break;
            case OP_STACK:       value = "OP_STACK"; break;
            case INSTR_MOV:      value = "INSTR_MOV"; break;
            case INSTR_UNARY:    value = "INSTR_UNARY"; break;
            case INSTR_BINARY:   value = "INSTR_BINARY"; break;
            case INSTR_CMP:      value = "INSTR_CMP"; break;
            case INSTR_IDIV:     value = "INSTR_IDIV"; break;
            case INSTR_CDQ:      value = "INSTR_CDQ"; break;
            case INSTR_JMP:      value = "INSTR_JMP"; break;
            case INSTR_JMP_COND: value = "INSTR_JMP_COND"; break;
            case INSTR_SET_COND: value = "INSTR_SET_COND"; break;
            case INSTR_LABEL:    value = "INSTR_LABEL"; break;
            case INSTR_ALLOC:    value = "INSTR_ALLOC"; break;
            case INSTR_RET:      value = "INSTR_RET"; break;
            case FUNCTION:       value = "FUNCTION"; break;
            case PROGRAM:        value = "PROGRAM"; break;
            default:             throw std::format_error("Unhandled AsmNode::Type enum");
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
    static constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    static auto format(const AsmUnary::Type& type, format_context& context) {
        std::string value{};

        switch (type) {
            using enum AsmUnary::Type;
            case UNARY_NEGATE:     value = "UNARY_NEGATE"; break;
            case UNARY_COMPLEMENT: value = "UNARY_COMPLEMENT"; break;
            default:               throw std::format_error("Unhandled AsmUnary::Type enum");
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
    static constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    static auto format(const AsmBinary::Type& type, format_context& context) {
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
            case BINARY_LOG_AND: value = "BINARY_LOG_AND"; break;
            case BINARY_LOG_OR:  value = "BINARY_LOG_OR"; break;
            default:             throw std::format_error("Unhandled AsmBinary::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std