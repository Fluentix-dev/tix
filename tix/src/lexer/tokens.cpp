#include "tokens.hpp"
#include "../context/positions.hpp"
#include <string>

namespace lexer {
    Token::Token(const context::Context context, TokenType type, const std::string value) {
        this->type = type;
        this->value = value;
    }
};