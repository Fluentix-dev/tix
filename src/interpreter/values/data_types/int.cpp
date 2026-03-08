#include "../values.hpp"
#include "../../../context/positions.hpp"
#include "../../../errors/error_handler.hpp"
#include <memory>
#include <cmath>
#include <string>

namespace interpreter {
    Int::Int(const context::Context& ctx, const long long value) {
        this->ctx = ctx;
        this->data_type = "int";
        this->value = value;
    }

    RuntimeResult Int::add(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, this->value + rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value + rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, (rhs->value ? this->value+1 : this->value)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::subtract(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, this->value - rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value - rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, (rhs->value ? this->value-1 : this->value)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform subtraction on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::multiply(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, this->value * rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, this->value * rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, (rhs->value ? this->value : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform multiplication on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::divide(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, this->value / (double)rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, this->value / rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return (rhs->value ? RuntimeResult(std::make_shared<Int>(ctx, this->value), nullptr) : RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0"))));
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform division on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::mod(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }
            
            return RuntimeResult(std::make_shared<Int>(ctx, this->value % rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, std::fmod(this->value, rhs->value)), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return (rhs->value ? RuntimeResult(std::make_shared<Int>(ctx, 0), nullptr) : RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0"))));
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform modulo on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::unplus(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Int>(ctx, this->value), nullptr);
    }

    RuntimeResult Int::negate(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Int>(ctx, -this->value), nullptr);
    }

    RuntimeResult Int::percent(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, this->value / 100.0), nullptr);
    }

    RuntimeResult Int::equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform equals comparison on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::not_equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform not equals comparison on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::greater_than(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform greater than comparison on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::smaller_than(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform smaller than comparison on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::greater_than_or_equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform equal greater than or equals on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::smaller_than_or_equals(const context::Context& ctx, const std::shared_ptr<RuntimeValue> other) {
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

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform smaller than or equals comparison on 'int' and '" + other->data_type + "'")));
    }

    RuntimeResult Int::increment(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Int>(Int(ctx, this->value+1)), nullptr);
    }

    RuntimeResult Int::decrement(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<Int>(Int(ctx, this->value-1)), nullptr);
    }

    RuntimeResult Int::repr(const context::Context& ctx) {
        return RuntimeResult(std::make_shared<String>(String(ctx, std::to_string(value))), nullptr);
    }
}