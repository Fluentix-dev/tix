#pragma once

#include "../../errors/error_handler.hpp"
#include "../../context/positions.hpp"
#include "scopes.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

namespace interpreter {
    struct RuntimeValue;
    struct RuntimeResult;

    struct RuntimeValue {
        std::string data_type;
        context::Context ctx;

        virtual RuntimeResult add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult subtract(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult multiply(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult unplus(const context::Context ctx);
        virtual RuntimeResult negate(const context::Context ctx);
        virtual RuntimeResult percent(const context::Context ctx);
        virtual RuntimeResult equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult not_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult greater_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult smaller_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult greater_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult smaller_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult repr(const context::Context ctx);
        virtual RuntimeResult call(const context::Context ctx, const std::vector<std::shared_ptr<RuntimeValue>> args);
        virtual RuntimeResult access(const context::Context ctx, const std::string attr);
    };

    struct RuntimeResult {
        std::shared_ptr<RuntimeValue> result;
        std::shared_ptr<errors::Error> error;

        RuntimeResult(const std::shared_ptr<RuntimeValue> result, const std::shared_ptr<errors::Error> error);
    };

    bool is(const std::shared_ptr<Scope> scope, const std::shared_ptr<RuntimeValue> value, const std::string data_type);

    struct Int : public RuntimeValue {
        long long value;
        Int(const context::Context ctx, const long long value);

        RuntimeResult add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult subtract(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult multiply(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;  
        RuntimeResult unplus(const context::Context ctx) override;
        RuntimeResult negate(const context::Context ctx) override;
        RuntimeResult percent(const context::Context ctx) override;
        RuntimeResult equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult not_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult greater_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult smaller_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult greater_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult smaller_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult repr(const context::Context ctx) override;
    };

    struct Double : public RuntimeValue {
        double value;
        Double(const context::Context ctx, const double value);

        RuntimeResult add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult subtract(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult multiply(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult unplus(const context::Context ctx) override;
        RuntimeResult negate(const context::Context ctx) override;
        RuntimeResult percent(const context::Context ctx) override;
        RuntimeResult equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult not_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult greater_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult smaller_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult greater_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult smaller_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult repr(const context::Context ctx) override;
    };

    struct Type : public RuntimeValue {
        std::string type;
        std::unordered_map<std::string, bool> inheritence;
        Type(const context::Context ctx, const std::string type, const std::unordered_map<std::string, bool> inheritence);
        
        RuntimeResult repr(const context::Context ctx) override;
    };

    struct Null : public RuntimeValue {
        Null(const context::Context ctx);

        // RuntimeResult equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        // RuntimeResult not_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult repr(const context::Context ctx) override;
    };

    struct String : public RuntimeValue {
        std::string value;
        String(const context::Context ctx, const std::string value);

        RuntimeResult equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult not_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult repr(const context::Context ctx) override;
    };

    struct BuiltInFunction : public RuntimeValue {
        std::function<RuntimeResult(std::vector<std::shared_ptr<RuntimeValue>>)> fn;
        BuiltInFunction(const context::Context ctx, const std::function<RuntimeResult(std::vector<std::shared_ptr<RuntimeValue>>)> fn);

        RuntimeResult call(const context::Context ctx, const std::vector<std::shared_ptr<RuntimeValue>> args) override;
    };

    struct Module : public RuntimeValue {
        std::unordered_map<std::string, std::shared_ptr<RuntimeValue>> vars;
        Module(const context::Context ctx, const std::unordered_map<std::string, std::shared_ptr<RuntimeValue>> vars);
        
        RuntimeResult access(const context::Context ctx, const std::string attr);
    };

    struct Boolean : public RuntimeValue {
        bool value;
        Boolean(const context::Context ctx, const bool value);

        RuntimeResult add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult subtract(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult multiply(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;  
        RuntimeResult unplus(const context::Context ctx) override;
        RuntimeResult negate(const context::Context ctx) override;
        RuntimeResult percent(const context::Context ctx) override;
        RuntimeResult equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult not_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult greater_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult smaller_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult greater_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult smaller_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult repr(const context::Context ctx) override;
    };
}