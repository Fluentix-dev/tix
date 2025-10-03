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
        Mod,
        Percent,
        LParen,
        RParen,
        Semi
    };

    struct Token {
        TokenType type;
        std::string value;
        context::Context ctx;
        
        Token();
        Token(const context::Context ctx, const TokenType type, const std::string value);
    };
}