#include "lexer.hpp"
#include "../context/positions.hpp"
#include "../errors/error_handler.hpp"
#include <string>
#include <unordered_map>

#define ctx_ess this->file_name, this->code
#define lower_parse_levels ('0' <= this->peek() && this->peek() <= '9') || ('A' <= this->peek() && this->peek() <= 'Z') || ('a' <= this->peek() && this->peek() <= 'z') || this->peek() == '('
#define whitespaces !this->overflow() && (this->code[this->idx] == ' ' || this->code[this->idx] == '\t' || this->code[this->idx] == '\n')
#define number_dot this->past() == '(' || this->past() == '[' || this->past() == '{' || this->past() == ',' || this->past() == '+' || this->past() == '-' || this->past() == '*' || this->past() == '/'

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
            {'[', TokenType::LBrac},
            {']', TokenType::RBrac},
            {'{', TokenType::LCurl},
            {'}', TokenType::RCurl},
            {';', TokenType::Semi},
            {',', TokenType::Comma}
        };

        this->keywords = {
            {"const", TokenType::Const},
            {"get", TokenType::Get},
            {"if", TokenType::If},
            {"else", TokenType::Else}
        };

        this->idx = -1;
        this->pos = context::Position(0, 1);
        this->current_char = '\0';
        this->advance();
    }

    void Lexer::tokenize() {
        while (!this->overflow()) {
            while (whitespaces) {
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

            if (this->current_char == '%') {
                if (lower_parse_levels) {
                    this->tokens.push_back(Token(context::Context(ctx_ess, this->pos, context::Position(this->pos.col+1, this->pos.line)), TokenType::Mod, "%"));
                } else {
                    this->tokens.push_back(Token(context::Context(ctx_ess, this->pos, context::Position(this->pos.col+1, this->pos.line)), TokenType::Percent, "%"));
                }

                this->advance();
                continue;
            }

            if (this->current_char == '.') {
                if (number_dot) {
                    std::cout << this->past() << " ";
                    this->tokens.push_back(this->build_number());
                    continue;
                }

                this->tokens.push_back(Token(context::Context(ctx_ess, this->pos, context::Position(this->pos.col+1, this->pos.line)), TokenType::Dot, "."));
                this->advance();
                continue;
            }

            if (this->current_char == '=') {
                context::Position pos_start = this->pos.copy();
                this->advance();
                if (this->current_char == '=') {
                    this->tokens.push_back(Token(context::Context(ctx_ess, pos_start, context::Position(this->pos.col+1, this->pos.line)), TokenType::EqualComp, "=="));
                    this->advance();
                } else {
                    this->tokens.push_back(Token(context::Context(ctx_ess, pos_start, this->pos.copy()), TokenType::Equals, "="));
                }

                continue;
            }

            if (this->current_char == '!') {
                context::Position pos_start = this->pos.copy();
                this->advance();
                if (this->current_char == '=') {
                    this->tokens.push_back(Token(context::Context(ctx_ess, pos_start, context::Position(this->pos.col+1, this->pos.line)), TokenType::NotEquals, "!="));
                    this->advance();
                } else {
                    this->errors.push_back(errors::SyntaxError(context::Context(ctx_ess, pos_start, this->pos), "invalid character '!'"));
                }

                continue;
            }

            if (this->current_char == '>') {
                context::Position pos_start = this->pos.copy();
                this->advance();
                if (this->current_char == '=') {
                    this->tokens.push_back(Token(context::Context(ctx_ess, pos_start, context::Position(this->pos.col+1, this->pos.line)), TokenType::GreaterOrEquals, ">="));
                    this->advance();
                } else {
                    this->tokens.push_back(Token(context::Context(ctx_ess, pos_start, this->pos.copy()), TokenType::Greater, ">"));
                }

                continue;
            }

            if (this->current_char == '<') {
                context::Position pos_start = this->pos.copy();
                this->advance();
                if (this->current_char == '=') {
                    this->tokens.push_back(Token(context::Context(ctx_ess, pos_start, context::Position(this->pos.col+1, this->pos.line)), TokenType::SmallerOrEquals, "<="));
                    this->advance();
                } else {
                    this->tokens.push_back(Token(context::Context(ctx_ess, pos_start, this->pos.copy()), TokenType::Smaller, "<"));
                }

                continue;
            }

            if (this->current_char == '"') {
                this->tokens.push_back(this->build_string());
                continue;
            }

            if (('0' <= this->current_char && this->current_char <= '9')) {
                this->tokens.push_back(this->build_number());
                continue;
            }

            if (('A' <= this->current_char && this->current_char <= 'Z') || ('a' <= this->current_char && this->current_char <= 'z') || this->current_char == '_') {
                this->tokens.push_back(this->build_ident());
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

    char Lexer::peek() {
        size_t save = this->idx;
        this->idx++;
        while (whitespaces) {
            this->idx++;
        }

        if (this->overflow()) {
            this->idx = save;
            return '\0';
        }

        char returned = this->code[this->idx];
        this->idx = save;
        return returned;
    }

    char Lexer::past() {
        if (this->idx == 0) {
            return ' ';
        }

        return this->code[this->idx-1];
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
            this->errors.push_back(errors::SyntaxError(context::Context(ctx_ess, position_start, this->pos.copy()), "standalone decimal point '.'"));
            return Token(context::Context(this->file_name, this->code, position_start, this->pos.copy()), TokenType::EndOfFile, "");
        }

        return Token(context::Context(this->file_name, this->code, position_start, this->pos.copy()), dot ? TokenType::Double : TokenType::Int, number);
    }

    Token Lexer::build_ident() {
        std::string ident = "";
        context::Position position_start = this->pos.copy();
        while (!this->overflow() && (('A' <= this->current_char && this->current_char <= 'Z') || ('a' <= this->current_char && this->current_char <= 'z') || this->current_char == '_')) {
            ident += this->current_char;
            this->advance();
        }

        auto it = this->keywords.find(ident);
        if (it == this->keywords.end()) {
            return Token(context::Context(this->file_name, this->code, position_start, this->pos.copy()), TokenType::Ident, ident);
        }

        return Token(context::Context(this->file_name, this->code, position_start, this->pos.copy()), it->second, ident);
    }

    Token Lexer::build_string() {
        std::string string = "";
        context::Position position_start = this->pos.copy();
        this->advance();
        while (!this->overflow() && this->current_char != '"') {
            if (this->current_char == '\\') {
                string += "\\";
                this->advance();
                if (this->overflow()) {
                    break;
                }
            }

            if (this->current_char == '\n') {
                string += "\\n";
            } else {
                string += this->current_char;
            }
            
            this->advance();
        }

        if (this->overflow()) {
            this->errors.push_back(errors::SyntaxError(context::Context(ctx_ess, position_start, this->pos.copy()), "unclosed string"));
            return Token(context::Context(this->file_name, this->code, position_start, this->pos.copy()), TokenType::EndOfFile, "");
        }

        this->advance();
        return Token(context::Context(this->file_name, this->code, position_start, this->pos.copy()), TokenType::String, string);
    }
}