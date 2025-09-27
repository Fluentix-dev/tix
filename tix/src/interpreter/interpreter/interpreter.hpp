#pragma once

#include "../values/values.hpp"
#include "../../parser/stmt.hpp"
#include "../../parser/nodes.hpp"
#include "../../errors/error_handler.hpp"
#include <memory>

namespace interpreter {
    struct Interpreter {
        std::shared_ptr<errors::Error> error;
        Interpreter(const std::shared_ptr<parser::BlockStatement> ast);
        void run();
    private:
        std::shared_ptr<parser::BlockStatement> ast;
        RuntimeResult evaluate(const std::shared_ptr<parser::Statement> stmt);
    };
}