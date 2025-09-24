#pragma once

#include "nodes.hpp"
#include "../context/positions.hpp"
#include <vector>
#include <memory>

namespace parser {
    struct BlockStatement : public Statement {
        std::vector<std::shared_ptr<Statement>> body;

        BlockStatement(const context::Context ctx);
    };
}