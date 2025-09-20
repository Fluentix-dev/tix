#include "lexer/lexer.hpp"
#include "errors/error_handler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

int main() {
    std::ifstream src("files/main.tx");
    if (!src) {
        std::cout << "File main.tx cannot be found!\n";
        return 0;
    }
    std::stringstream code;
    std::string line;
    while (src.is_open() && std::getline(src, line)) {
        code << line << "\n";
    }

    std::string real = code.str();

    lexer::Lexer lex = lexer::Lexer("main.tx", real);
    lex.tokenize();

    if (lex.errors.empty()) {
        for (const lexer::Token &token : lex.tokens) {
            std::cout << token.value << "\n";
        }
    } else {
        std::cout << "Total errors: " << lex.errors.size() << "\n";
        for (const errors::Error &err : lex.errors) {
            errors::print_error(err);
            std::cout << "\n";
        }
    }

    return 0;
}