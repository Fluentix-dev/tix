#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace context {
    struct Position {
        size_t col;
        size_t line;

        Position();
        Position(const size_t col, const size_t line);
        Position copy();
    };

    struct Context {
        std::string file_name;
        std::vector<std::string> lines;
        Position start;
        Position end;

        Context();
        Context(const std::string file_name, const std::string code, const Position start, Position end);
    };
}