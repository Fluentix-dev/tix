#include "positions.hpp"
#include <iostream>

namespace context {
    Position::Position() {}
    Position::Position(const size_t col, const size_t line) {
        this->col = col;
        this->line = line;
    }

    Position Position::copy() {
        return Position(this->col, this->line);
    }

    Context::Context() {}
    Context::Context(const std::string file_name, const std::string code, const Position start, const Position end) {
        this->file_name = file_name;
        this->start = start;
        this->end = end;

        this->lines.clear();
        std::string line = "";
        for (size_t i = 0; i < code.size(); i++) {
            if (code[i] == '\n') {
                this->lines.push_back(line);
                line = "";
            } else {
                line += code[i];
            }
        }

        this->lines.push_back(line);
    }
}