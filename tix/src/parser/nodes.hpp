#pragma once

#include "../context/positions.hpp"
#include <memory>

namespace parser {
    enum struct NodeType {
        // Stmts
        BlockStmt,

        // Exprs
        BinaryExpr,
        UnaryExpr,
        IntExpr,
        DoubleExpr
    };
    
    struct Statement {
        NodeType node_type;
        context::Context ctx;
    };

    struct Expression : public Statement {};
}