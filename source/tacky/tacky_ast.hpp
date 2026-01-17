#pragma once

#include <format>
#include <memory>
#include <string>
#include <vector>

namespace wacc {
namespace tacky {
namespace ast {

// Forward declarations
class TackyNode;
class TackyVal;
class TackyLitInt;
class TackyVariable;
class TackyInstr;
class TackyReturn;
class TackyUnary;
class TackyBinary;
class TackyCopy;
class TackyJump;
class TackyJumpZero;
class TackyJumpNotZero;
class TackyLabel;
class TackyFun;
class TackyProg;

// Aliases
using TackyNodePtr = std::unique_ptr<TackyNode>;
using TackyValPtr = std::unique_ptr<TackyVal>;
using TackyLitIntPtr = std::unique_ptr<TackyLitInt>;
using TackyVarPtr = std::unique_ptr<TackyVariable>;
using TackyInstrPtr = std::unique_ptr<TackyInstr>;
using TackyReturnPtr = std::unique_ptr<TackyReturn>;
using TackyUnaryPtr = std::unique_ptr<TackyUnary>;
using TackyBinaryPtr = std::unique_ptr<TackyBinary>;
using TackyCopyPtr = std::unique_ptr<TackyCopy>;
using TackyJumpPtr = std::unique_ptr<TackyJump>;
using TackyJumpZeroPtr = std::unique_ptr<TackyJumpZero>;
using TackyJumpNotZeroPtr = std::unique_ptr<TackyJumpNotZero>;
using TackyLabelPtr = std::unique_ptr<TackyLabel>;
using TackyFunPtr = std::unique_ptr<TackyFun>;
using TackyProgPtr = std::unique_ptr<TackyProg>;

using TackyInstrPtrs = std::vector<TackyInstrPtr>;

// TackyNode
class TackyNode {
public:
    enum class Type : std::uint8_t {
        LITERAL_INT = 1,
        VARIABLE,
        INSTR_RETURN,
        INSTR_UNARY,
        INSTR_BINARY,
        INSTR_COPY,
        INSTR_JUMP,
        INSTR_JUMP_ZERO,
        INSTR_JUMP_NOT_ZERO,
        INSTR_LABEL,
        FUNCTION,
        PROGRAM,
    };
    explicit TackyNode(Type type);

    virtual ~TackyNode() = default;

    Type type;
};

namespace {
using enum TackyNode::Type;
}

// TackyVal
class TackyVal : public TackyNode {
public:
    explicit TackyVal(Type type);
};

// TackyLitInt
class TackyLitInt : public TackyVal {
public:
    explicit TackyLitInt(int value);

    int value;
};

// TackyVariable
class TackyVariable : public TackyVal {
public:
    explicit TackyVariable(std::string_view identifier);

    std::string identifier;
};

// TackyInstr
class TackyInstr : public TackyNode {
public:
    explicit TackyInstr(Type type);
};

// TackyReturn
class TackyReturn : public TackyInstr {
public:
    explicit TackyReturn(TackyValPtr val);

    TackyValPtr val;
};

// TackyUnary
class TackyUnary : public TackyInstr {
public:
    enum class Type : std::uint8_t {
        UNARY_COMPLEMENT = 1,
        UNARY_NEGATE,
        UNARY_NOT,
    };

    TackyUnary(Type op, TackyValPtr src, TackyValPtr dest);

    Type op;
    TackyValPtr src;
    TackyValPtr dest;
};

// TackyBinary
class TackyBinary : public TackyInstr {
public:
    enum class Type : std::uint8_t {
        BINARY_ADD = 1,
        BINARY_SUBTRACT,
        BINARY_MULTIPLY,
        BINARY_DIVIDE,
        BINARY_REMAINDER,
        BINARY_BIT_AND,
        BINARY_BIT_OR,
        BINARY_BIT_XOR,
        BINARY_BIT_LSH,
        BINARY_BIT_RSH,
        BINARY_LOG_AND,
        BINARY_LOG_OR,
        BINARY_EQUAL,
        BINARY_NOT_EQUAL,
        BINARY_LESS,
        BINARY_LESS_EQUAL,
        BINARY_GREATER,
        BINARY_GREATER_EQUAL,
    };

    TackyBinary(Type op, TackyValPtr src1, TackyValPtr src2, TackyValPtr dest);

    Type op;
    TackyValPtr src1;
    TackyValPtr src2;
    TackyValPtr dest;
};

// TackyCopy
class TackyCopy : public TackyInstr {
public:
    TackyCopy(TackyValPtr src, TackyValPtr dest);

    TackyValPtr src;
    TackyValPtr dest;
};

// TackyJump
class TackyJump : public TackyInstr {
public:
    explicit TackyJump(std::string_view target);

    std::string target;
};

// TackyJumpZero
class TackyJumpZero : public TackyInstr {
public:
    TackyJumpZero(TackyValPtr condition, std::string_view target);

    TackyValPtr condition;
    std::string target;
};

// TackyJumpNotZero
class TackyJumpNotZero : public TackyInstr {
public:
    TackyJumpNotZero(TackyValPtr condition, std::string_view target);

    TackyValPtr condition;
    std::string target;
};

// TackyLabel
class TackyLabel : public TackyInstr {
public:
    explicit TackyLabel(std::string_view identifier);

    std::string identifier;
};

// TackyFun
class TackyFun : public TackyNode {
public:
    TackyFun(std::string_view identifier, TackyInstrPtrs body);

    std::string identifier;
    TackyInstrPtrs body;
};

// TackyProg
class TackyProg : public TackyNode {
public:
    explicit TackyProg(TackyFunPtr function);

    TackyFunPtr function;
};
} // namespace ast
} // namespace tacky
} // namespace wacc

namespace std {
namespace {
using wacc::tacky::ast::TackyNode;
}

template <>
class formatter<TackyNode::Type> {
public:
    static constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    static auto format(const TackyNode::Type& type, format_context& context) {
        std::string value{};

        switch (type) {
            using enum TackyNode::Type;

            case LITERAL_INT:         value = "LITERAL_INT"; break;
            case VARIABLE:            value = "VARIABLE"; break;
            case INSTR_RETURN:        value = "INSTR_RETURN"; break;
            case INSTR_UNARY:         value = "INSTR_UNARY"; break;
            case INSTR_BINARY:        value = "INSTR_BINARY"; break;
            case INSTR_COPY:          value = "INSTR_COPY"; break;
            case INSTR_JUMP:          value = "INSTR_JUMP"; break;
            case INSTR_JUMP_ZERO:     value = "INSTR_JUMP_ZERO"; break;
            case INSTR_JUMP_NOT_ZERO: value = "INSTR_JUMP_NOT_ZERO"; break;
            case INSTR_LABEL:         value = "INSTR_LABEL"; break;
            case FUNCTION:            value = "FUNCTION"; break;
            case PROGRAM:             value = "PROGRAM"; break;
            default:                  throw std::format_error("Unhandled TackyNode::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::tacky::ast::TackyUnary;
}

template <>
class formatter<TackyUnary::Type> {
public:
    static constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    static auto format(const TackyUnary::Type& type, format_context& context) {
        std::string value{};

        switch (type) {
            using enum TackyUnary::Type;

            case UNARY_COMPLEMENT: value = "UNARY_COMPLEMENT"; break;
            case UNARY_NEGATE:     value = "UNARY_NEGATE"; break;
            case UNARY_NOT:        value = "UNARY_NOT"; break;
            default:               throw std::format_error("Unhandled TackyUnary::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::tacky::ast::TackyBinary;
}

template <>
class formatter<TackyBinary::Type> {
public:
    static constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    static auto format(const TackyBinary::Type& type, format_context& context) {
        std::string value{};

        switch (type) {
            using enum TackyBinary::Type;

            case BINARY_ADD:           value = "BINARY_ADD"; break;
            case BINARY_SUBTRACT:      value = "BINARY_SUBTRACT"; break;
            case BINARY_MULTIPLY:      value = "BINARY_MULTIPLY"; break;
            case BINARY_DIVIDE:        value = "BINARY_DIVIDE"; break;
            case BINARY_REMAINDER:     value = "BINARY_REMAINDER"; break;
            case BINARY_BIT_AND:       value = "BINARY_BIT_AND"; break;
            case BINARY_BIT_OR:        value = "BINARY_BIT_OR"; break;
            case BINARY_BIT_XOR:       value = "BINARY_BIT_XOR"; break;
            case BINARY_BIT_LSH:       value = "BINARY_BIT_LSH"; break;
            case BINARY_BIT_RSH:       value = "BINARY_BIT_RSH"; break;
            case BINARY_EQUAL:         value = "BINARY_EQUAL"; break;
            case BINARY_LOG_AND:       value = "BINARY_LOG_AND"; break;
            case BINARY_LOG_OR:        value = "BINARY_LOG_OR"; break;
            case BINARY_NOT_EQUAL:     value = "BINARY_NOT_EQUAL"; break;
            case BINARY_LESS:          value = "BINARY_LESS"; break;
            case BINARY_LESS_EQUAL:    value = "BINARY_LESS_EQUAL"; break;
            case BINARY_GREATER:       value = "BINARY_GREATER"; break;
            case BINARY_GREATER_EQUAL: value = "BINARY_GREATER_EQUAL"; break;
            default:
                throw std::format_error("Unhandled TackyBinary::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std