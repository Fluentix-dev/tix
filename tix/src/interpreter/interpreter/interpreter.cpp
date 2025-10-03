#include "interpreter.hpp"
#include "../../parser/stmt.hpp"
#include "../values/values.hpp"
#include "../../errors/error_handler.hpp"
#include "../../parser/expr.hpp"
#include <memory>

#define BlockStmt parser::NodeType::BlockStmt
#define BinaryExpr parser::NodeType::BinaryExpr
#define UnaryExpr parser::NodeType::UnaryExpr
#define IntExpr parser::NodeType::IntExpr
#define DoubleExpr parser::NodeType::DoubleExpr

namespace interpreter {
    Interpreter::Interpreter(const std::shared_ptr<parser::BlockStatement> ast) {
        this->ast = ast;
        this->error = nullptr;
    }

    void Interpreter::run() {
        RuntimeResult rr = this->evaluate(this->ast);
        this->error = rr.error;
    }

    RuntimeResult Interpreter::evaluate(const std::shared_ptr<parser::Statement> stmt) {
        switch (stmt->node_type) {
        case BlockStmt: {
            std::shared_ptr<parser::BlockStatement> block = std::static_pointer_cast<parser::BlockStatement>(stmt);
            for (const std::shared_ptr<parser::Statement> &stmt : block->body) {
                RuntimeResult res = evaluate(stmt);
                if (res.error != nullptr) {
                    return res;
                }

                if (res.result->data_type == "int") {
                    std::cout << std::static_pointer_cast<Int>(res.result)->value << "\n";
                } else if (res.result->data_type == "double") {
                    std::cout << std::static_pointer_cast<Double>(res.result)->value << "\n";
                }
            }

            return RuntimeResult(nullptr, nullptr);
        }
        case BinaryExpr: {
            std::shared_ptr<parser::BinaryExpression> binary = std::static_pointer_cast<parser::BinaryExpression>(stmt);
            RuntimeResult lhs = evaluate(binary->lhs);
            if (lhs.error != nullptr) {
                return lhs;
            }

            RuntimeResult rhs = evaluate(binary->rhs);
            if (rhs.error != nullptr) {
                return rhs;
            }

            if (binary->op == "+") {
                RuntimeResult res = lhs.result->add(rhs.result);
                return res;
            }

            if (binary->op == "-") {
                return lhs.result->subtract(rhs.result);
            }

            if (binary->op == "*") {
                return lhs.result->multiply(rhs.result);
            }

            if (binary->op == "/") {
                return lhs.result->divide(rhs.result);
            }

            if (binary->op == "%") {
                return lhs.result->mod(rhs.result);
            }

            return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(binary->ctx, "unsupported operand '" + binary->op + "'")));
        }
        case UnaryExpr: {
            std::shared_ptr<parser::UnaryExpression> unary = std::static_pointer_cast<parser::UnaryExpression>(stmt);
            RuntimeResult value = this->evaluate(unary->value);
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
        case IntExpr:
            return RuntimeResult(std::make_shared<Int>(interpreter::Int(stmt->ctx, std::static_pointer_cast<parser::IntExpression>(stmt)->value)), nullptr);
        case DoubleExpr:
            return RuntimeResult(std::make_shared<Double>(interpreter::Double(stmt->ctx, std::static_pointer_cast<parser::DoubleExpression>(stmt)->value)), nullptr);
        default:
            return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(stmt->ctx, "unsupported node type: " + std::to_string(static_cast<int>(stmt->node_type)))));
        }
    }
}