#include "lexer/lexer.hpp"
#include "errors/error_handler.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>

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
    real.erase(std::prev(real.end())); // Remove the ending newline

    lexer::Lexer lex = lexer::Lexer("main.tx", real);
    lex.tokenize();

    if (!lex.errors.empty()) {
        std::cout << "Total errors: " << lex.errors.size() << "\n";
        for (const errors::Error &err : lex.errors) {
            errors::print_error(err);
            std::cout << "\n";
        }

        return 0;
    }

    parser::Parser parse = parser::Parser(lex.tokens);
    parse.parse();

    if (!parse.errors.empty()) {
        std::cout << "Total errors: " << parse.errors.size() << "\n";
        for (errors::Error &err : parse.errors) {
            errors::print_error(err);
            std::cout << "\n";
        }

        return 0;
    }

    return 0;
}