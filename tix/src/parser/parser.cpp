#include "parser.hpp"
#include "../lexer/lexer.hpp"
#include "nodes.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "../context/positions.hpp"
#include <vector>
#include <utility>
#include <iterator>
#include <memory>
#include <string>

#define tt this->current_tok.type
#define tv this->current_tok.value
#define tc this->current_tok.ctx

#define EOF lexer::TokenType::EndOfFile
#define Semi lexer::TokenType::Semi
#define Plus lexer::TokenType::Plus
#define Minus lexer::TokenType::Minus
#define Mult lexer::TokenType::Mult
#define Div lexer::TokenType::Div
#define Mod lexer::TokenType::Mod
#define LParen lexer::TokenType::LParen
#define RParen lexer::TokenType::RParen
#define Int lexer::TokenType::Int
#define Double lexer::TokenType::Double

#define additive !this->overflow() && (tt == Plus || tt == Minus)
#define multiplicative !this->overflow() && (tt == Mult || tt == Div || tt == Mod)

namespace parser {
    ParseResult::ParseResult(std::shared_ptr<Statement> result, const std::vector<errors::Error> errors) {
        this->result = result; // this will turn nullptr if an error occurs
        this->errors = errors;
    }

    Parser::Parser(const std::string fn, const std::string src, const std::vector<lexer::Token> tokens) {
        this->fn = fn;
        this->src = src;
        this->tokens = tokens;
        this->errors.clear();
        this->block = std::make_shared<BlockStatement>(BlockStatement(context::Context(this->fn, this->src, this->tokens[0].ctx.start, (*std::prev(this->tokens.end())).ctx.end)));
        this->idx = -1;
        this->advance();
    }

    void Parser::advance() {
        this->idx++;
        if (!this->overflow()) {
            this->current_tok = this->tokens[this->idx];
        }
    }

    bool Parser::overflow() {
        return (this->idx >= this->tokens.size());
    }

    ParseResult Parser::eol() {
        if (tt != Semi) {
            return ParseResult(nullptr, {errors::SyntaxError(tc, "expected semicolon")});
        }

        return ParseResult(nullptr, {});
    }

    ParseResult Parser::expect(const lexer::TokenType expected, const std::string error_msg) {
        if (tt != expected) {
            return ParseResult(nullptr, {errors::SyntaxError(tc, error_msg)});
        }
        
        return ParseResult(nullptr, {});
    }

    void Parser::parse() {
        while (!this->overflow() && tt != EOF) {
            ParseResult pr = this->statement();
            this->block->body.push_back(pr.result);
            for (const errors::Error &err : pr.errors) {
                this->errors.push_back(err);
            }

            if (pr.result == nullptr) {
                this->advance();
            }

            pr = this->eol();
            for (const errors::Error &err : pr.errors) {
                this->errors.push_back(err);
            }

            this->advance();
        }
    }

    ParseResult Parser::statement() {
        return this->expression();
    }

    ParseResult Parser::expression() {
        return this->additive_expression();
    }

    ParseResult Parser::additive_expression() {
        ParseResult lhs = this->multiplicative_expression();
        if (lhs.result == nullptr) {
            return lhs;
        }

        while (additive) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult rhs = this->multiplicative_expression();
            for (const errors::Error &err : rhs.errors) {
                lhs.errors.push_back(err);
            }

            lhs.result = std::make_shared<BinaryExpression>(BinaryExpression(context::Context(this->fn, this->src, lhs.result->ctx.start, rhs.result->ctx.end), std::static_pointer_cast<Expression>(lhs.result), op.value, std::static_pointer_cast<Expression>(rhs.result)));
            if (rhs.result) {
                lhs.result->ctx.end = rhs.result->ctx.end;
            } else {
                lhs.result->ctx.end = op.ctx.end;
            }
        }

        return lhs;
    }

    ParseResult Parser::multiplicative_expression() {
        ParseResult lhs = this->unary_expression();
        if (lhs.result == nullptr) {
            return lhs;
        }

        while (multiplicative) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult rhs = this->unary_expression();
            for (const errors::Error &err : rhs.errors) {
                lhs.errors.push_back(err);
            }

            lhs.result = std::make_shared<BinaryExpression>(BinaryExpression(context::Context(this->fn, this->src, lhs.result->ctx.start, rhs.result->ctx.end), std::static_pointer_cast<Expression>(lhs.result), op.value, std::static_pointer_cast<Expression>(rhs.result)));
            if (rhs.result) {
                lhs.result->ctx.end = rhs.result->ctx.end;
            } else {
                lhs.result->ctx.end = op.ctx.end;
            }
        }

        return lhs;
    }

    ParseResult Parser::unary_expression() {
        if (additive) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult pr = this->unary_expression();
            ParseResult returned = ParseResult(std::make_shared<UnaryExpression>(UnaryExpression(context::Context(this->fn, this->src, op.ctx.start, pr.result->ctx.end), op.value, std::static_pointer_cast<Expression>(pr.result))), pr.errors);
            if (pr.result) {
                op.ctx.end = pr.result->ctx.end;
            }

            return returned;
        }

        return this->primary_expression();
    }

    ParseResult Parser::primary_expression() {
        switch (tt) {
        case LParen: {
            this->advance();
            ParseResult expr = this->expression();
            ParseResult pr = this->expect(RParen, "expected ')'");
            if (!pr.errors.empty()) {
                return pr;
            }

            this->advance();
            return expr;
        }
        case Int: {
            ParseResult returned = ParseResult(std::make_shared<IntExpression>(IntExpression(tc, std::stoll(tv))), {});
            this->advance();
            return returned;
        }
        case Double: {
            ParseResult returned = ParseResult(std::make_shared<DoubleExpression>(DoubleExpression(tc, std::stod(tv))), {});
            this->advance();
            return returned;
        }
        default: {
            ParseResult returned = ParseResult(nullptr, {errors::SyntaxError(tc, "Invalid syntax!")});
            this->advance();
            return returned;
        }
        }
    }

}
