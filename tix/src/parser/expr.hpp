#pragma once 

#include "nodes.hpp"
#include "../context/positions.hpp"
#include <string>
#include <memory>

namespace parser {
    struct BinaryExpression : public Expression {
        std::shared_ptr<Expression> lhs;
        std::string op;
        std::shared_ptr<Expression> rhs;

        BinaryExpression(const context::Context ctx, std::shared_ptr<Expression> lhs, const std::string op, std::shared_ptr<Expression> rhs);
    };

    struct UnaryExpression : public Expression {
        std::string sign;
        std::shared_ptr<Expression> value;

        UnaryExpression(const context::Context ctx, const std::string sign, std::shared_ptr<Expression> value);
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