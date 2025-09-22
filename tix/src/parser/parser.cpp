#include "parser.hpp"
#include "../lexer/lexer.hpp"
#include "nodes.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include <vector>
#include <utility>

#define tt this->current_tok.type
#define tv this->current_tok.value
#define tc this->current_tok.ctx

#define EOF lexer::TokenType::EndOfFile
#define Plus lexer::TokenType::Plus
#define Minus lexer::TokenType::Minus
#define Mult lexer::TokenType::Mult
#define Div lexer::TokenType::Div
#define Int lexer::TokenType::Int
#define Double lexer::TokenType::Double

#define additive !this->overflow() && (tt == Plus || tt == Minus)
#define multiplicative !this->overflow() && (tt == Mult || tt == Div)

namespace parser {
    ParseResult::ParseResult(Statement* result, const std::vector<errors::Error> errors) {
        this->result = result; // this will turn nullptr if an error occurs
        this->errors = errors;
    }

    Parser::Parser(const std::vector<lexer::Token> tokens) {
        this->tokens = tokens;
        this->errors.clear();
        this->block = new BlockStatement(this->tokens[0].ctx);
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

            lhs.result = new BinaryExpression(lhs.result->ctx, static_cast<Expression*>(lhs.result), op.value, static_cast<Expression*>(rhs.result));
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

            lhs.result = new BinaryExpression(lhs.result->ctx, static_cast<Expression*>(lhs.result), op.value, static_cast<Expression*>(rhs.result));
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
            ParseResult returned = ParseResult(new UnaryExpression(op.ctx, op.value, static_cast<Expression*>(pr.result)), pr.errors);
            if (pr.result) {
                op.ctx.end = pr.result->ctx.end;
            }

            return returned;
        }

        return this->primary_expression();
    }

    ParseResult Parser::primary_expression() {
        switch (tt) {
        case Int: {
            ParseResult returned = ParseResult(new IntExpression(tc, std::stoll(tv)), {});
            this->advance();
            return returned;
        }
        case Double: {
            ParseResult returned = ParseResult(new DoubleExpression(tc, std::stod(tv)), {});
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