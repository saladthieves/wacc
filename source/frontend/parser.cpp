#include "parser.hpp"
#include "ast.hpp"
#include "source.hpp"
#include "token.hpp"

namespace wacc::front::parse {
Parser::Parser(TokensPtr ptr, src::Source source) :
    tokens{std::move(ptr)}, src{source}, current{tokens->cbegin()},
    next{tokens->cbegin()}, last{tokens->cend()} {
}

ast::AstNodePtr Parser::parse() {
    if (!tokens || tokens->empty()) {
        fail("No tokens found");
    }

    return parseProgram();
}

ast::AstProgPtr Parser::parseProgram() {
    auto function = parseFunction();
    return std::make_unique<ast::AstProg>(std::move(function));
}

ast::AstFunPtr Parser::parseFunction() {
    using enum TokenType;

    expect(TokenType::KEYWORD_INT);
    auto name = parseIdentifier();
    expect({OPEN_PAREN, KEYWORD_VOID, CLOSE_PAREN});

    expect(TokenType::OPEN_BRACE);
    auto body = parseStatement();
    expect(TokenType::CLOSE_BRACE);

    return std::make_unique<ast::AstFun>(std::move(name), std::move(body));
}

ast::AstStmtPtr Parser::parseStatement() {
    return parseReturn();
}

ast::AstReturnPtr Parser::parseReturn() {
    expect(TokenType::KEYWORD_RETURN);
    auto expression = parseExpression();
    expect(TokenType::SEMICOLON);

    return std::make_unique<ast::AstReturn>(std::move(expression));
}

ast::AstExprPtr Parser::parseExpression(PrecedenceValue value) {
    auto left = parseFactor();
    while (isBinaryOp(peek().type) && getPrecedence(peek().type) >= value) {
        const auto precedence = getPrecedence(peek().type);
        auto op = parseBinaryOperator();
        auto right = parseExpression(precedence + 1);
        left = std::make_unique<ast::AstBinary>(op, std::move(left),
                                                std::move(right));
    }

    return left;
}

ast::AstExprPtr Parser::parseFactor() {
    switch (peek().type) {
        using enum TokenType;
        case LITERAL_INT: {
            return parseLiteralInteger();
        }
        case OP_BIT_COMPLEMENT:
        case OP_NEGATE:
        case OP_LOG_NOT:        {
            return parseUnaryExpression();
        }
        case OPEN_PAREN: {
            expect(OPEN_PAREN);
            auto expression = parseExpression();
            expect(CLOSE_PAREN);
            return expression;
        }

        default: fail("Unknown or malformed factor expression:");
    }
}

ast::AstIdentPtr Parser::parseIdentifier() {
    const auto& token = expect(TokenType::IDENTIFIER);

    return std::make_unique<ast::AstIdent>(token, token.value);
}

ast::AstLitIntPtr Parser::parseLiteralInteger() {
    const auto& token = expect(TokenType::LITERAL_INT);
    int value = std::stoi(std::string{token.value});

    return std::make_unique<ast::AstLitInt>(token, value);
}

ast::AstUnaryPtr Parser::parseUnaryExpression() {
    auto op = parseUnaryOperator();
    auto expr = parseFactor();
    return std::make_unique<ast::AstUnary>(op, std::move(expr));
}

ast::AstUnary::Type Parser::parseUnaryOperator() {
    using enum TokenType;

    const auto& token = advance();
    ast::AstUnary::Type type{};

    switch (token.type) {
        using enum ast::AstUnary::Type;
        case OP_BIT_COMPLEMENT: type = UNARY_COMPLEMENT; break;
        case OP_NEGATE:         type = UNARY_NEGATE; break;
        case OP_LOG_NOT:        type = UNARY_NOT; break;
        default:
            fail("Cannot parse AstUnaryOpType from ast::TokenType[{}]", type);
    }
    
    sync();
    
    return type;
}

ast::AstBinary::Type Parser::parseBinaryOperator() {
    using enum TokenType;

    const auto& token = advance();
    ast::AstBinary::Type type{};

    switch (token.type) {
        using enum ast::AstBinary::Type;
        case OP_ADDITION:      type = BINARY_ADD; break;
        case OP_NEGATE:        type = BINARY_SUBTRACT; break;
        case OP_MULTIPLY:      type = BINARY_MULTIPLY; break;
        case OP_DIVIDE:        type = BINARY_DIVIDE; break;
        case OP_REMAINDER:     type = BINARY_REMAINDER; break;
        case OP_BIT_AND:       type = BINARY_BIT_AND; break;
        case OP_BIT_OR:        type = BINARY_BIT_OR; break;
        case OP_BIT_XOR:       type = BINARY_BIT_XOR; break;
        case OP_BIT_LSH:       type = BINARY_BIT_LSH; break;
        case OP_BIT_RSH:       type = BINARY_BIT_RSH; break;
        case OP_LOG_AND:       type = BINARY_LOG_AND; break;
        case OP_LOG_OR:        type = BINARY_LOG_OR; break;
        case OP_EQUAL:         type = BINARY_EQUAL; break;
        case OP_NOT_EQUAL:     type = BINARY_NOT_EQUAL; break;
        case OP_LESS_THAN:     type = BINARY_LESS; break;
        case OP_LESS_EQUAL:    type = BINARY_LESS_EQUAL; break;
        case OP_GREATER_THAN:  type = BINARY_GREATER; break;
        case OP_GREATER_EQUAL: type = BINARY_GREATER_EQUAL; break;
        default:
            fail("Cannot parse AstBinaryOpType from TokenType[{}]", token.type);
    }

    sync();

    return type;
}

bool Parser::isBinaryOp(const TokenType& type) {
    using enum TokenType;
    // clang-format off
    switch (type) {
        case OP_ADDITION:       case OP_NEGATE:
        case OP_MULTIPLY:       case OP_DIVIDE:
        case OP_REMAINDER:      case OP_BIT_AND:
        case OP_BIT_OR:         case OP_BIT_XOR:
        case OP_BIT_LSH:        case OP_BIT_RSH:
        case OP_LOG_AND:        case OP_LOG_OR:
        case OP_EQUAL:          case OP_NOT_EQUAL:
        case OP_LESS_THAN:      case OP_LESS_EQUAL:
        case OP_GREATER_THAN:   case OP_GREATER_EQUAL: 
        return true;
        default:               return false;
    }
    // clang-format on
}

unsigned int Parser::getPrecedence(const TokenType& type) {
    for (const auto& entry : precedences) {
        if (entry.first == type) return entry.second;
    }

    return 0;
}

auto Parser::expect(std::initializer_list<const TokenType> types)
    -> const Token& {
    const Token* token{nullptr};
    for (const auto& type : types) {
        token = &advance();
        if (token->type == type) {
            sync();
            continue;
        }

        fail("Expected [{}] but got [{}] instead:", type, token->type);
    }

    return *token;
}

auto Parser::expect(const TokenType& type) -> const Token& {
    const auto& token = advance();
    if (token.type == type) {
        sync();
        return token;
    }

    fail("Expected [{}] but got [{}] instead:", type, token.type);
}

const Parser::PrecedenceMap Parser::precedences{
    {TokenType::OP_MULTIPLY,      100},
    {TokenType::OP_DIVIDE,        100},
    {TokenType::OP_REMAINDER,     100},

    {TokenType::OP_ADDITION,      95 },
    {TokenType::OP_NEGATE,        95 },

    {TokenType::OP_BIT_LSH,       90 },
    {TokenType::OP_BIT_RSH,       90 },

    {TokenType::OP_LESS_THAN,     85 },
    {TokenType::OP_LESS_EQUAL,    85 },
    {TokenType::OP_GREATER_THAN,  85 },
    {TokenType::OP_GREATER_EQUAL, 85 },

    {TokenType::OP_EQUAL,         80 },
    {TokenType::OP_NOT_EQUAL,     80 },

    {TokenType::OP_BIT_AND,       75 },
    {TokenType::OP_BIT_XOR,       70 },
    {TokenType::OP_BIT_OR,        65 },
    {TokenType::OP_LOG_AND,       60 },
    {TokenType::OP_LOG_OR,        55 },
};
} // namespace wacc::front::parse