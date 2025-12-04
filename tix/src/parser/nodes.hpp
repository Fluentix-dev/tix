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
        CallExpr,
        MemberExpr,
        IntExpr,
        DoubleExpr,
        StringExpr,
        IdentifierExpr,
        ListTypeExpr,
        GetExpr
    };
    
    struct Statement {
        NodeType node_type;
        context::Context ctx;
    };

    struct Expression : public Statement {};
}