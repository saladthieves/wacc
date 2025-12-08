#include "assembly.hpp"

namespace wacc::core::assembly {
// AsmImm
AsmImm::AsmImm(int value) : value{value} {
}

// AsmReg

// AsmMov

AsmMov::AsmMov(AsmOperandPtr src, AsmOperandPtr dest) :
    src{std::move(src)}, dest{std::move(dest)} {
}

// AsmRet

// AsmFun
AsmFun::AsmFun(std::string name, AsmInstrPtrs instructions) :
    name{name}, instructions{std::move(instructions)} {
}

// AsmProg
AsmProg::AsmProg(AsmFunPtr function) : function{std::move(function)} {
}

} // namespace wacc::core::assembly