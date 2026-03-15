#include "error_handler.hpp"
#include <cmath>
#include <iostream>

#define start_line error.context.start.line
#define end_line error.context.end.line

#define start_col error.context.start.col
#define end_col error.context.end.col

#define lines error.context.lines
#define file error.context.file_name

#define type error.error_type
#define det error.details

namespace errors {
    SyntaxError::SyntaxError(const context::Context context, const std::string& details) {
        this->error_type = "Syntax Error";
        this->details = details;
        this->context = context;
    }

    TypeError::TypeError(const context::Context context, const std::string& details) {
        this->error_type = "Type Error";
        this->details = details;
        this->context = context;
    }

    MathError::MathError(const context::Context context, const std::string& details) {
        this->error_type = "Math Error";
        this->details = details;
        this->context = context;
    }

    InterpreterError::InterpreterError(const context::Context context, const std::string& details) {
        this->error_type = "Interpreter Error";
        this->details = details;
        this->context = context;
    }

    ScopeError::ScopeError(const context::Context context, const std::string& details) {
        this->error_type = "Scope Error";
        this->details = details;
        this->context = context;
    }

    AttributeError::AttributeError(const context::Context context, const std::string& details) {
        this->error_type = "Attribute Error";
        this->details = details;
        this->context = context;
    }

    ModuleError::ModuleError(const context::Context context, const std::string& details) {
        this->error_type = "Module Error";
        this->details = details;
        this->context = context;
    }

    ArgumentError::ArgumentError(const context::Context context, const std::string& details) {
        this->error_type = "Argument Error";
        this->details = details;
        this->context = context;
    }
    
    LoopError::LoopError(const context::Context context, const std::string& details) {
        this->error_type = "Loop Error";
        this->details = details;
        this->context = context;
    }

    OverflowError::OverflowError(const context::Context context, const std::string& details) {
        this->error_type = "Overflow Error";
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
            // step 1: print the line no.
            size_t digits = std::log(i) / std::log(10) + 1;
            for (size_t i = 0; i < max_digits-digits; i++) {
                std::cout << 0;
            }

            std::cout << i << " || " << lines[i-1] << "\n";
            
            // step 2: print the offset
            for (size_t i = 0; i < max_digits + 4; i++) {
                std::cout << " ";
            }

            // step 3: print the pointer
            if (i == start_line) {
                // print from start pos to end line
                for (size_t j = 1; j < start_col; j++) {
                    std::cout << " ";
                }

                for (size_t j = start_col; j <= lines[i-1].size(); j++) {
                    std::cout << "^";
                }
            } else if (i == end_line) {
                // print from start line to end pos
                for (size_t j = 1; j < end_col; j++) {
                    std::cout << "^";
                }
            } else {
                // print the whole line
                for (const char c : lines[i-1]) {
                    std::cout << "^";
                }
            }

            // step 4: end it with a newline
            std::cout << "\n";
        }

        std::cout << "File " << file << ", line " << start_line << "\n" << type << ": " << det << "\n";
    }
}