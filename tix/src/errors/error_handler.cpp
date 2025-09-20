#include "error_handler.hpp"
#include <cmath>

#define start_line error.context.start.line
#define end_line error.context.end.line

#define start_col error.context.start.col
#define end_col error.context.end.col

#define lines error.context.lines
#define file error.context.file_name

#define type error.error_type
#define det error.details

namespace errors {
    SyntaxError::SyntaxError(const context::Context context, const std::string details) {
        this->error_type = "Syntax Error";
        this->details = details;
        this->context = context;
    }

    void print_error(const Error error) {
        if (start_line == end_line) {
            std::cout << start_line << " || " << lines[start_line-1] << "\n";
            size_t digits = std::log(start_line) / std::log(10) + 1;

            for (size_t i = 0; i < digits + 3 + start_col; i++) {
                std::cout << " ";
            }

            for (size_t i = start_col; i < end_col; i++) {
                std::cout << "^";
            }

            std::cout << "\nFile " << file << ", line " << start_line << "\n" << type << ": " << det << "\n";
            return;
        }

        size_t max_digits = std::log(end_line) / std::log(10) + 1;
        for (size_t i = start_line; i <= end_line; i++) {
            size_t digits = std::log(i) / std::log(10) + 1;

            for (size_t j = digits; j < max_digits; j++) {
                std::cout << "0";
            }

            std::cout << i << " || " << lines[i-1] << "\n";
            
            for (size_t j = 0; j < max_digits + 4; j++) {
                std::cout << " ";
            }

            if (i == start_line) {
                for (size_t j = 1; j < start_col; j++) {
                    std::cout << " ";
                }

                for (size_t j = start_col; j <= lines[i].size(); j++) {
                    std::cout << "^";
                }

                std::cout << "\n";
                continue;
            }

            if (i == end_line) {
                for (size_t j = 1; j <= end_col; j++) {
                    std::cout << "^";
                }

                std::cout << "\n";
                break;
            }

            for (size_t j = 0; j < lines[i].size(); j++) {
                std::cout << "^";
            }

            std::cout << "\n";
        }

        std::cout << "File " << file << ", line " << start_line << "\n" << type << ": " << det << "\n";
    }
}