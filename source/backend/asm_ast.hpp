#pragma once

#include <memory>
#include <vector>

namespace wacc {
namespace back {
namespace ast {
enum class AsmNodeType : unsigned {
    OPERAND = 1,
    OP_IMM,
    OP_REG,
    INSTRUCTION,
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
};

// AsmOperand
class AsmOperand : public AsmNode {
public:
    virtual AsmNodeType type() const override { return OPERAND; };
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
    virtual AsmNodeType type() const override { return INSTRUCTION; };
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