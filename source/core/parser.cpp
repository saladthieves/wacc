#include "parser.hpp"
#include "ast.hpp"
#include "formatting.hpp"

#include <format>
#include <stdexcept>
#include <string>

namespace wacc::core::parse {
Parser::Parser(TokensPtr ptr) :
    tokens{std::move(ptr)}, current{tokens->cbegin()}, next{tokens->cbegin()},
    end{tokens->cbegin()} {
}

ast::AstNodePtr Parser::parse() {
    if (tokens->empty()) {
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
    return parseInteger();
}

ast::AstIdentPtr Parser::parseIdentifier() {
    const auto& token = expect(TokenType::IDENTIFIER);

    return std::make_unique<ast::AstIdent>(token, token.value);
}

ast::AstIntPtr Parser::parseInteger() {
    const auto& token = expect(TokenType::CONSTANT_INT);
    int value = std::stoi(std::string{token.value});

    return std::make_unique<ast::AstInt>(token, value);
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

        auto message = std::format("Expected [{}] but got [{}] instead", type,
                                   token->type);
        fail(message);
    }

    return *token;
}

auto Parser::expect(const TokenType& type) -> const Token& {
    const auto& token = advance();
    if (token.type == type) {
        sync();
        return token;
    }

    auto message =
        std::format("Expected [{}] but got [{}].", type, token.type);
    fail(message);
}

[[noreturn]] void Parser::fail(std::string_view message) {
    auto output = std::format("ParserError:\n  message: {}", message);
    throw std::runtime_error(output);
}
} // namespace wacc::core::parse