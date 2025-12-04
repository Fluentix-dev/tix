#pragma once 

#include "nodes.hpp"
#include "../context/positions.hpp"
#include <string>
#include <memory>

namespace parser {
    struct AssignmentExpression : public Expression {
        std::shared_ptr<Expression> assigner;
        std::shared_ptr<Expression> assignee;

        AssignmentExpression(const context::Context ctx, std::shared_ptr<Expression> assigner, const std::shared_ptr<Expression> assignee);
    };

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

    struct CallExpression : public Expression {
        std::shared_ptr<Expression> callee;
        std::vector<std::shared_ptr<Expression>> arguments;

        CallExpression(const context::Context ctx, const std::shared_ptr<Expression> callee, const std::vector<std::shared_ptr<Expression>> arguments);
    };

    struct MemberExpression : public Expression {
        std::shared_ptr<Expression> parent;
        std::string member;

        MemberExpression(const context::Context ctx, const std::shared_ptr<Expression> parent, std::string member);
    };

    struct IntExpression : public Expression {
        long long value;

        IntExpression(const context::Context ctx, const long long value);
    };

    struct DoubleExpression : public Expression {
        double value;

        DoubleExpression(const context::Context ctx, const double value);
    };

    struct StringExpression : public Expression {
        std::string value;

        StringExpression(const context::Context ctx, const std::string value);
    };

    struct IdentifierExpression : public Expression {
        std::string var_name;

        IdentifierExpression(const context::Context ctx, const std::string var_name);
    };

    struct ListTypeExpression : public Expression {
        std::shared_ptr<Expression> parent;

        ListTypeExpression(const context::Context ctx, const std::shared_ptr<Expression>);
    };

    struct GetExpression : public Expression {
        std::string module_name;

        GetExpression(const context::Context ctx, const std::string module_name);
    };
}