#include "stmt.hpp"
#include "nodes.hpp"
#include "../context/positions.hpp"

namespace parser {
    BlockStatement::BlockStatement(const context::Context ctx) {
        this->node_type = NodeType::BlockStmt;
        this->ctx = ctx;
        this->body.clear();
    }

    VariableDeclarationStatement::VariableDeclarationStatement(const context::Context ctx, const bool constant, const std::shared_ptr<Expression> data_type, const std::string var_name, const std::shared_ptr<Expression> value) {
        this->node_type = NodeType::VariableDeclarationStmt;
        this->ctx = ctx;
        this->constant = constant;
        this->data_type = data_type;
        this->var_name = var_name;
        this->value = value;
    }
}