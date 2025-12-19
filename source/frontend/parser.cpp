#include "parser.hpp"
#include "ast.hpp"
#include "source.hpp"
#include "token.hpp"

namespace wacc::front::parse {
Parser::Parser(TokensPtr ptr, src::Source source) :
    tokens{std::move(ptr)}, src{source}, current{tokens->cbegin()},
    next{tokens->cbegin()}, end{tokens->cbegin()} {
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

ast::AstExprPtr Parser::parseExpression() {
    const auto& type = peek().type;
    switch (type) {
        using enum TokenType;
        case CONSTANT_INT:  return parseConstantInteger();
        case OP_COMPLEMENT:
        case OP_NEGATE:     {
            return parseUnaryExpression();
        }
        case OPEN_PAREN: {
            expect(OPEN_PAREN);
            auto expression = parseExpression();
            expect(CLOSE_PAREN);
            return expression;
        }
        default: fail("Unknown or malformed expression:");
    }
}

ast::AstIdentPtr Parser::parseIdentifier() {
    const auto& token = expect(TokenType::IDENTIFIER);

    return std::make_unique<ast::AstIdent>(token, token.value);
}

ast::AstConstIntPtr Parser::parseConstantInteger() {
    const auto& token = expect(TokenType::CONSTANT_INT);
    int value = std::stoi(std::string{token.value});

    return std::make_unique<ast::AstConstInt>(token, value);
}

ast::AstUnaryPtr Parser::parseUnaryExpression() {
    auto op = parseUnaryOperator();
    auto expr = parseExpression();
    return std::make_unique<ast::AstUnary>(op, std::move(expr));
}

ast::AstUnaryOpType Parser::parseUnaryOperator() {
    using enum TokenType;
    const auto& type = expectAny({OP_COMPLEMENT, OP_NEGATE}).type;
    
    switch (type) {
        case OP_COMPLEMENT: return ast::AstUnaryOpType::UNARY_COMPLEMENT;
        case OP_NEGATE:     return ast::AstUnaryOpType::UNARY_NEGATE;
        default:
            fail("Cannot parse AstUnaryOpType from ast::TokenType[{}]", type);
    }
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

auto Parser::expectAny(std::initializer_list<const TokenType> types)
    -> const Token& {
    const auto& token = advance();
    for (const auto& type : types) {
        if (token.type == type) {
            sync();
            return token;
        }
    }

    std::string output{};
    auto begin = types.begin();
    auto end = types.end();
    for (; begin != end; ++begin) {
        output += std::format("{}", *begin);
        if (begin != end - 1) {
            output += ", ";
        }
    }

    fail("Exected any of [{}] but got [{}] instead:", output, token.type);
}
} // namespace wacc::front::parse