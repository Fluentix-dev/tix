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
#include <iostream>

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
#define Percent lexer::TokenType::Percent
#define LParen lexer::TokenType::LParen
#define RParen lexer::TokenType::RParen
#define Equals lexer::TokenType::Equals
#define Int lexer::TokenType::Int
#define Double lexer::TokenType::Double
#define Ident lexer::TokenType::Ident
#define Const lexer::TokenType::Const
#define LBrac lexer::TokenType::LBrac
#define RBrac lexer::TokenType::RBrac

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
            while (!this->overflow() && tt == Semi) {
                this->advance();
            }

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
        switch (tt) {
        case Const: {
            lexer::Token const_tok = this->current_tok;
            this->advance();

            ParseResult data_type = this->expression();
            ParseResult pr = this->expect(Ident, "expected variable name in constant variable declaration");
            for (const errors::Error &err : pr.errors) {
                data_type.errors.push_back(err);
            }

            if (!pr.errors.empty()) {
                return data_type;
            }

            data_type = this->variable_declaration_statement(data_type);
            std::shared_ptr<VariableDeclarationStatement> variable_declaration = std::static_pointer_cast<VariableDeclarationStatement>(data_type.result);
            variable_declaration->constant = true;
            variable_declaration->ctx.start = const_tok.ctx.start;

            return ParseResult(variable_declaration, data_type.errors);
        }
        default: {
            ParseResult pr = this->expression();
            if (tt == Ident) {
                return this->variable_declaration_statement(pr);
            }

            return pr;
        }
        }
    }

    ParseResult Parser::variable_declaration_statement(ParseResult data_type) {
        lexer::Token variable = this->current_tok;
        std::string var_name = tv;
        this->advance();
        ParseResult pr = this->expect(Equals, "expected '=' in variable declaration");

        lexer::Token equals = this->current_tok;
        this->advance();
        ParseResult value = this->expression();
        
        context::Position start;
        context::Position end;
        if (data_type.errors.empty()) {
            start = data_type.result->ctx.start;
        } else {
            start = variable.ctx.start;
        }

        if (value.errors.empty()) {
            end = value.result->ctx.end;
        } else {
            end = equals.ctx.end;
        }

        for (const errors::Error &err : pr.errors) {
            data_type.errors.push_back(err);
        }

        for (const errors::Error &err : value.errors) {
            data_type.errors.push_back(err);
        }

        return ParseResult(std::make_shared<VariableDeclarationStatement>(VariableDeclarationStatement(context::Context(this->fn, this->src, start, end), false, std::static_pointer_cast<Expression>(data_type.result), var_name, std::static_pointer_cast<Expression>(value.result))), data_type.errors);
    }

    ParseResult Parser::expression() {
        return this->assignment_expression();
    }

    ParseResult Parser::assignment_expression() {
        ParseResult pr = this->additive_expression();
        if (tt != Equals) {
            return pr;
        }

        lexer::Token equals = this->current_tok;
        this->advance();
        ParseResult pr2 = this->assignment_expression();

        context::Position start;
        context::Position end;
        if (pr.errors.empty()) {
            start = pr.result->ctx.start;
            if (pr.result->node_type != NodeType::IdentifierExpr) {
                pr.errors.push_back(errors::SyntaxError(pr.result->ctx, "expected a variable inside assignment"));
            }
        } else {
            start = equals.ctx.start;
        }

        if (pr2.errors.empty()) {
            end = pr2.result->ctx.end;
        } else {
            end = equals.ctx.end;
        }

        for (const errors::Error &err : pr2.errors) {
            pr.errors.push_back(err);
        }

        return ParseResult(std::make_shared<AssignmentExpression>(AssignmentExpression(context::Context(this->fn, this->src, start, end), std::static_pointer_cast<Expression>(pr.result), std::static_pointer_cast<Expression>(pr2.result))), pr.errors);
    }

    ParseResult Parser::additive_expression() {
        ParseResult lhs = this->multiplicative_expression();
        while (additive) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult rhs = this->multiplicative_expression();
            for (const errors::Error &err : rhs.errors) {
                lhs.errors.push_back(err);
            }

            context::Position start;
            context::Position end;
            if (lhs.errors.empty()) {
                start = lhs.result->ctx.start;
            } else {
                start = op.ctx.start;
            }

            if (rhs.errors.empty()) {
                end = rhs.result->ctx.end;
            } else {
                end = op.ctx.end;
            }

            lhs.result = std::make_shared<BinaryExpression>(BinaryExpression(context::Context(this->fn, this->src, start, end), std::static_pointer_cast<Expression>(lhs.result), op.value, std::static_pointer_cast<Expression>(rhs.result)));
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

            context::Position start;
            context::Position end;
            if (lhs.errors.empty()) {
                start = lhs.result->ctx.start;
            } else {
                start = op.ctx.start;
            }

            if (rhs.errors.empty()) {
                end = rhs.result->ctx.end;
            } else {
                end = op.ctx.end;
            }

            lhs.result = std::make_shared<BinaryExpression>(BinaryExpression(context::Context(this->fn, this->src, start, end), std::static_pointer_cast<Expression>(lhs.result), op.value, std::static_pointer_cast<Expression>(rhs.result)));
        }

        return lhs;
    }

    ParseResult Parser::unary_expression() {
        if (additive) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult pr = this->unary_expression();
            context::Position end;
            if (pr.errors.empty()) {
                end = pr.result->ctx.end;
            } else {
                end = op.ctx.end;
            }

            ParseResult returned = ParseResult(std::make_shared<UnaryExpression>(UnaryExpression(context::Context(this->fn, this->src, op.ctx.start, end), op.value, std::static_pointer_cast<Expression>(pr.result))), pr.errors);
            return returned;
        }

        ParseResult pr = this->primary_expression();
        if (tt == Percent) {
            lexer::Token op = this->current_tok;
            this->advance();

            context::Position start;
            if (pr.errors.empty()) {
                start = pr.result->ctx.start;
            } else {
                start = op.ctx.start;
            }

            return ParseResult(std::make_shared<UnaryExpression>(UnaryExpression(context::Context(this->fn, this->src, start, op.ctx.end), "%", std::static_pointer_cast<Expression>(pr.result))), pr.errors);
        }

        return pr;
    }

    ParseResult Parser::primary_expression() {
        switch (tt) {
        case LParen: {
            lexer::Token lparen = this->current_tok;
            this->advance();
            ParseResult expr = this->expression();
            ParseResult pr = this->expect(RParen, "expected ')'");
            this->advance();
            if (!pr.errors.empty()) {
                return pr;
            }

            lexer::Token rparen = this->current_tok;
            expr.result->ctx.start = lparen.ctx.start;
            expr.result->ctx.end = rparen.ctx.end;
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
        case Ident: {
            return this->list_type_expression();
        }
        default: {
            ParseResult returned = ParseResult(nullptr, {errors::SyntaxError(tc, "Invalid syntax!")});
            this->advance();
            return returned;
        }
        }
    }

    ParseResult Parser::list_type_expression() {
        ParseResult returned = ParseResult(std::make_shared<IdentifierExpression>(IdentifierExpression(tc, tv)), {});
        this->advance();
        while (tt == LBrac) {
            lexer::Token lbrac = this->current_tok;
            this->advance();
            // Expect a ']'
            ParseResult pr = this->expect(RBrac, "expected ']'");
            if (!pr.errors.empty()) {
                return pr;
            }

            context::Position start;
            if (returned.errors.empty()) {
                start = returned.result->ctx.start;
            } else {
                start = lbrac.ctx.start;
            }

            returned = ParseResult(std::make_shared<ListTypeExpression>(ListTypeExpression(context::Context(this->fn, this->src, start, tc.end), std::static_pointer_cast<Expression>(returned.result))), pr.errors);
            this->advance();

            // Continue looping because there can be a matrix and so on,...
        }

        return returned;
    }
}