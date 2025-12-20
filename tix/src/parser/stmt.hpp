#pragma once

#include "nodes.hpp"
#include "../context/positions.hpp"
#include <vector>
#include <memory>
#include <string>

namespace parser {
    struct BlockStatement : public Statement {
        std::vector<std::shared_ptr<Statement>> body;

        BlockStatement(const context::Context ctx);
    };

    struct VariableDeclarationStatement : public Statement {
        bool constant;
        std::shared_ptr<Expression> data_type;
        std::string var_name;
        std::shared_ptr<Expression> value;

        VariableDeclarationStatement(const context::Context ctx, const bool constant, const std::shared_ptr<Expression> data_type, const std::string var_name, const std::shared_ptr<Expression> value);
    };

    struct IfElseStatement : public Statement {
        std::shared_ptr<Expression> condition;
        std::shared_ptr<BlockStatement> body;
        std::shared_ptr<IfElseStatement> next;

        IfElseStatement(const context::Context ctx, const std::shared_ptr<Expression> condition, const std::shared_ptr<BlockStatement> body, const std::shared_ptr<IfElseStatement> next);
    };
}