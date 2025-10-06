#pragma once

#include "../context/positions.hpp"
#include <memory>

namespace parser {
    enum struct NodeType {
        // Stmts
        BlockStmt,
        VariableDeclarationStmt,

        // Exprs
        AssignmentExpr,
        BinaryExpr,
        UnaryExpr,
        IntExpr,
        DoubleExpr,
        IdentifierExpr,
        ListTypeExpr,
    };
    
    struct Statement {
        NodeType node_type;
        context::Context ctx;
    };

    struct Expression : public Statement {};
}