#include "interpreter.hpp"
#include "../../parser/stmt.hpp"
#include "../values/values.hpp"
#include "../../errors/error_handler.hpp"
#include "../../parser/expr.hpp"
#include "../values/scopes.hpp"
#include <memory>

#define BlockStmt parser::NodeType::BlockStmt
#define VariableDeclarationStmt parser::NodeType::VariableDeclarationStmt

#define AssignmentExpr parser::NodeType::AssignmentExpr
#define BinaryExpr parser::NodeType::BinaryExpr
#define UnaryExpr parser::NodeType::UnaryExpr
#define IdentifierExpr parser::NodeType::IdentifierExpr
#define IntExpr parser::NodeType::IntExpr
#define DoubleExpr parser::NodeType::DoubleExpr

namespace interpreter {
    Interpreter::Interpreter(const std::shared_ptr<Scope> scope, const std::shared_ptr<parser::BlockStatement> ast) {
        this->scope = scope;
        this->ast = ast;
        this->error = nullptr;
    }

    void Interpreter::run() {
        RuntimeResult rr = this->evaluate(this->scope, this->ast);
        this->error = rr.error;
    }

    RuntimeResult Interpreter::evaluate(const std::shared_ptr<Scope> scope, const std::shared_ptr<parser::Statement> stmt) {
        switch (stmt->node_type) {
        case BlockStmt: {
            std::shared_ptr<parser::BlockStatement> block = std::static_pointer_cast<parser::BlockStatement>(stmt);
            for (const std::shared_ptr<parser::Statement> &stmt : block->body) {
                RuntimeResult res = evaluate(scope, stmt);
                if (res.error != nullptr) {
                    return res;
                }
                
                if (res.result == nullptr) {
                    continue;
                }

                if (res.result->data_type == "int") {
                    std::cout << std::static_pointer_cast<Int>(res.result)->value << "\n";
                } else if (res.result->data_type == "double") {
                    std::cout << std::static_pointer_cast<Double>(res.result)->value << "\n";
                }
            }

            return RuntimeResult(nullptr, nullptr);
        }
        case VariableDeclarationStmt: {
            std::shared_ptr<parser::VariableDeclarationStatement> variable_declaration = std::static_pointer_cast<parser::VariableDeclarationStatement>(stmt);
            RuntimeResult data_type = this->evaluate(scope, variable_declaration->data_type);
            if (data_type.error != nullptr) {
                return data_type;
            }

            if (data_type.result->data_type != "type") {
                return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(variable_declaration->ctx, "expected a data type to declare a variable")));
            }

            RuntimeResult value = this->evaluate(scope, variable_declaration->value);
            if (value.error != nullptr) {
                return value;
            }

            return scope->declare(variable_declaration->ctx, variable_declaration->constant, std::static_pointer_cast<Type>(data_type.result)->type, variable_declaration->var_name, value.result);
        }
        case AssignmentExpr: {
            std::shared_ptr<parser::AssignmentExpression> assignment = std::static_pointer_cast<parser::AssignmentExpression>(stmt);
            switch (assignment->assigner->node_type) {
            case IdentifierExpr: {
                RuntimeResult assignee = this->evaluate(scope, assignment->assignee);
                if (assignee.error != nullptr) {
                    return assignee;
                }

                return scope->assign(assignment->ctx, std::static_pointer_cast<parser::IdentifierExpression>(assignment->assigner)->var_name, assignee.result);
            }
            default:
                return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(assignment->assigner->ctx, "assignment check not sturdy enough")));
            }
        }
        case BinaryExpr: {
            std::shared_ptr<parser::BinaryExpression> binary = std::static_pointer_cast<parser::BinaryExpression>(stmt);
            RuntimeResult lhs = this->evaluate(scope, binary->lhs);
            if (lhs.error != nullptr) {
                return lhs;
            }

            RuntimeResult rhs = this->evaluate(scope, binary->rhs);
            if (rhs.error != nullptr) {
                return rhs;
            }

            if (binary->op == "+") {
                RuntimeResult res = lhs.result->add(binary->ctx, rhs.result);
                return res;
            }

            if (binary->op == "-") {
                return lhs.result->subtract(binary->ctx, rhs.result);
            }

            if (binary->op == "*") {
                return lhs.result->multiply(binary->ctx, rhs.result);
            }

            if (binary->op == "/") {
                return lhs.result->divide(binary->ctx, rhs.result);
            }

            if (binary->op == "%") {
                return lhs.result->mod(binary->ctx, rhs.result);
            }

            return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(binary->ctx, "unsupported operand '" + binary->op + "'")));
        }
        case UnaryExpr: {
            std::shared_ptr<parser::UnaryExpression> unary = std::static_pointer_cast<parser::UnaryExpression>(stmt);
            RuntimeResult value = this->evaluate(scope, unary->value);
            if (value.error != nullptr) {
                return value;
            }

            if (unary->sign == "+") {
                return value.result->unplus(unary->ctx);
            }

            if (unary->sign == "-") {
                return value.result->negate(unary->ctx);
            }

            if (unary->sign == "%") {
                return value.result->percent(unary->ctx);
            }

            return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(unary->ctx, "unsupported operand '" + unary->sign + "'")));
        }
        case IdentifierExpr:
            return scope->get(stmt->ctx, std::static_pointer_cast<parser::IdentifierExpression>(stmt)->var_name);
        case IntExpr:
            return RuntimeResult(std::make_shared<Int>(interpreter::Int(stmt->ctx, std::static_pointer_cast<parser::IntExpression>(stmt)->value)), nullptr);
        case DoubleExpr:
            return RuntimeResult(std::make_shared<Double>(interpreter::Double(stmt->ctx, std::static_pointer_cast<parser::DoubleExpression>(stmt)->value)), nullptr);
        default:
            return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(stmt->ctx, "unsupported node type: " + std::to_string(static_cast<int>(stmt->node_type)))));
        }
    }
}