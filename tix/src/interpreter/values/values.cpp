#include "values.hpp"
#include "../../errors/error_handler.hpp"
#include "../../context/positions.hpp"
#include "scopes.hpp"
#include <memory>
#include <vector>

namespace interpreter {
    RuntimeResult RuntimeValue::add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform addition on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::subtract(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform subtraction on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::multiply(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform multiplication on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::divide(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform division on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::mod(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform modulo on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::unplus(const context::Context ctx) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform unary plus on '" + this->data_type + "'")));
    }

    RuntimeResult RuntimeValue::negate(const context::Context ctx) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform negation on '" + this->data_type + "'")));
    }

    RuntimeResult RuntimeValue::percent(const context::Context ctx) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform percentage on '" + this->data_type + "'")));
    }

    RuntimeResult RuntimeValue::repr(const context::Context ctx) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform repr on '" + this->data_type + "'")));
    }

    RuntimeResult RuntimeValue::call(const context::Context ctx, const std::vector<std::shared_ptr<RuntimeValue>> args) {
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "Cannot perform call on '" + this->data_type + "'")));
    }

    RuntimeResult RuntimeValue::access(const context::Context ctx, const std::string attr) {
        return RuntimeResult(nullptr, std::make_shared<errors::AttributeError>(errors::AttributeError(ctx, "'" + this->data_type + "' has no attribute '" + attr + "'")));
    }

    RuntimeResult::RuntimeResult(const std::shared_ptr<RuntimeValue> result, const std::shared_ptr<errors::Error> error) {
        this->result = result;
        this->error = error;
    }

    bool is(const std::shared_ptr<Scope> scope, const std::shared_ptr<RuntimeValue> value, const std::string data_type) {
        RuntimeResult rr = scope->get(value->ctx, data_type);
        if (rr.error != nullptr) {
            return false;
        }

        if (rr.result->data_type != "type") {
            return false;
        }

        rr = scope->get(value->ctx, value->data_type);
        if (rr.error != nullptr) {
            return false;
        }

        if (rr.result->data_type != "type") {
            return false;
        }

        std::shared_ptr<Type> type = std::static_pointer_cast<Type>(rr.result);
        return type->type == data_type || type->inheritence[data_type];
    }
}