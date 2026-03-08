#include "../values.hpp"
#include "../../../context/positions.hpp"
#include "../../../errors/error_handler.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace interpreter {
    Module::Module(const context::Context& ctx, const std::unordered_map<std::string, std::shared_ptr<RuntimeValue>> vars) {
        this->ctx = ctx;
        this->data_type = "module";
        this->vars = vars;
    }

    RuntimeResult Module::access(const context::Context& ctx, const std::string& attr) {
        auto it = this->vars.find(attr);
        if (it == this->vars.end()) {
            return RuntimeResult(nullptr, std::make_shared<errors::ModuleError>(errors::ModuleError(ctx, "this module has no attribute '" + attr + "' defined")));
        }

        std::shared_ptr<RuntimeValue> value = it->second;
        value->ctx = ctx;
        return RuntimeResult(value, nullptr);
    }
}