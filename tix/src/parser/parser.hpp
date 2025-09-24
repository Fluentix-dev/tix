#pragma once

#include "stmt.hpp"
#include "../lexer/tokens.hpp"
#include "../errors/error_handler.hpp"
#include <vector>
#include <memory>

namespace parser {
    struct ParseResult {
        std::shared_ptr<Statement> result;
        std::vector<errors::Error> errors;

        ParseResult(std::shared_ptr<Statement> result, const std::vector<errors::Error> errors);
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

        // Parser essentials
        ParseResult statement();
        ParseResult expression();
        ParseResult additive_expression();
        ParseResult multiplicative_expression();
        ParseResult unary_expression();
        ParseResult primary_expression();
    };

}