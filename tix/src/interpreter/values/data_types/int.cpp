#include "../values.hpp"
#include "../../../context/positions.hpp"
#include "../../../errors/error_handler.hpp"
#include <memory>

namespace interpreter {
    Int::Int(const context::Context ctx, const long long value) {
        this->data_type = "int";
        this->ctx = ctx;
        this->value = value;
    }

    RuntimeResult Int::add(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(new_ctx, this->value + rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(new_ctx, this->value + rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "cannot perform addition on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::subtract(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(new_ctx, this->value - rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(new_ctx, this->value - rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "cannot perform subtraction on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::multiply(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(new_ctx, this->value * rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(new_ctx, this->value * rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "cannot perform multiplication on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::divide(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(new_ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Int>(new_ctx, this->value / rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(new_ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(new_ctx, this->value / rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "cannot perform subtraction on 'int' and '" + other->data_type + "'")));
    }
}