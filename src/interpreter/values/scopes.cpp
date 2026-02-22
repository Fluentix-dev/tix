#include "scopes.hpp"
#include "values.hpp"
#include "../../context/positions.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace interpreter {
    Scope::Scope(const std::shared_ptr<Scope> parent) {
        this->parent = parent;
        this->scope.clear();
        this->constants.clear();
        this->scope_type.clear();
        if (parent == nullptr) {
            // Predefine type so other values can rely on this
            this->scope["type"] = std::make_shared<interpreter::Type>(interpreter::Type(context::Context("", "", context::Position(0, 0), context::Position(0, 0)), "type", {}));
            this->constants["type"] = true;
            this->scope_type["type"] = "type";
        }
    }

    RuntimeResult Scope::declare(const context::Context& ctx, const bool constant, const std::string& data_type, const std::string& var_name, const std::shared_ptr<RuntimeValue> value) {
        if (this->scope.find(var_name) != this->scope.end()) {
            return RuntimeResult(nullptr, std::make_shared<errors::ScopeError>(errors::ScopeError(ctx, "variable '" + var_name + "' is already declared")));
        }

        if (!is(this, value, data_type)) {
            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "assignee does not have the data type of '" + data_type + "' that the variable declaration with variable '" + var_name + "' expected")));
        }

        this->scope_type[var_name] = data_type;
        this->scope[var_name] = value;
        this->constants[var_name] = constant;

        return RuntimeResult(nullptr, nullptr);
    }

    RuntimeResult Scope::assign(const context::Context& ctx, const std::string& var_name, const std::shared_ptr<RuntimeValue> value) {
        auto it = this->scope.find(var_name);
        if (it != this->scope.end()) {
            if (this->constants[var_name]) {
                return RuntimeResult(nullptr, std::make_shared<errors::ScopeError>(errors::ScopeError(ctx, "cannot assign to constant variable '" + var_name + "'")));
            }

            if (!is(this, value, this->scope_type[var_name])) {
                return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "assignee does not have the data type of '" + this->scope_type[var_name] + "' that the variable assignment with variable '" + var_name + "' expected")));
            }

            this->scope[var_name] = value;
            this->scope[var_name]->ctx = ctx;
            return RuntimeResult(this->scope[var_name], {});
        }

        if (this->parent == nullptr) {
            return RuntimeResult(nullptr, std::make_shared<errors::ScopeError>(errors::ScopeError(ctx, "cannot assign to variable '" + var_name + "' that is not declared yet")));
        }

        return this->parent->assign(ctx, var_name, value);
    }

    RuntimeResult Scope::get(const context::Context& ctx, const std::string& var_name) const {
        auto it = this->scope.find(var_name);
        if (it != this->scope.end()) {
            std::shared_ptr<RuntimeValue> returned = it->second;
            returned->ctx = ctx;

            return RuntimeResult(returned, nullptr);
        }

        if (this->parent == nullptr) {
            return RuntimeResult(nullptr, std::make_shared<errors::ScopeError>(errors::ScopeError(ctx, "variable '" + var_name + "' does not exist in program")));
        }

        return this->parent->get(ctx, var_name);
    }

    void Scope::clear() {
        this->scope.clear();
        this->scope_type.clear();
        this->constants.clear();
    }
}