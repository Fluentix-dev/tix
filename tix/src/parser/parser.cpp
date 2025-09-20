#include "parser.hpp"
#include "../lexer/lexer.hpp"
#include <vector>

namespace parser {
    Parser::Parser(std::vector<lexer::Token> tokens) {
        this->tokens = tokens;
        this->block = new BlockStatement(this->tokens[0].context);
    }
}