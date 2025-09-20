#pragma once

#include "../context/positions.hpp"
#include <string>
#include <vector>

namespace lexer {
    enum struct TokenType {
        EndOfFile,
        Int,
        Double,
        Plus,
        Minus,
        Mult,
        Div,
        LParen,
        RParen
    };

    struct Token {
        TokenType type;
        std::string value;
        context::Context context;
        
        Token(const context::Context context, const TokenType type, const std::string value);
    };
}