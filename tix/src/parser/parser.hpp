#pragma once

#include "stmt.hpp"
#include "../lexer/tokens.hpp"
#include <vector>

namespace parser {
    struct Parser {
        BlockStatement* block;
        
        Parser(std::vector<lexer::Token> tokens);
    private:
        std::vector<lexer::Token> tokens;
    };
}