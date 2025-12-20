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
    INSTR_MOV,
    INSTR_RET,
    FUNCTION,
    PROGRAM,
};

// Forward declarations
class AsmNode;
class AsmOperand;
class AsmImm;
class AsmReg;
class AsmInstr;
class AsmMov;
class AsmRet;
class AsmFun;
class AsmProg;

// Aliases
using AsmNodePtr = std::unique_ptr<AsmNode>;
using AsmOperandPtr = std::unique_ptr<AsmOperand>;
using AsmImmPtr = std::unique_ptr<AsmImm>;
using AsmRegPtr = std::unique_ptr<AsmReg>;
using AsmInstrPtr = std::unique_ptr<AsmInstr>;
using AsmMovPtr = std::unique_ptr<AsmMov>;
using AsmRetPtr = std::unique_ptr<AsmRet>;
using AsmFunPtr = std::unique_ptr<AsmFun>;
using AsmProgPtr = std::unique_ptr<AsmProg>;

using AsmInstrPtrs = std::vector<AsmInstrPtr>;

namespace {
using enum AsmNodeType;
}

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
    virtual AsmNodeType type() const override { return OP_REG; };
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
            case OP_IMM:    value = "OP_IMM"; break;
            case OP_REG:    value = "OP_REG"; break;
            case INSTR_MOV: value = "INSTR_MOV"; break;
            case INSTR_RET: value = "INSTR_RET"; break;
            case FUNCTION:  value = "FUNCTION"; break;
            case PROGRAM:   value = "PROGRAM"; break;
            default:
                throw std::format_error(
                    "Unhandled back::ast::AsmNodeType enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std