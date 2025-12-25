#include "tacky_ast.hpp"

namespace wacc::tacky::ast {
// TackyConstant
TackyConstant::TackyConstant(int value) : value{value} {
}

// TackyVariable
TackyVariable::TackyVariable(std::string identifier) : identifier{identifier} {
}

// TackyReturn
TackyReturn::TackyReturn(TackyValPtr val) : val{std::move(val)} {
}

// TackyUnary
TackyUnary::TackyUnary(TackyUnaryOpType op, TackyValPtr src, TackyValPtr dest) :
    op{op}, src{std::move(src)}, dest{std::move(dest)} {
}

// TackyBinary
TackyBinary::TackyBinary(TackyBinaryOpType op, TackyValPtr src1,
                         TackyValPtr src2, TackyValPtr dest) :
    op{op}, src1{std::move(src1)}, src2{std::move(src2)},
    dest{std::move(dest)} {
}

// TackyFun
TackyFun::TackyFun(std::string identifier, TackyInstrPtrs body) :
    identifier{identifier}, body{std::move(body)} {
}

// TackyProg
TackyProg::TackyProg(TackyFunPtr function) : function{std::move(function)} {
}

} // namespace wacc::tacky::ast