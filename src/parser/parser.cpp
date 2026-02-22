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
#include <utility>

#define tt this->current_tok.type
#define tv this->current_tok.value
#define tc this->current_tok.ctx

using enum lexer::TokenType;
#define EOF EndOfFile

#define comparative !this->overflow() && (tt == EqualComp || tt == NotEquals || tt == Greater || tt == GreaterOrEquals || tt == Smaller || tt == SmallerOrEquals)
#define additive !this->overflow() && (tt == Plus || tt == Minus)
#define multiplicative !this->overflow() && (tt == Mult || tt == Div || tt == Mod)
#define unary !this->overflow() && (tt == Plus || tt == Minus || tt == Not || tt == Increment || tt == Decrement)

namespace parser {
    bool is_assignable(const std::shared_ptr<Statement> node) {
        return node->node_type == NodeType::IdentifierExpr;
    }

    std::pair<std::string, std::string> encode_string(const std::string original) {
        std::string new_str = "";
        size_t string_idx = 0;
        while (string_idx < original.size()) {
            if (original[string_idx] == '\\') {
                char escaped_char = original[++string_idx];
                string_idx++;
                switch (escaped_char) {
                case '\'':
                    new_str += "'";
                    break;
                case '"':
                    new_str += '"';
                    break;
                case '\\':
                    new_str += "\\";
                    break;
                case 'n':
                    new_str += "\n";
                    break;
                case 'r':
                    new_str += "\r";
                    break;
                case 't':
                    new_str += "\t";
                    break;
                case 'b':
                    new_str += "\b";
                    break;
                case 'f':
                    new_str += "\f";
                    break;
                case 'v':
                    new_str += "\v";
                    break;
                case '0':
                    new_str += '\0';
                    break;
                case 'a':
                    new_str += "\a";
                    break;
                case 'x': {
                    if (string_idx+1 >= original.size()) {
                        return {"", "unicode escape \\x can't be decoded because of invalid syntax"};
                    }

                    char first = original[string_idx++];
                    char second = original[string_idx++];
                    if (!(('0' <= first && first <= '9') || ('a' <= first && first <= 'f') || ('A' <= first && first <= 'F'))) {
                        return {"", "unicode escape \\x can't be decoded because of invalid syntax"};
                    }

                    if (!(('0' <= second && second <= '9') || ('a' <= second && second <= 'f') || ('A' <= second && second <= 'F'))) {
                        return {"", "unicode escape \\x can't be decoded because of invalid syntax"};
                    }

                    size_t ascii_val;
                    if ('0' <= first && first <= '9') {
                        ascii_val = (first - '0')*16;
                    } else if ('a' <= first && first <= 'f') {
                        ascii_val = (first - 'a' + 11)*16;
                    } else {
                        ascii_val = (first - 'A' + 11)*16;
                    }

                    if ('0' <= second && second <= '9') {
                        ascii_val += second - '0';
                    } else if ('a' <= second && second <= 'f') {
                        ascii_val += second - 'a' + 11;
                    } else {
                        ascii_val += second - 'A' + 11;
                    }

                    char new_char = (char)ascii_val;
                    new_str += new_char;
                    break;
                }
                default:
                    return {"", "unexpected character after '\\'"};
                }
            } else {
                new_str += original[string_idx++];
            }
        }

        return {new_str, ""};
    }

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

            if (tt == EOF) {
                break;
            }

            ParseResult pr = this->statement(true);
            this->block->body.push_back(pr.result);
            for (const errors::Error &err : pr.errors) {
                this->errors.push_back(err);
            }
        }
    }

    ParseResult Parser::statement(const bool eol) {
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

            if (eol) {
                pr = this->eol();
                for (const errors::Error &err : pr.errors) {
                    data_type.errors.push_back(err);
                }
            }

            return ParseResult(variable_declaration, data_type.errors);
        }
        case If:
            return this->if_else_statement();
        case While:
            return this->while_statement();
        case For:
            return this->for_statement();
        default: {
            ParseResult pr = this->expression();
            if (tt == Ident) {
                pr = this->variable_declaration_statement(pr);
                if (eol) {
                    ParseResult pr2 = this->eol();
                    for (const errors::Error &err : pr.errors) {
                        pr.errors.push_back(err);
                    }
                }
            }

            if (eol) {
                ParseResult pr2 = this->eol();
                for (const errors::Error &err : pr2.errors) {
                    pr.errors.push_back(err);
                }
            }

            return pr;
        }
        }
    }

    ParseResult Parser::inner_block() {
        context::Position pos_start = tc.start;
        std::vector<std::shared_ptr<Statement>> stmt = {};
        std::vector<errors::Error> errs = {};
        this->advance();
        while (!this->overflow() && tt != EOF && tt != RCurl) {
            while (!this->overflow() && tt == Semi) {
                this->advance();
            }

            if (tt == EOF) {
                break;
            }

            if (tt == RCurl) {
                break;
            }
            
            ParseResult pr = this->statement(true);
            for (const errors::Error &err : pr.errors) {
                errs.push_back(err);
            }

            stmt.push_back(pr.result);
        }

        ParseResult pr = this->expect(RCurl, "expected '}' to end the body");
        for (const errors::Error &err : pr.errors) {
            errs.push_back(err);
        }

        context::Position pos_end = tc.end;
        this->advance();

        std::shared_ptr<BlockStatement> returned = std::make_shared<BlockStatement>(BlockStatement(context::Context(this->fn, this->src, pos_start, pos_end)));
        returned->body = stmt;
        
        return ParseResult(returned, errs);
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

    ParseResult Parser::if_else_statement() {
        context::Position pos_start = tc.start;
        this->advance();

        ParseResult condition = this->expression();
        std::vector<errors::Error> errs = {};
        for (const errors::Error &err : condition.errors) {
            errs.push_back(err);
        }

        ParseResult pr = this->expect(LCurl, "expected '{' to start a body");
        for (const errors::Error &err : pr.errors) {
            errs.push_back(err);
        }

        ParseResult body = this->inner_block();
        for (const errors::Error &err : body.errors) {
            errs.push_back(err);
        }

        if (tt != Else) {
            return ParseResult(std::make_shared<IfElseStatement>(IfElseStatement(context::Context(this->fn, this->src, pos_start, body.result->ctx.end), std::static_pointer_cast<Expression>(condition.result), std::static_pointer_cast<BlockStatement>(body.result), nullptr)), errs);
        }

        this->advance();
        if (tt == If) {
            ParseResult chain = this->if_else_statement();
            for (const errors::Error &err : chain.errors) {
                errs.push_back(err);
            }

            return ParseResult(std::make_shared<IfElseStatement>(IfElseStatement(context::Context(this->fn, this->src, pos_start, chain.result->ctx.end), std::static_pointer_cast<Expression>(condition.result), std::static_pointer_cast<BlockStatement>(body.result), std::static_pointer_cast<IfElseStatement>(chain.result))), errs);
        }

        pr = this->expect(LCurl, "expected '{' to start a body");
        for (const errors::Error &err : pr.errors) {
            errs.push_back(err);
        }

        ParseResult else_body = this->inner_block();
        for (const errors::Error &err : else_body.errors) {
            errs.push_back(err);
        }

        return ParseResult(std::make_shared<IfElseStatement>(IfElseStatement(context::Context(this->fn, this->src, pos_start, else_body.result->ctx.end), std::static_pointer_cast<Expression>(condition.result), std::static_pointer_cast<BlockStatement>(body.result), std::make_shared<IfElseStatement>(else_body.result->ctx, std::make_shared<IdentifierExpression>(IdentifierExpression(context::Context(this->fn, this->src, tc.end, tc.end), "true")), std::static_pointer_cast<BlockStatement>(else_body.result), nullptr))), errs);
    }

    ParseResult Parser::while_statement() {
        context::Position pos_start = tc.start;
        this->advance();

        ParseResult condition = this->expression();
        std::vector<errors::Error> errs = {};
        for (const errors::Error &err : condition.errors) {
            errs.push_back(err);
        }

        ParseResult pr = this->expect(LCurl, "expected '{' to start a body");
        for (const errors::Error &err : pr.errors) {
            errs.push_back(err);
        }
        
        ParseResult body = this->inner_block();
        for (const errors::Error &err : body.errors) {
            errs.push_back(err);
        }

        return ParseResult(std::make_shared<WhileStatement>(WhileStatement(context::Context(this->fn, this->src, pos_start, tc.end), std::static_pointer_cast<Expression>(condition.result), std::static_pointer_cast<BlockStatement>(body.result))), errs);
    }

    ParseResult Parser::for_statement() {
        context::Position pos_start = tc.start;
        this->advance();

        ParseResult initialization = this->statement(true);
        std::vector<errors::Error> errs = {};
        for (const errors::Error &err : initialization.errors) {
            errs.push_back(err);
        }

        this->advance();
        ParseResult condition = this->expression();
        for (const errors::Error &err : condition.errors) {
            errs.push_back(err);
        }

        ParseResult pr = this->eol();
        for (const errors::Error &err : pr.errors) {
            errs.push_back(err);
        }

        this->advance();
        ParseResult increment = this->statement(false);
        for (const errors::Error &err : increment.errors) {
            errs.push_back(err);
        }

        pr = this->expect(LCurl, "expected '{' to start a body");
        for (const errors::Error &err : pr.errors) {
            errs.push_back(err);
        }

        ParseResult body = this->inner_block();
        for (const errors::Error &err : body.errors) {
            errs.push_back(err);
        }

        return ParseResult(std::make_shared<ForV1Statement>(ForV1Statement(context::Context(this->fn, this->src, pos_start, tc.end), initialization.result, std::static_pointer_cast<Expression>(condition.result), increment.result, std::static_pointer_cast<BlockStatement>(body.result))), errs);
    }

    ParseResult Parser::expression() {
        return this->assignment_expression();
    }

    ParseResult Parser::assignment_expression() {
        ParseResult pr = this->or_expression();
        if (tt != Equals && tt != PlusEquals && tt != MinusEquals && tt != MultEquals && tt != DivEquals && tt != ModEquals) {
            return pr;
        }

        lexer::Token equals = this->current_tok;
        this->advance();
        ParseResult pr2 = this->or_expression();

        context::Position start;
        context::Position end;
        if (pr.errors.empty()) {
            start = pr.result->ctx.start;
            if (!is_assignable(pr.result)) {
                pr.errors.push_back(errors::SyntaxError(pr.result->ctx, "invalid syntax!"));
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

        return ParseResult(std::make_shared<AssignmentExpression>(AssignmentExpression(context::Context(this->fn, this->src, start, end), std::static_pointer_cast<Expression>(pr.result), equals.value, std::static_pointer_cast<Expression>(pr2.result))), pr.errors);
    }

    ParseResult Parser::or_expression() {
        ParseResult lhs = this->and_expression();
        while (tt == Or) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult rhs = this->and_expression();
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

    ParseResult Parser::and_expression() {
        ParseResult lhs = this->comparative_expression();
        while (tt == And) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult rhs = this->comparative_expression();
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

    ParseResult Parser::comparative_expression() {
        ParseResult lhs = this->additive_expression();
        while (comparative) {
            lexer::Token op = this->current_tok;
            this->advance();

            ParseResult rhs = this->additive_expression();
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
        if (unary) {
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

        ParseResult pr = this->call_expression();
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

        if (tt == Increment) {
            lexer::Token op = this->current_tok;
            this->advance();
            context::Position start;
            if (pr.errors.empty()) {
                start = pr.result->ctx.start;
                if (!is_assignable(pr.result)) {
                    pr.errors.push_back(errors::SyntaxError(pr.result->ctx, "invalid syntax!"));
                }
            } else {
                start = op.ctx.start;
            }

            return ParseResult(std::make_shared<UnaryExpression>(UnaryExpression(context::Context(this->fn, this->src, start, op.ctx.end), "++ end", std::static_pointer_cast<Expression>(pr.result))), pr.errors);
        }

        if (tt == Decrement) {
            lexer::Token op = this->current_tok;
            this->advance();
            context::Position start;
            if (pr.errors.empty()) {
                start = pr.result->ctx.start;
                if (!is_assignable(pr.result)) {
                    pr.errors.push_back(errors::SyntaxError(pr.result->ctx, "invalid syntax!"));
                }
            } else {
                start = op.ctx.start;
            }

            return ParseResult(std::make_shared<UnaryExpression>(UnaryExpression(context::Context(this->fn, this->src, start, op.ctx.end), "-- end", std::static_pointer_cast<Expression>(pr.result))), pr.errors);
        }

        return pr;
    }

    ParseResult Parser::call_expression() {
        ParseResult callee = this->member_expression();
        if (tt != LParen) {
            return callee;
        }

        this->advance();
        std::vector<std::shared_ptr<Expression>> arguments = {};
        std::vector<errors::Error> errors = {};
        while (true) {
            if (tt == RParen) {
                break;
            }

            ParseResult arg = this->expression();
            arguments.push_back(std::static_pointer_cast<Expression>(arg.result));
            for (const errors::Error &err : arg.errors) {
                errors.push_back(err);
            }

            if (tt == RParen) {
                break;
            }

            ParseResult pr = this->expect(Comma, "expected ',' to seperate the arguments");
            for (const errors::Error &err : pr.errors) {
                errors.push_back(err);
            }

            this->advance();
        }

        lexer::Token rparen = this->current_tok;
        this->advance();
        return ParseResult(std::make_shared<CallExpression>(CallExpression(context::Context(this->fn, this->src, callee.result->ctx.start, rparen.ctx.end), std::static_pointer_cast<Expression>(callee.result), arguments)), errors);
    }

    ParseResult Parser::member_expression() {
        ParseResult parent = this->primary_expression();
        while (tt == Dot) {
            this->advance();
            ParseResult pr = this->expect(Ident, "expected variable name for attribute accessing");
            for (const errors::Error &err : pr.errors) {
                parent.errors.push_back(err);
            }

            if (pr.errors.empty()) {
                parent.result = std::make_shared<MemberExpression>(MemberExpression(context::Context(this->fn, this->src, parent.result->ctx.start, tc.end), std::static_pointer_cast<Expression>(parent.result), tv));
            } else {
                parent.result = std::make_shared<MemberExpression>(MemberExpression(parent.result->ctx, std::static_pointer_cast<Expression>(parent.result), ""));
            }

            this->advance();
        }

        return parent;
    }

    ParseResult Parser::primary_expression() {
        switch (tt) {
        case LParen: {
            lexer::Token lparen = this->current_tok;
            this->advance();
            ParseResult expr = this->expression();
            ParseResult pr = this->expect(RParen, "expected ')'");
            for (const errors::Error &err : pr.errors) {
                expr.errors.push_back(err);
            }

            if (!pr.errors.empty()) {
                return expr;
            }

            this->advance();
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
        case String: {
            std::pair<std::string, std::string> result = encode_string(tv);
            if (result.second != "") {
                return ParseResult(nullptr, {errors::SyntaxError(tc, result.second)});
            }

            ParseResult returned = ParseResult(std::make_shared<StringExpression>(StringExpression(tc, result.first)), {});
            this->advance();
            return returned;
        }
        case Ident:
            return this->list_type_expression();
        case Get: {
            return this->get_expression();
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

    ParseResult Parser::get_expression() {
        context::Position start = tc.start;
        this->advance();

        ParseResult pr = this->expect(String, "expected a string literal in get expressions");
        if (!pr.errors.empty()) {
            return pr;
        }

        context::Position end = tc.end;
        std::shared_ptr<GetExpression> returned = std::make_shared<GetExpression>(GetExpression(context::Context(this->fn, this->src, start, end), tv));
        this->advance();

        return ParseResult(returned, {});
    }
}