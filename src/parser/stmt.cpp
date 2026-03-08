#include "stmt.hpp"
#include "nodes.hpp"
#include "../context/positions.hpp"

namespace parser {
    BlockStatement::BlockStatement(const context::Context& ctx) {
        this->node_type = NodeType::BlockStmt;
        this->ctx = ctx;
        this->body.clear();
    }

    VariableDeclarationStatement::VariableDeclarationStatement(const context::Context& ctx, const bool constant, const std::shared_ptr<Expression> data_type, const std::string& var_name, const std::shared_ptr<Expression> value) {
        this->node_type = NodeType::VariableDeclarationStmt;
        this->ctx = ctx;
        this->constant = constant;
        this->data_type = data_type;
        this->var_name = var_name;
        this->value = value;
    }

    IfElseStatement::IfElseStatement(const context::Context& ctx, const std::shared_ptr<Expression> condition, const std::shared_ptr<BlockStatement> body, const std::shared_ptr<IfElseStatement> next) {
        this->node_type = NodeType::IfElseStmt;
        this->ctx = ctx;
        this->condition = condition;
        this->body = body;
        this->next = next;
    }

    WhileStatement::WhileStatement(const context::Context& ctx, const std::shared_ptr<Expression> condition, const std::shared_ptr<BlockStatement> body) {
        this->node_type = NodeType::WhileStmt;
        this->ctx = ctx;
        this->condition = condition;
        this->body = body;
    }

    ForV1Statement::ForV1Statement(const context::Context& ctx, const std::shared_ptr<Statement> initialization, const std::shared_ptr<Expression> condition, const std::shared_ptr<Statement> increment, const std::shared_ptr<BlockStatement> body) {
        this->node_type = NodeType::ForV1Stmt;
        this->ctx = ctx;
        this->initialization = initialization;
        this->condition = condition;
        this->increment = increment;
        this->body = body;
    }

    BreakStatement::BreakStatement(const context::Context& ctx) {
        this->node_type = NodeType::BreakStmt;
        this->ctx = ctx;
    }

    ContinueStatement::ContinueStatement(const context::Context& ctx) {
        this->node_type = NodeType::ContinueStmt;
        this->ctx = ctx;
    }
}