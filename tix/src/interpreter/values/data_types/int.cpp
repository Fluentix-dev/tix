#include "../values.hpp"
#include "../../../context/positions.hpp"
#include "../../../errors/error_handler.hpp"
#include <memory>
#include <cmath>

namespace interpreter {
    Int::Int(const context::Context ctx, const long long value) {
        this->ctx = ctx;
        this->data_type = "int";
        this->value = value;
    }

    RuntimeResult Int::add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, this->value + rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value + rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::subtract(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, this->value - rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value - rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform subtraction on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::multiply(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, this->value * rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value * rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform multiplication on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, this->value / (double)rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, this->value / rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform division on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }
            
            return RuntimeResult(std::make_shared<Int>(ctx, this->value % rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, std::fmod(this->value, rhs->value)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform modulo on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::unplus(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Int>(ctx, this->value), nullptr);
    }

    RuntimeResult Int::negate(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Int>(ctx, -this->value), nullptr);
    }

    RuntimeResult Int::percent(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, this->value / 100.0), nullptr);
    }
}