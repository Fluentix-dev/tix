#pragma once

#include "../../context/positions.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

namespace interpreter {
    struct RuntimeResult;
    struct RuntimeValue;

    struct Scope {
        Scope(const std::shared_ptr<Scope> parent);
        RuntimeResult declare(const context::Context ctx, const bool constant, const std::string data_type, const std::string var_name, const std::shared_ptr<RuntimeValue> value);
        RuntimeResult assign(const context::Context ctx, const std::string var_name, const std::shared_ptr<RuntimeValue> value);
        RuntimeResult get(const context::Context ctx, const std::string var_name);
    private:
        std::shared_ptr<Scope> parent;
        std::unordered_map<std::string, std::shared_ptr<RuntimeValue>> scope;
        std::unordered_map<std::string, std::string> scope_type;
        std::unordered_map<std::string, bool> constants;
    };
}