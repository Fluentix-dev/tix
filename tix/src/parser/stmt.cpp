#include "stmt.hpp"
#include "nodes.hpp"
#include "../context/positions.hpp"

namespace parser {
    BlockStatement::BlockStatement(const context::Context ctx) {
        this->node_type = NodeType::BlockStmt;
        this->ctx = ctx;
        this->body.clear();
    }
}