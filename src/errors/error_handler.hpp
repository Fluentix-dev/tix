#pragma once

#include "../context/positions.hpp"
#include <string>

namespace errors {
    struct Error {
        std::string error_type;
        std::string details;
        context::Context context;
    };

    struct SyntaxError : public Error {
        SyntaxError(const context::Context context, const std::string& details);
    };

    struct TypeError : public Error {
        TypeError(const context::Context context, const std::string& details);
    };

    struct MathError : public Error {
        MathError(const context::Context context, const std::string& details);
    };

    struct InterpreterError : public Error {
        InterpreterError(const context::Context context, const std::string& details);
    };

    struct ScopeError : public Error {
        ScopeError(const context::Context context, const std::string& details);
    };

    struct AttributeError : public Error {
        AttributeError(const context::Context context, const std::string& details);
    };

    struct ModuleError : public Error {
        ModuleError(const context::Context context, const std::string& details);
    };

    struct ArgumentError : public Error {
        ArgumentError(const context::Context context, const std::string& details);
    };

    struct LoopError : public Error {
        LoopError(const context::Context context, const std::string& details);
    };

    void print_error(const Error error);
}