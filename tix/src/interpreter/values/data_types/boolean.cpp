#include "../values.hpp"
#include "../../../context/positions.hpp"
#include "../../../errors/error_handler.hpp"
#include <memory>
#include <string>
#include <cmath>

namespace interpreter {
    Boolean::Boolean(const context::Context ctx, const bool value) {
        this->ctx = ctx;
        this->data_type = "boolean";
        this->value = value;
    }

    RuntimeResult Boolean::add(const context::Context, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, lhs_val + rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, lhs_val + rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, (rhs->value ? lhs_val+1 : lhs_val)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::subtract(const context::Context, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, lhs_val - rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, lhs_val - rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, (rhs->value ? lhs_val-1 : lhs_val)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::multiply(const context::Context, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, lhs_val * rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Double>(ctx, lhs_val * rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Int>(ctx, (rhs->value ? lhs_val : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, lhs_val / (double)rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, lhs_val / rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return (rhs->value ? RuntimeResult(std::make_shared<Int>(ctx, lhs_val), nullptr) : RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "division by 0"))));
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform division on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }
            
            return RuntimeResult(std::make_shared<Int>(ctx, lhs_val % rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            if (rhs->value == 0) {
                return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0")));
            }

            return RuntimeResult(std::make_shared<Double>(ctx, std::fmod(lhs_val, rhs->value)), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return (rhs->value ? RuntimeResult(std::make_shared<Int>(ctx, 0), nullptr) : RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "modulo by 0"))));
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform modulo on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::unplus(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Int>(ctx, (this->value ? 1 : 0)), nullptr);
    }

    RuntimeResult Boolean::negate(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Int>(ctx, (this->value ? -1 : 0)), nullptr);
    }

    RuntimeResult Boolean::percent(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Double>(ctx, (this->value ? 0.01 : 0.0)), nullptr);
    }

    RuntimeResult Boolean::equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val == rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val == rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val == (rhs->value ? 1 : 0)), nullptr);
        }
    }

    RuntimeResult Boolean::not_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val != rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val != rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val != (rhs->value ? 1 : 0)), nullptr);
        }
    }

    RuntimeResult Boolean::greater_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val > rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val > rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val > (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform greater than comparison on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::smaller_than(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val < rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val < rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val < (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform smaller than comparison on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::greater_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val >= rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val >= rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val >= (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform greater than or equals comparison on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::smaller_than_or_equals(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        long long lhs_val = (this->value ? 1 : 0);
        if (other->data_type == "int") {
            std::shared_ptr<Int> rhs = std::static_pointer_cast<Int>(other);            
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val <= rhs->value), nullptr);
        }

        if (other->data_type == "double") {
            std::shared_ptr<Double> rhs = std::static_pointer_cast<Double>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val <= rhs->value), nullptr);
        }

        if (other->data_type == "boolean") {
            std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
            return RuntimeResult(std::make_shared<Boolean>(ctx, lhs_val <= (rhs->value ? 1 : 0)), nullptr);
        }

        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform smaller than or equals comparison on 'boolean' and '" + other->data_type + "'")));
    }

    RuntimeResult Boolean::and_(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (!this->value) {
            return RuntimeResult(std::make_shared<Boolean>(ctx, false), nullptr);
        }

        if (other->data_type != "boolean") {
            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(ctx, "cannot perform and operation on 'boolean' and " + other->data_type + "'"));
        }

        std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
        return RuntimeResult(std::make_shared<Boolean>(ctx, rhs->value), nullptr);
    }

    RuntimeResult Boolean::or_(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (this->value) {
            return RuntimeResult(std::make_shared<Boolean>(ctx, true), nullptr);
        }

        if (other->data_type != "boolean") {
            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(ctx, "cannot perform and operation on 'boolean' and " + other->data_type + "'"));
        }

        std::shared_ptr<Boolean> rhs = std::static_pointer_cast<Boolean>(other);
        return RuntimeResult(std::make_shared<Boolean>(ctx, rhs->value), nullptr);
    }

    RuntimeResult Boolean::not_(const context::Context ctx) {
        return RuntimeResult(std::make_shared<Boolean>(ctx, !this->value), nullptr);
    }

    RuntimeResult Boolean::repr(const context::Context ctx) {
        return RuntimeResult(std::make_shared<String>(ctx, (this->value ? "true" : "false")), nullptr);
    }
}