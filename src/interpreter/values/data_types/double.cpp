#include "../values.hpp"
#include "../../../context/positions.hpp"
#include "../../../errors/error_handler.hpp"
#include <memory>
#include <cmath>
#include <string>

namespace interpreter {
    Double::Double(const context::Context& ctx, const double value) {
        this->data_type = "double";
        this->ctx = ctx;
        this->value = value;
    }

    RuntimeResult Double::add(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value + rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value + rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, (rhs->value ? this->value+1 : this->value)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::subtract(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value - rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value - rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, (rhs->value ? this->value-1 : this->value)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform subtraction on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::multiply(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::dynamic_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value * rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::dynamic_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value * rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, (rhs->value ? this->value : 0.0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform multiplication on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::divide(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return (rhs->value ? RuntimeResult(std::make_shared<Double>(ctx, this->value), nullptr) : RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0"))));
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform division on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::mod(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return (rhs->value ? RuntimeResult(std::make_shared<Int>(ctx, std::fmod(this->value, 1.0)), nullptr) : RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0"))));
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform modulo on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::unplus(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, this->value), nullptr);
    }

    RuntimeResult Double::negate(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, -this->value), nullptr);
    }

    RuntimeResult Double::percent(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, this->value / 100), nullptr);
    }

    RuntimeResult Double::equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value == rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value == rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value == (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform equals comparison on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::not_equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value != rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value != rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value != (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform not equals comparison on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::greater_than(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value > rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value > rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value > (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform greater than comparison on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::smaller_than(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value < rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value < rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value < (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform smaller than comparison on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::greater_than_or_equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value >= rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value >= rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value >= (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform greater than or equals comparison on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::smaller_than_or_equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value <= rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value <= rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, this->value <= (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform smaller than or equals comparison on 'double' and '" + other->data_type + "'")));
    }

    RuntimeResult Double::increment(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Double>(Double(ctx, this->value+1)), nullptr);
    }

    RuntimeResult Double::decrement(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Double>(Double(ctx, this->value-1)), nullptr);
    }

    RuntimeResult Double::repr(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<String>(String(ctx, std::to_string(value))), nullptr);
    }
}