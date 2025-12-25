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
class TackyConstant;
class TackyVariable;
class TackyInstr;
class TackyReturn;
class TackyUnary;
class TackyFun;
class TackyProg;

// Aliases
using TackyNodePtr = std::unique_ptr<TackyNode>;
using TackyValPtr = std::unique_ptr<TackyVal>;
using TackyConstPtr = std::unique_ptr<TackyConstant>;
using TackyVarPtr = std::unique_ptr<TackyVariable>;
using TackyInstrPtr = std::unique_ptr<TackyInstr>;
using TackyReturnPtr = std::unique_ptr<TackyReturn>;
using TackyUnaryPtr = std::unique_ptr<TackyUnary>;
using TackyFunPtr = std::unique_ptr<TackyFun>;
using TackyProgPtr = std::unique_ptr<TackyProg>;

using TackyInstrPtrs = std::vector<TackyInstrPtr>;

enum class TackyNodeType : unsigned {
    CONSTANT = 1,
    VARIABLE,
    INSTR_RETURN,
    INSTR_UNARY,
    FUNCTION,
    PROGRAM,
};

enum class TackyUnaryOpType : unsigned {
    UNARY_COMPLEMENT = 1,
    UNARY_NEGATE,
};

namespace {
using enum TackyNodeType;
}

// TackyNode
class TackyNode {
public:
    virtual TackyNodeType type() const = 0;

    virtual ~TackyNode() = default;
};

// TackyVal
class TackyVal : public TackyNode {
public:
    virtual TackyNodeType type() const override = 0;
};

// TackyConstant
class TackyConstant : public TackyVal {
public:
    TackyConstant(int value);

    virtual TackyNodeType type() const override { return CONSTANT; }

    int value;
};

// TackyVariable
class TackyVariable : public TackyVal {
public:
    TackyVariable(std::string identifier);

    virtual TackyNodeType type() const override { return VARIABLE; }

    std::string identifier;
};

// TackyInstr
class TackyInstr : public TackyNode {
public:
    virtual TackyNodeType type() const override = 0;
};

// TackyReturn
class TackyReturn : public TackyInstr {
public:
    TackyReturn(TackyValPtr val);

    virtual TackyNodeType type() const override { return INSTR_RETURN; }

    TackyValPtr val;
};

// TackyUnary
class TackyUnary : public TackyInstr {
public:
    TackyUnary(TackyUnaryOpType op, TackyValPtr src, TackyValPtr dest);

    virtual TackyNodeType type() const override { return INSTR_UNARY; }

    TackyUnaryOpType op;
    TackyValPtr src;
    TackyValPtr dest;
};

// TackyFun
class TackyFun : public TackyNode {
public:
    TackyFun(std::string identifier, TackyInstrPtrs body);

    virtual TackyNodeType type() const override { return FUNCTION; }

    std::string identifier;
    TackyInstrPtrs body;
};

// TackyProg
class TackyProg : public TackyNode {
public:
    TackyProg(TackyFunPtr function);

    virtual TackyNodeType type() const override { return PROGRAM; }

    TackyFunPtr function;
};

} // namespace ast
} // namespace tacky
} // namespace wacc

namespace std {
namespace {
using wacc::tacky::ast::TackyNodeType;
}

template <>
class formatter<TackyNodeType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const TackyNodeType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum TackyNodeType;

            case CONSTANT:     value = "CONSTANT"; break;
            case VARIABLE:     value = "VARIABLE"; break;
            case INSTR_RETURN: value = "INSTR_RETURN"; break;
            case INSTR_UNARY:  value = "INSTR_UNARY"; break;
            case FUNCTION:     value = "FUNCTION"; break;
            case PROGRAM:      value = "PROGRAM"; break;
            default:
                throw std::format_error(
                    "Unhandled tacky::ast::TackyNodeType enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};

namespace {
using wacc::tacky::ast::TackyUnaryOpType;
}

template <>
class formatter<TackyUnaryOpType> {
public:
    constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    auto format(const TackyUnaryOpType& type, format_context& context) const {
        std::string value{};

        switch (type) {
            using enum TackyUnaryOpType;

            case UNARY_COMPLEMENT: value = "UNARY_COMPLEMENT"; break;
            case UNARY_NEGATE:     value = "UNARY_NEGATE"; break;
            default:
                throw std::format_error(
                    "Unhandled tacky::ast::TackyUnaryOpType enum");
        }

        return std::format_to(context.out(), "{}", value);
    }
};
} // namespace std