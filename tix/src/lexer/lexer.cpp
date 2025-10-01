#include "lexer.hpp"
#include "../context/positions.hpp"
#include "../errors/error_handler.hpp"
#include <string>
#include <unordered_map>

#define ctx_ess this->file_name, this->code 

namespace lexer {
    Lexer::Lexer(const std::string file_name, const std::string code) {
        this->file_name = file_name;
        this->code = code;
        this->tokens.clear();
        this->errors.clear();
        this->single_char_token = {
            {'+', TokenType::Plus},
            {'-', TokenType::Minus},
            {'*', TokenType::Mult},
            {'/', TokenType::Div},
            {'(', TokenType::LParen},
            {')', TokenType::RParen},
            {'%', TokenType::Mod},
            {';', TokenType::Semi}
        };

        this->idx = -1;
        this->pos = context::Position(0, 1);
        this->current_char = '\0';
        this->advance();
    }

    void Lexer::tokenize() {
        while (!this->overflow()) {
            while (!this->overflow() && (this->current_char == ' ' || this->current_char == '\t' || this->current_char == '\n')) {
                this->advance();
            }

            if (this->overflow()) {
                break;
            }

            if (this->current_char == '#') {
                while (!this->overflow() && this->current_char != '\n') {
                    this->advance();
                }

                continue;
            }

            if (('0' <= this->current_char && this->current_char <= '9') || this->current_char == '.') {
                this->tokens.push_back(this->build_number());
                continue;
            }

            auto it = this->single_char_token.find(this->current_char);
            if (it == this->single_char_token.end()) {
                this->errors.push_back(errors::SyntaxError(context::Context(ctx_ess, this->pos, context::Position(this->pos.col+1, this->pos.line)), "invalid character '" + std::string(1, this->current_char) + "'"));
                this->advance();
                continue;
            }

            this->tokens.push_back(Token(context::Context(ctx_ess, this->pos, context::Position(this->pos.col+1, this->pos.line)), it->second, "" + std::string(1, this->current_char)));
            this->advance();
        }

        context::Position position_start = this->pos.copy();
        this->advance();
        this->tokens.push_back(Token(context::Context(ctx_ess, position_start, this->pos), TokenType::EndOfFile, "EOF"));
    }

    void Lexer::advance() {
        this->idx++;
        if (this->current_char == '\n') {
            this->pos.line++;
            this->pos.col = 1;
        } else {
            this->pos.col++;
        }

        if (!this->overflow()) {
            this->current_char = this->code[this->idx];
        } else {
            this->current_char = '\0';
        }
    }

    bool Lexer::overflow() {
        return (this->idx >= this->code.size());
    }

    Token Lexer::build_number() {
        std::string number = "";
        bool dot = false;

        context::Position position_start = this->pos.copy();
        while (!this->overflow() && (('0' <= this->current_char && this->current_char <= '9') || this->current_char == '.')) {
            if (this->current_char == '.') {
                if (dot) {
                    break;
                }

                dot = true;
            }

            number += this->current_char;
            this->advance();
        }

        if (number == ".") {
            this->errors.push_back(errors::SyntaxError(context::Context(ctx_ess, position_start, this->pos), "standalone decimal point '.'"));
            return Token(context::Context(this->file_name, this->code, position_start, this->pos), TokenType::EndOfFile, "");
        }

        return Token(context::Context(this->file_name, this->code, position_start, this->pos), dot ? TokenType::Double : TokenType::Int, number);
    }
}