#include "../values.hpp"
#include "../../../context/positions.hpp"

namespace interpreter {
    Null::Null(const context::Context ctx) {
        this->ctx = ctx;
        this->data_type = "null_type";
    }

    RuntimeResult Null::repr(const context::Context ctx) {
        return RuntimeResult(std::make_shared<String>(String(ctx, "null")), nullptr);
    }
};