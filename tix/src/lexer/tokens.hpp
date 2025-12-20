#pragma once

#include "../context/positions.hpp"
#include <string>
#include <vector>

namespace lexer {
    enum struct TokenType {
        EndOfFile,
        Int,
        Double,
        String,
        Plus,
        Minus,
        Mult,
        Div,
        Mod,
        Percent,
        LParen,
        RParen,
        LBrac,
        RBrac,
        LCurl,
        RCurl,
        Equals,
        Semi,
        Comma,
        Dot,
        EqualComp,
        NotEquals,
        Greater,
        Smaller,
        GreaterOrEquals,
        SmallerOrEquals,

        Ident,
        Const,
        Get,
        If,
        Else
    };

    struct Token {
        TokenType type;
        std::string value;
        context::Context ctx;
        
        Token();
        Token(const context::Context ctx, const TokenType type, const std::string value);
    };
}