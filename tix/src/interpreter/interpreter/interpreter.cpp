#include "interpreter.hpp"
#include "../../parser/stmt.hpp"
#include "../values/values.hpp"
#include "../../errors/error_handler.hpp"
#include "../../parser/expr.hpp"
#include "../values/scopes.hpp"
#include <memory>
#include <cmath>

#define BlockStmt parser::NodeType::BlockStmt
#define VariableDeclarationStmt parser::NodeType::VariableDeclarationStmt
#define IfElseStmt parser::NodeType::IfElseStmt
#define WhileStmt parser::NodeType::WhileStmt
#define ForV1Stmt parser::NodeType::ForV1Stmt

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
        case IfElseStmt: {
            std::shared_ptr<parser::IfElseStatement> if_else = std::static_pointer_cast<parser::IfElseStatement>(stmt);
            while (if_else != nullptr) {
                RuntimeResult condition = this->evaluate(scope, if_else->condition);
                if (condition.error != nullptr) {
                    return condition;
                }

                if (condition.result->data_type != "boolean") {
                    return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(if_else->condition->ctx, "expected a boolean in the condition parameter")));
                }

                std::shared_ptr<Boolean> boolean = std::static_pointer_cast<Boolean>(condition.result);
                if (boolean->value) {
                    RuntimeResult body = this->evaluate(std::make_shared<Scope>(Scope(scope)), if_else->body);
                    return body;
                }

                if_else = if_else->next;
            }

            return RuntimeResult(nullptr, nullptr);
        }
        case WhileStmt: {
            std::shared_ptr<parser::WhileStatement> while_ = std::static_pointer_cast<parser::WhileStatement>(stmt);
            while (true) {
                RuntimeResult condition = this->evaluate(scope, while_->condition);
                if (condition.error != nullptr) {
                    return condition;
                }

                if (condition.result->data_type != "boolean") {
                    return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(while_->condition->ctx, "expected a boolean in the condition parameter")));
                }

                std::shared_ptr<Boolean> boolean = std::static_pointer_cast<Boolean>(condition.result);
                if (!boolean->value) {
                    return RuntimeResult(nullptr, nullptr);
                }

                RuntimeResult body = this->evaluate(std::make_shared<Scope>(Scope(scope)), while_->body);
                if (body.error != nullptr) {
                    return body;
                }
            }
        }
        case ForV1Stmt: {
            std::shared_ptr<parser::ForV1Statement> for_v1 = std::static_pointer_cast<parser::ForV1Statement>(stmt);
            std::shared_ptr<Scope> child_scope = std::make_shared<Scope>(Scope(scope));
            
            RuntimeResult initialization = this->evaluate(child_scope, for_v1->initialization);
            if (initialization.error != nullptr) {
                return initialization;
            }

            while (true) {
                RuntimeResult condition = this->evaluate(child_scope, for_v1->condition);
                if (condition.error != nullptr) {
                    return condition;
                }

                if (condition.result->data_type != "boolean") {
                    return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(for_v1->condition->ctx, "expected a boolean in the condition parameter")));
                }

                std::shared_ptr<Boolean> boolean = std::static_pointer_cast<Boolean>(condition.result);
                if (!boolean->value) {
                    return RuntimeResult(nullptr, nullptr);
                }

                RuntimeResult body = this->evaluate(std::make_shared<Scope>(Scope(child_scope)), for_v1->body);
                if (body.error != nullptr) {
                    return body;
                }

                RuntimeResult increment = this->evaluate(child_scope, for_v1->increment);
                if (increment.error != nullptr) {
                    return increment;
                }
            }
        }
        case AssignmentExpr: {
            std::shared_ptr<parser::AssignmentExpression> assignment = std::static_pointer_cast<parser::AssignmentExpression>(stmt);
            switch (assignment->assigner->node_type) {
            case IdentifierExpr: {
                RuntimeResult assignee = this->evaluate(scope, assignment->assignee);
                if (assignee.error != nullptr) {
                    return assignee;
                }

                std::shared_ptr<parser::IdentifierExpression> variable = std::static_pointer_cast<parser::IdentifierExpression>(assignment->assigner);
                std::string var_name = variable->var_name;
                if (assignment->op == "=") {
                    return scope->assign(assignment->ctx, var_name, assignee.result);
                } else if (assignment->op == "+=") {
                    RuntimeResult value = scope->get(variable->ctx, var_name);
                    if (value.error) {
                        return value;
                    }

                    RuntimeResult assigned = value.result->add(assignment->ctx, assignee.result);
                    if (assigned.error) {
                        return assigned;
                    }

                    return scope->assign(assignment->ctx, var_name, assigned.result);
                } else if (assignment->op == "-=") {
                    RuntimeResult value = scope->get(variable->ctx, var_name);
                    if (value.error) {
                        return value;
                    }

                    RuntimeResult assigned = value.result->subtract(assignment->ctx, assignee.result);
                    if (assigned.error) {
                        return assigned;
                    }

                    return scope->assign(assignment->ctx, var_name, assigned.result);
                } else if (assignment->op == "*=") {
                    RuntimeResult value = scope->get(variable->ctx, var_name);
                    if (value.error) {
                        return value;
                    }

                    RuntimeResult assigned = value.result->multiply(assignment->ctx, assignee.result);
                    if (assigned.error) {
                        return assigned;
                    }

                    return scope->assign(assignment->ctx, var_name, assigned.result);
                } else if (assignment->op == "/=") {
                    RuntimeResult value = scope->get(variable->ctx, var_name);
                    if (value.error) {
                        return value;
                    }

                    RuntimeResult assigned = value.result->divide(assignment->ctx, assignee.result);
                    if (assigned.error) {
                        return assigned;
                    }

                    return scope->assign(assignment->ctx, var_name, assigned.result);
                } else if (assignment->op == "%=") {
                    RuntimeResult value = scope->get(variable->ctx, var_name);
                    if (value.error) {
                        return value;
                    }

                    RuntimeResult assigned = value.result->mod(assignment->ctx, assignee.result);
                    if (assigned.error) {
                        return assigned;
                    }

                    return scope->assign(assignment->ctx, var_name, assigned.result);
                }
                
                return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(assignment->ctx, "unsupported operand: '" + assignment->op + "'")));
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

            if (binary->op == "==") {
                return lhs.result->equals(binary->ctx, rhs.result);
            }

            if (binary->op == "!=") {
                return lhs.result->not_equals(binary->ctx, rhs.result);
            }

            if (binary->op == ">") {
                return lhs.result->greater_than(binary->ctx, rhs.result);
            }

            if (binary->op == "<") {
                return lhs.result->smaller_than(binary->ctx, rhs.result);
            }

            if (binary->op == ">=") {
                return lhs.result->greater_than_or_equals(binary->ctx, rhs.result);
            }

            if (binary->op == "<=") {
                return lhs.result->smaller_than_or_equals(binary->ctx, rhs.result);
            }

            if (binary->op == "&&") {
                return lhs.result->and_(binary->ctx, rhs.result);
            }

            if (binary->op == "||") {
                return lhs.result->or_(binary->ctx, rhs.result);
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
            
            if (unary->sign == "!") {
                return value.result->not_(unary->ctx);
            }

            if (unary->sign == "++") {
                switch (unary->value->node_type) {
                case IdentifierExpr: {
                    RuntimeResult assigned = value.result->increment(unary->ctx);
                    if (assigned.error != nullptr) {
                        return assigned;
                    }

                    return scope->assign(unary->ctx, std::static_pointer_cast<parser::IdentifierExpression>(unary->value)->var_name, assigned.result);
                }
                default:
                    return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(unary->value->ctx, "assignment check not sturdy enough")));
                }
            }

            if (unary->sign == "--") {
                switch (unary->value->node_type) {
                case IdentifierExpr: {
                    RuntimeResult assigned = value.result->decrement(unary->ctx);
                    if (assigned.error != nullptr) {
                        return assigned;
                    }

                    return scope->assign(unary->ctx, std::static_pointer_cast<parser::IdentifierExpression>(unary->value)->var_name, assigned.result);
                }
                default:
                    return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(unary->value->ctx, "assignment check not sturdy enough")));
                }
            }

            if (unary->sign == "++ end") {
                switch (unary->value->node_type) {
                case IdentifierExpr: {
                    RuntimeResult assigned = value.result->increment(unary->ctx);
                    if (assigned.error != nullptr) {
                        return assigned;
                    }

                    RuntimeResult rr = scope->assign(unary->ctx, std::static_pointer_cast<parser::IdentifierExpression>(unary->value)->var_name, assigned.result);
                    if (rr.error != nullptr) {
                        return rr;
                    }

                    return value;
                }
                default:
                    return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(unary->value->ctx, "assignment check not sturdy enough")));
                }
            }

            if (unary->sign == "-- end") {
                switch (unary->value->node_type) {
                case IdentifierExpr: {
                    RuntimeResult assigned = value.result->decrement(unary->ctx);
                    if (assigned.error != nullptr) {
                        return assigned;
                    }

                    RuntimeResult rr = scope->assign(unary->ctx, std::static_pointer_cast<parser::IdentifierExpression>(unary->value)->var_name, assigned.result);
                    if (rr.error != nullptr) {
                        return rr;
                    }

                    return value;
                }
                default:
                    return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(unary->value->ctx, "assignment check not sturdy enough")));
                }
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
                    {"log", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
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
                    {"logln", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
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
                    {"prompt", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
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

            if (module_name == "math") {
                context::Context ctx = get->ctx;
                return RuntimeResult(std::make_shared<Module>(Module(ctx, {
                    {"floor", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'floor', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expect a double in 'val'")));
                        }

                        double val = std::static_pointer_cast<Double>(args[0])->value;
                        long long int_part = val;
                        if (int_part < 0 && val-int_part != 0) {
                            int_part--;
                        }

                        return RuntimeResult(std::make_shared<Int>(Int(ctx, int_part)), nullptr);
                    }))},
                    {"ceil", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'ceil', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expect a double in 'val'")));
                        }

                        double val = std::static_pointer_cast<Double>(args[0])->value;
                        long long int_part = val;
                        if (int_part < 0 && val-int_part != 0) {
                            int_part--;
                        }

                        return RuntimeResult(std::make_shared<Int>(Int(ctx, int_part+1)), nullptr);
                    }))},
                    {"abs_int", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'abs_int', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "int") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a int in 'val'")));
                        }

                        long long value = std::static_pointer_cast<Int>(args[0])->value;
                        return RuntimeResult(std::make_shared<Int>(Int(ctx, (value >= 0 ? value : -value))), nullptr);
                    }))},
                    {"abs_double", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'abs_double', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'val'")));
                        }

                        double value = std::static_pointer_cast<Double>(args[0])->value;
                        return RuntimeResult(std::make_shared<Double>(Double(ctx, (value >= 0 ? value : -value))), nullptr);
                    }))},
                    {"dist1d_int", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 2) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 2 arguments in 'dist1d_int', got " + std::to_string(args.size()) + "/2")));
                        }

                        if (args[0]->data_type != "int") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a int in 'start'")));
                        }

                        if (args[1]->data_type != "int") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a int in 'end'")));
                        }

                        long long diff = std::static_pointer_cast<Int>(args[0])->value - std::static_pointer_cast<Int>(args[1])->value;
                        return RuntimeResult(std::make_shared<Int>(Int(ctx, (diff >= 0 ? diff : -diff))), nullptr);
                    }))},
                    {"dist1d_double", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 2) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 2 arguments in 'dist1d_double', got " + std::to_string(args.size()) + "/2")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'start'")));
                        }

                        if (args[1]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'end'")));
                        }

                        double diff = std::static_pointer_cast<Double>(args[0])->value - std::static_pointer_cast<Double>(args[1])->value;
                        return RuntimeResult(std::make_shared<Double>(Double(ctx, (diff >= 0 ? diff : -diff))), nullptr);
                    }))},
                    {"pi", std::make_shared<Double>(Double(ctx, 3.141592653589793))},
                    {"e", std::make_shared<Double>(Double(ctx, 2.718281828459045))},
                    {"sin", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'sin', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'theta'")));
                        }

                        double theta = std::static_pointer_cast<Double>(args[0])->value;
                        return RuntimeResult(std::make_shared<Double>(Double(ctx, std::sin(theta))), nullptr);
                    }))},
                    {"cos", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'cos', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'theta'")));
                        }

                        double theta = std::static_pointer_cast<Double>(args[0])->value;
                        return RuntimeResult(std::make_shared<Double>(Double(ctx, std::cos(theta))), nullptr);
                    }))},
                    {"to_radians", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'to_radians', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'degrees'")));
                        }

                        double degrees = std::static_pointer_cast<Double>(args[0])->value;
                        return RuntimeResult(std::make_shared<Double>(Double(ctx, degrees/180*3.141592653589793)), nullptr);
                    }))},
                    {"to_degrees", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'to_degrees', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'radians'")));
                        }

                        double radians = std::static_pointer_cast<Double>(args[0])->value;
                        return RuntimeResult(std::make_shared<Double>(Double(ctx, radians*180.0/3.141592653589793)), nullptr);
                    }))},
                    {"sqrt", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'sqrt', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'val'")));
                        }

                        double value = std::static_pointer_cast<Double>(args[0])->value;
                        if (value < 0) {
                            return RuntimeResult(nullptr, std::make_shared<errors::MathError>(errors::MathError(ctx, "i don't want to work with complex numbers...")));
                        }

                        return RuntimeResult(std::make_shared<Double>(Double(ctx, std::sqrt(value))), nullptr);
                    }))},
                    {"cbrt", std::make_shared<BuiltInFunction>(BuiltInFunction(ctx, [scope](context::Context ctx, std::vector<std::shared_ptr<RuntimeValue>> args) {
                        if (args.size() != 1) {
                            return RuntimeResult(nullptr, std::make_shared<errors::ArgumentError>(errors::ArgumentError(ctx, "expected 1 argument in 'cbrt', got " + std::to_string(args.size()) + "/1")));
                        }

                        if (args[0]->data_type != "double") {
                            return RuntimeResult(nullptr, std::make_shared<errors::TypeError>(errors::TypeError(ctx, "expected a double in 'val'")));
                        }

                        double value = std::static_pointer_cast<Double>(args[0])->value;
                        return RuntimeResult(std::make_shared<Double>(Double(ctx, std::cbrt(value))), nullptr);
                    }))},
                })), nullptr);
            }
            
            return RuntimeResult(nullptr, std::make_shared<errors::ModuleError>(errors::ModuleError(stmt->ctx, "module '" + get->module_name + "' does not exist")));
        }
        default:
            return RuntimeResult(nullptr, std::make_shared<errors::InterpreterError>(errors::InterpreterError(stmt->ctx, "unsupported node type: " + std::to_string(static_cast<int>(stmt->node_type)))));
        }
    }
}