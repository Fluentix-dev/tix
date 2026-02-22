#pragma once

#include "stmt.hpp"
#include "../lexer/tokens.hpp"
#include "../errors/error_handler.hpp"
#include <vector>
#include <memory>
#include <string>

namespace parser {
    struct ParseResult {
        std::shared_ptr<Statement> result;
        std::vector<errors::Error> errors;

        ParseResult(const std::shared_ptr<Statement> result, const std::vector<errors::Error> errors);
    };

    struct Parser {
        std::shared_ptr<BlockStatement> block;
        std::string fn;
        std::string src;
        std::vector<errors::Error> errors;

        Parser(const std::string fn, const std::string src, const std::vector<lexer::Token> tokens);
        void parse();
    private:
        std::vector<lexer::Token> tokens;
        size_t idx;
        lexer::Token current_tok;

        void advance();
        bool overflow();
        ParseResult eol();
        ParseResult expect(const lexer::TokenType expected, const std::string error_msg);

        // Parser essentials
        ParseResult statement(const bool eol);
        ParseResult expression();

        ParseResult inner_block();

        ParseResult variable_declaration_statement(ParseResult data_type);
        ParseResult if_else_statement();
        ParseResult while_statement();
        ParseResult for_statement();

        ParseResult assignment_expression();
        ParseResult or_expression();
        ParseResult and_expression();
        ParseResult comparative_expression();
        ParseResult additive_expression();
        ParseResult multiplicative_expression();
        ParseResult unary_expression();
        ParseResult call_expression();
        ParseResult member_expression();
        ParseResult primary_expression();
        ParseResult list_type_expression();
        ParseResult get_expression();
    };
}