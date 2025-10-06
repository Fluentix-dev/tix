#pragma once

#include "../../errors/error_handler.hpp"
#include "../../context/positions.hpp"
#include "scopes.hpp"
#include <string>
#include <memory>
#include <unordered_map>

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
    };

    struct Type : public RuntimeValue {
        std::string type;
        std::unordered_map<std::string, bool> inheritence;

        Type(const context::Context ctx, const std::string type, const std::unordered_map<std::string, bool> inheritence);
    };
}