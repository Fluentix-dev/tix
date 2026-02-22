#include "../values.hpp"
#include "../../../context/positions.hpp"
#include <functional>
#include <vector>
#include <memory>

namespace interpreter {
    BuiltInFunction::BuiltInFunction(const context::Context& ctx, const std::function<RuntimeResult(context::Context, std::vector<std::shared_ptr<RuntimeValue>>)> fn) {
        this->ctx = ctx;
        this->data_type = "builtin_func";
        this->fn = fn;
    }

    RuntimeResult BuiltInFunction::call(const context::Context& ctx, const std::vector<std::shared_ptr<RuntimeValue>> args) {
        RuntimeResult result = this->fn(ctx, args);
        if (result.error != nullptr) {
            return result;
        }

        return result;
    }
}