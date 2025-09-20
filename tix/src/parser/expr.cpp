#include "expr.hpp"
#include "nodes.hpp"
#include "../context/positions.hpp"
#include <string>

namespace parser {
    BinaryExpression::BinaryExpression(const context::Context ctx, Expression* lhs, const std::string op, Expression* rhs) {
        this->node_type = NodeType::BinaryExpr;
        this->ctx = ctx;
        this->lhs = lhs;
        this->op = op;
        this->rhs = rhs;
    }

    UnaryExpression::UnaryExpression(const context::Context ctx, const std::string sign, Expression* value) {
        this->node_type = NodeType::UnaryExpr;
        this->ctx = ctx;
        this->sign = sign;
        this->value = value;
    }

    IntExpression::IntExpression(const context::Context ctx, const long long value) {
        this->node_type = NodeType::IntExpr;
        this->ctx = ctx;
        this->value = value;
    }

    DoubleExpression::DoubleExpression(const context::Context ctx, const double value) {
        this->node_type = NodeType::DoubleExpr;
        this->ctx = ctx;
        this->value = value;
    }
}