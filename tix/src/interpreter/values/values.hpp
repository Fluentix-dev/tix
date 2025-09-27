#pragma once

#include "../../errors/error_handler.hpp"
#include "../../context/positions.hpp"
#include <string>
#include <memory>

namespace interpreter {
    struct RuntimeValue;
    struct RuntimeResult;

    struct RuntimeValue {
        std::string data_type;
        context::Context ctx;

        virtual RuntimeResult add(const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult subtract(const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult multiply(const std::shared_ptr<RuntimeValue> other);
        virtual RuntimeResult divide(const std::shared_ptr<RuntimeValue> other);
    };

    struct RuntimeResult {
        std::shared_ptr<RuntimeValue> result;
        std::shared_ptr<errors::Error> error;

        RuntimeResult(const std::shared_ptr<RuntimeValue> result, const std::shared_ptr<errors::Error> error);
    };

    struct Int : public RuntimeValue {
        long long value;
        Int(const context::Context ctx, const long long value);

        RuntimeResult add(const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult subtract(const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult multiply(const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult divide(const std::shared_ptr<RuntimeValue> other) override;
    };

    struct Double : public RuntimeValue {
        double value;
        Double(const context::Context ctx, const double value);

        RuntimeResult add(const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult subtract(const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult multiply(const std::shared_ptr<RuntimeValue> other) override;
        RuntimeResult divide(const std::shared_ptr<RuntimeValue> other) override;
    };
}