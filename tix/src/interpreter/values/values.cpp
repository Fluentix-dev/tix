#include "values.hpp"
#include "../../errors/error_handler.hpp"
#include "../../context/positions.hpp"
#include <memory>

namespace interpreter {
    RuntimeResult RuntimeValue::add(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "Cannot perform addition on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::subtract(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "Cannot perform subtraction on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::multiply(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "Cannot perform multiplication on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult RuntimeValue::divide(const std::shared_ptr<RuntimeValue> other) {
        context::Context new_ctx = this->ctx;
        new_ctx.end = other->ctx.end;
        return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(new_ctx, "Cannot perform division on '" + this->data_type + "' and '" + other->data_type + "'")));
    }

    RuntimeResult::RuntimeResult(const std::shared_ptr<RuntimeValue> result, const std::shared_ptr<errors::Error> error) {
        this->result = result;
        this->error = error;
    }
}