#pragma once

#include <memory>
#include <string>
#include <vector>

namespace wacc {
namespace core {
namespace assembly {

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

class AsmNode {};

// AsmOperand
class AsmOperand : public AsmNode {};

// AsmImm
class AsmImm : public AsmOperand {
public:
    AsmImm(int value);

    int value;
};

// AsmReg
class AsmReg : public AsmOperand {};

// AsmInstr
class AsmInstr : public AsmNode {};

// AsmMov
class AsmMov : public AsmInstr {
public:
    AsmMov(AsmOperandPtr src, AsmOperandPtr dest);

    AsmOperandPtr src;
    AsmOperandPtr dest;
};

// AsmRet
class AsmRet : public AsmInstr {};

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
} // namespace assembly
} // namespace core
} // namespace wacc