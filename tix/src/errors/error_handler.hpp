#pragma once

#include "../context/positions.hpp"
#include <string>
#include <iostream>

namespace errors {
    struct Error {
        std::string error_type;
        std::string details;
        context::Context context;
    };

    struct SyntaxError : public Error {
        SyntaxError(const context::Context context, const std::string details);
    };

    void print_error(const Error error);
}