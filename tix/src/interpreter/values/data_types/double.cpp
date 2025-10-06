#include "../values.hpp"
#include "../../../context/positions.hpp"
#include "../../../errors/error_handler.hpp"
#include <memory>
#include <cmath>

namespace interpreter {
    Double::Double(const context::Context ctx, const double value) {
        this->data_type = "double";
        this->ctx = ctx;
        this->value = value;
    }

    RuntimeResult Double::add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value + rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value + rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::subtract(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value - rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value - rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform subtraction on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::multiply(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value * rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value * rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform multiplication on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, this->value / rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, this->value / rhs->value), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform division on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, std::fmod(this->value, rhs->value)), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, std::fmod(this->value, rhs->value)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform modulo on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::unplus(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, this->value), nullptr);
    }

    RuntimeResult Double::negate(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, -this->value), nullptr);
    }

    RuntimeResult Double::percent(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, this->value / 100), nullptr);
    }
}