#pragma once 

#include "nodes.hpp"
#include "../context/positions.hpp"
#include <string>

namespace parser {
    struct BinaryExpression : public Expression {
        Expression* lhs;
        std::string op;
        Expression* rhs;

        BinaryExpression(const context::Context ctx, Expression* lhs, const std::string op, Expression* rhs);
    };

    struct UnaryExpression : public Expression {
        std::string sign;
        Expression* value;

        UnaryExpression(const context::Context ctx, const std::string sign, Expression* value);
    };

    struct IntExpression : public Expression {
        long long value;

        IntExpression(const context::Context ctx, const long long value);
    };

    struct DoubleExpression : public Expression {
        double value;

        DoubleExpression(const context::Context ctx, const double value);
    };
}