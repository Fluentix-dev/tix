#include "tokens.hpp"
#include "../context/positions.hpp"
#include <string>

namespace lexer {
    Token::Token() {}
    Token::Token(const context::Context ctx, TokenType type, const std::string value) {
        this->type = type;
        this->ctx = ctx;
        this->value = value;
    }
};