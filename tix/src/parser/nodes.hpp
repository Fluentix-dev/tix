#pragma once

#include "../context/positions.hpp"

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