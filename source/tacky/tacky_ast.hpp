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
        FUNCTION,
        PROGRAM,
    };
    TackyNode(Type type);

    virtual ~TackyNode() = default;

    Type type;
};

namespace {
using enum TackyNode::Type;
}

// TackyVal
class TackyVal : public TackyNode {
public:
    TackyVal(Type type);
};

// TackyLitInt
class TackyLitInt : public TackyVal {
public:
    TackyLitInt(int value);

    int value;
};

// TackyVariable
class TackyVariable : public TackyVal {
public:
    TackyVariable(std::string identifier);

    std::string identifier;
};

// TackyInstr
class TackyInstr : public TackyNode {
public:
    TackyInstr(Type type);
};

// TackyReturn
class TackyReturn : public TackyInstr {
public:
    TackyReturn(TackyValPtr val);

    TackyValPtr val;
};

// TackyUnary
class TackyUnary : public TackyInstr {
public:
    enum class Type : std::uint8_t {
        UNARY_COMPLEMENT = 1,
        UNARY_NEGATE,
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
    };

    TackyBinary(Type op, TackyValPtr src1, TackyValPtr src2, TackyValPtr dest);

    Type op;
    TackyValPtr src1;
    TackyValPtr src2;
    TackyValPtr dest;
};

// TackyFun
class TackyFun : public TackyNode {
public:
    TackyFun(std::string identifier, TackyInstrPtrs body);

    std::string identifier;
    TackyInstrPtrs body;
};

// TackyProg
class TackyProg : public TackyNode {
public:
    TackyProg(TackyFunPtr function);

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
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const TackyNode::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum TackyNode::Type;

            case LITERAL_INT:  value = "LITERAL_INT"; break;
            case VARIABLE:     value = "VARIABLE"; break;
            case INSTR_RETURN: value = "INSTR_RETURN"; break;
            case INSTR_UNARY:  value = "INSTR_UNARY"; break;
            case INSTR_BINARY: value = "INSTR_BINARY"; break;
            case FUNCTION:     value = "FUNCTION"; break;
            case PROGRAM:      value = "PROGRAM"; break;
            default:           throw std::format_error("Unhandled TackyNode::Type enum");
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
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const TackyUnary::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum TackyUnary::Type;

            case UNARY_COMPLEMENT: value = "UNARY_COMPLEMENT"; break;
            case UNARY_NEGATE:     value = "UNARY_NEGATE"; break;
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
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const TackyBinary::Type& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum TackyBinary::Type;

            case BINARY_ADD:       value = "BINARY_ADD"; break;
            case BINARY_SUBTRACT:  value = "BINARY_SUBTRACT"; break;
            case BINARY_MULTIPLY:  value = "BINARY_MULTIPLY"; break;
            case BINARY_DIVIDE:    value = "BINARY_DIVIDE"; break;
            case BINARY_REMAINDER: value = "BINARY_REMAINDER"; break;
            default:
                throw std::format_error("Unhandled TackyBinary::Type enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std