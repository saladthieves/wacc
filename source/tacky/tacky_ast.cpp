#include "tacky_ast.hpp"

namespace wacc::tacky::ast {
// TackyNode
TackyNode::TackyNode(Type type) : type{type} {
}

// TackyVal
TackyVal::TackyVal(Type type) : TackyNode(type) {
}

// TackyLitInt
TackyLitInt::TackyLitInt(int value) : TackyVal(LITERAL_INT), value{value} {
}

// TackyVariable
TackyVariable::TackyVariable(std::string_view identifier) :
    TackyVal(VARIABLE), identifier{identifier} {
}

// TackyInstr
TackyInstr::TackyInstr(Type type) : TackyNode(type) {
}

// TackyReturn
TackyReturn::TackyReturn(TackyValPtr val) :
    TackyInstr(INSTR_RETURN), val{std::move(val)} {
}

// TackyUnary
TackyUnary::TackyUnary(Type op, TackyValPtr src, TackyValPtr dest) :
    TackyInstr(INSTR_UNARY), op{op}, src{std::move(src)},
    dest{std::move(dest)} {
}

// TackyBinary
TackyBinary::TackyBinary(Type op, TackyValPtr src1, TackyValPtr src2,
                         TackyValPtr dest) :
    TackyInstr(INSTR_BINARY), op{op}, src1{std::move(src1)},
    src2{std::move(src2)}, dest{std::move(dest)} {
}

// TackyCopy
TackyCopy::TackyCopy(TackyValPtr src, TackyValPtr dest) :
    TackyInstr(INSTR_COPY), src{std::move(src)}, dest{std::move(dest)} {
}

// TackyJump
TackyJump::TackyJump(std::string_view target) :
    TackyInstr(INSTR_JUMP), target{target} {
}

// TackyJumpZero
TackyJumpZero::TackyJumpZero(TackyValPtr condition, std::string_view target) :
    TackyInstr(INSTR_JUMP_ZERO), condition{std::move(condition)},
    target{target} {
}

// TackyJumpNotZero
TackyJumpNotZero::TackyJumpNotZero(TackyValPtr condition,
                                   std::string_view target) :
    TackyInstr(INSTR_JUMP_NOT_ZERO), condition{std::move(condition)},
    target{target} {
}

// TackyLabel
TackyLabel::TackyLabel(std::string_view identifier) :
    TackyInstr(INSTR_LABEL), identifier{identifier} {
}

// TackyFun
TackyFun::TackyFun(std::string_view identifier, TackyInstrPtrs body) :
    TackyNode(FUNCTION), identifier{identifier}, body{std::move(body)} {
}

// TackyProg
TackyProg::TackyProg(TackyFunPtr function) :
    TackyNode(PROGRAM), function{std::move(function)} {
}

} // namespace wacc::tacky::ast