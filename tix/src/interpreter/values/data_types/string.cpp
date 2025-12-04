#include "../values.hpp"
#include "../../../context/positions.hpp"
#include <string>

namespace interpreter {
    String::String(const context::Context ctx, std::string value) {
        this->ctx = ctx;
        this->data_type = "string";
        this->value = value;
    }

    RuntimeResult String::add(const context::Context ctx, const std::shared_ptr<RuntimeValue> other) {
        if (other->data_type != "string") {
            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "cannot perform addition on 'int' and '" + other->data_type + "'")));
        }

        return RuntimeResult(std::make_shared<String>(String(ctx, this->value + std::static_pointer_cast<String>(other)->value)), nullptr);
    }

    RuntimeResult String::repr(const context::Context ctx) {
        return RuntimeResult(std::make_shared<String>(String(ctx, this->value)), nullptr);
    }
}