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
#define CallExpr parser::NodeType::CallExpr
#define MemberExpr parser::NodeType::MemberExpr
#define IdentifierExpr parser::NodeType::IdentifierExpr
#define IntExpr parser::NodeType::IntExpr
#define DoubleExpr parser::NodeType::DoubleExpr
#define StringExpr parser::NodeType::StringExpr
#define GetExpr parser::NodeType::GetExpr

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
        case CallExpr: {
            std::shared_ptr<parser::CallExpression> call = std::static_pointer_cast<parser::CallExpression>(stmt);
            RuntimeResult callee = this->evaluate(scope, call->callee);
            if (callee.error != nullptr) {
                return callee;
            }

            std::vector<std::shared_ptr<RuntimeValue>> args = {};
            for (const std::shared_ptr<parser::Expression> &arg : call->arguments) {
                RuntimeResult argument = this->evaluate(scope, arg);
                if (argument.error != nullptr) {
                    return argument;
                }

                args.push_back(argument.result);
            }

            return callee.result->call(call->ctx, args);
        }
        case MemberExpr: {
            std::shared_ptr<parser::MemberExpression> member = std::static_pointer_cast<parser::MemberExpression>(stmt);
            RuntimeResult parent = this->evaluate(scope, member->parent);
            if (parent.error != nullptr) {
                return parent;
            }

            return parent.result->access(member->ctx, member->member);
        }
        case IdentifierExpr:
            return scope->get(stmt->ctx, std::static_pointer_cast<parser::IdentifierExpression>(stmt)->var_name);
        case IntExpr:
            return RuntimeResult(std::make_shared<Int>(Int(stmt->ctx, std::static_pointer_cast<parser::IntExpression>(stmt)->value)), nullptr);
        case DoubleExpr:
            return RuntimeResult(std::make_shared<Double>(Double(stmt->ctx, std::static_pointer_cast<parser::DoubleExpression>(stmt)->value)), nullptr);
        case StringExpr:
            return RuntimeResult(std::make_shared<String>(String(stmt->ctx, std::static_pointer_cast<parser::StringExpression>(stmt)->value)), nullptr);
        case GetExpr: {
            std::shared_ptr<parser::GetExpression> get = std::static_pointer_cast<parser::GetExpression>(stmt);
            std::string module_name = get->module_name;
            if (module_name == "io") {
                context::Context ctx = get->ctx;
                return RuntimeResult(std::make_shared<Module>(Module(ctx, {
                    {"log", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope, ctx](std::vector<std::shared_ptr<RuntimeValue>> args) {
                        for (const std::shared_ptr<RuntimeValue> &arg : args) {
                            RuntimeResult str_repr = arg->repr(arg->ctx);
                            if (str_repr.error != nullptr) {
                                return str_repr;
                            }
                
                            if (str_repr.result->data_type != "string") {
                                return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(arg->ctx, "repr of a value must be a string")));
                            }
                
                            std::cout << std::static_pointer_cast<String>(str_repr.result)->value;
                        }
                
                        return scope->get(ctx, "null");
                    }))},
                    {"logln", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope, ctx](std::vector<std::shared_ptr<RuntimeValue>> args) {
                        for (const std::shared_ptr<RuntimeValue> &arg : args) {
                            RuntimeResult str_repr = arg->repr(arg->ctx);
                            if (str_repr.error != nullptr) {
                                return str_repr;
                            }
                
                            if (str_repr.result->data_type != "string") {
                                return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(arg->ctx, "repr of a value must be a string")));
                            }
                
                            std::cout << std::static_pointer_cast<String>(str_repr.result)->value;
                        }
                
                        std::cout << "\n";
                        return scope->get(ctx, "null");
                    }))},
                    {"prompt", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope, ctx](std::vector<std::shared_ptr<RuntimeValue>> args) {
                        for (const std::shared_ptr<RuntimeValue> &arg : args) {
                            RuntimeResult str_repr = arg->repr(arg->ctx);
                            if (str_repr.error != nullptr) {
                                return str_repr;
                            }
                
                            if (str_repr.result->data_type != "string") {
                                return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(arg->ctx, "repr of a value must be a string")));
                            }
                
                            std::cout << std::static_pointer_cast<String>(str_repr.result)->value;
                        }
                
                        std::string returned;
                        std::getline(std::cin, returned);
                        return RuntimeResult(std::make_shared<String>(String(ctx, returned)), nullptr);
                    }))}
                })), nullptr);
            }
            
            return RuntimeResult(nullptr, std::make_shared<errors::ModuleError>(errors::ModuleError(stmt->ctx, "module '" + get->module_name + "' does not exist")));
        }
        default:
            return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(stmt->ctx, "unsupported node type: " + std::to_string(static_cast<int>(stmt->node_type)))));
        }
    }
}