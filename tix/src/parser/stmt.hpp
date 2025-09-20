#pragma once

#include "nodes.hpp"
#include "../context/positions.hpp"
#include <vector>

namespace parser {
    struct BlockStatement : public Statement {
        std::vector<Statement*> body;

        BlockStatement(const context::Context ctx);
    };
}