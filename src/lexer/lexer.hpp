#pragma once

#include "tokens.hpp"
#include "../context/positions.hpp"
#include "../errors/error_handler.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace lexer {
    struct Lexer {
        std::vector<Token> tokens;
        std::vector<errors::Error> errors;

        Lexer(const std::string& file_name, const std::string& code);
        void tokenize();
    private:
        std::string file_name;
        std::string code;

        std::unordered_map<char, TokenType> single_char_token;
        std::unordered_map<std::string, TokenType> keywords;
        size_t idx;
        context::Position pos;
        char current_char;

        void advance();
        bool overflow();
        char peek();
        char past();

        Token build_number();
        Token build_ident();
        Token build_string();
    };
}