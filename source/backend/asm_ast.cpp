#include "asm_ast.hpp"

namespace wacc::back::ast {
// AsmNode
AsmNode::AsmNode(Type type) : type{type} {
}

// AsmOperand
AsmOperand::AsmOperand(Type type) : AsmNode(type) {
}

// AsmImm
AsmImm::AsmImm(int value) : AsmOperand(OP_IMM), value{value} {
}

// AsmReg
AsmReg::AsmReg(Type reg, Size size) : AsmOperand(OP_REG), reg{reg}, size{size} {
}

// AsmPseudo
AsmPseudo::AsmPseudo(std::string_view identifier) :
    AsmOperand(OP_PSEUDO), identifier{identifier} {
}

// AsmStack
AsmStack::AsmStack(signed value) : AsmOperand(OP_STACK), value{value} {
}

// AsmInstr
AsmInstr::AsmInstr(Type type) : AsmNode(type) {
}

// AsmMov
AsmMov::AsmMov(AsmOperandPtr src, AsmOperandPtr dest) :
    AsmInstr(INSTR_MOV), src{std::move(src)}, dest{std::move(dest)} {
}

// AsmUnary
AsmUnary::AsmUnary(Type op, AsmOperandPtr operand) :
    AsmInstr(INSTR_UNARY), op{op}, operand{std::move(operand)} {
}

// AsmBinary
AsmBinary::AsmBinary(Type op, AsmOperandPtr src, AsmOperandPtr dest) :
    AsmInstr(INSTR_BINARY), op{op}, src{std::move(src)}, dest{std::move(dest)} {
}

// AsmCmp
AsmCmp::AsmCmp(AsmOperandPtr left, AsmOperandPtr right) :
    AsmInstr(INSTR_CMP), left{std::move(left)}, right{std::move(right)} {
}

// AsmIdiv
AsmIdiv::AsmIdiv(AsmOperandPtr operand) :
    AsmInstr(INSTR_IDIV), operand{std::move(operand)} {
}

// AsmCdq
AsmCdq::AsmCdq() : AsmInstr(INSTR_CDQ) {
}

// AsmJmp
AsmJmp::AsmJmp(std::string_view label) : AsmInstr(INSTR_JMP), label{label} {
}

// AsmJmpCond
AsmJmpCond::AsmJmpCond(Code condition, std::string_view label) :
    AsmInstr(INSTR_JMP_COND), condition{condition}, label{label} {
}

// AsmSetCond
AsmSetCond::AsmSetCond(Code condition, AsmOperandPtr operand) :
    AsmInstr(INSTR_SET_COND), condition{condition},
    operand{std::move(operand)} {
}

// AsmLabel
AsmLabel::AsmLabel(std::string_view value) :
    AsmInstr(INSTR_LABEL), value{value} {
}

// AsmAllocStack
AsmAllocStack::AsmAllocStack(unsigned value) :
    AsmInstr(INSTR_ALLOC), value{value} {
}

// AsmRet
AsmRet::AsmRet() : AsmInstr(INSTR_RET) {
}

// AsmFun
AsmFun::AsmFun(std::string name, AsmInstrPtrs instructions) :
    AsmNode(FUNCTION), name{name}, instructions{std::move(instructions)} {
}

// AsmProg
AsmProg::AsmProg(AsmFunPtr function) :
    AsmNode(PROGRAM), function{std::move(function)} {
}
} // namespace wacc::back::ast