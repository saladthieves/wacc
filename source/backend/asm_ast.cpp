#include "asm_ast.hpp"

namespace wacc::back::ast {
// AsmImm
AsmImm::AsmImm(int value) : value{value} {
}

// AsmReg
AsmReg::AsmReg(AsmRegisterType reg) : reg{reg} {
}

// AsmPseudo
AsmPseudo::AsmPseudo(std::string identifier) : identifier{identifier} {
}

// AsmStack
AsmStack::AsmStack(signed value) : value{value} {
}

// AsmMov
AsmMov::AsmMov(AsmOperandPtr src, AsmOperandPtr dest) :
    src{std::move(src)}, dest{std::move(dest)} {
}

// AsmUnary
AsmUnary::AsmUnary(AsmUnaryOpType op, AsmOperandPtr operand) :
    op{op}, operand{std::move(operand)} {
}

// AsmBinary
AsmBinary::AsmBinary(AsmBinaryOpType op, AsmOperandPtr src,
                     AsmOperandPtr dest) :
    op{op}, src{std::move(src)}, dest{std::move(dest)} {
}

// AsmIdiv
AsmIdiv::AsmIdiv(AsmOperandPtr operand) : operand{std::move(operand)} {
}

// AsmAllocStack
AsmAllocStack::AsmAllocStack(unsigned value) : value{value} {
}

// AsmFun
AsmFun::AsmFun(std::string name, AsmInstrPtrs instructions) :
    name{name}, instructions{std::move(instructions)} {
}

// AsmProg
AsmProg::AsmProg(AsmFunPtr function) : function{std::move(function)} {
}
} // namespace wacc::back::ast