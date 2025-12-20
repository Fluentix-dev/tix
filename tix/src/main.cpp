#include "lexer/lexer.hpp"
#include "errors/error_handler.hpp"
#include "parser/parser.hpp"
#include "parser/nodes.hpp"
#include "parser/stmt.hpp"
#include "parser/expr.hpp"
#include "interpreter/interpreter/interpreter.hpp"
#include "interpreter/values/scopes.hpp"
#include "interpreter/values/values.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <vector>

void debug_expr(std::shared_ptr<parser::Expression> expr) {
    switch (expr->node_type) {
    case parser::NodeType::BinaryExpr: {
        std::shared_ptr<parser::BinaryExpression> binary = std::static_pointer_cast<parser::BinaryExpression>(expr);
        std::cout << "(";
        debug_expr(binary->lhs);
        std::cout << " " << binary->op << " ";
        debug_expr(binary->rhs);
        std::cout << ")";
        break;
    }
    case parser::NodeType::UnaryExpr: {
        std::shared_ptr<parser::UnaryExpression> unary = std::static_pointer_cast<parser::UnaryExpression>(expr);
        std::cout << "(" << unary->sign;
        debug_expr(unary->value);
        std::cout << ")";
        break;
    }
    case parser::NodeType::CallExpr: {
        std::shared_ptr<parser::CallExpression> call = std::static_pointer_cast<parser::CallExpression>(expr);
        std::cout << "(";
        debug_expr(call->callee);
        std::cout << "(";
        for (size_t i = 0; i < call->arguments.size(); i++) {
            debug_expr(call->arguments[i]);
            if (i < call->arguments.size()-1) {
                std::cout << ", ";
            }
        }

        std::cout << "))";
        break;
    }
    case parser::NodeType::MemberExpr: {
        std::shared_ptr<parser::MemberExpression> member = std::static_pointer_cast<parser::MemberExpression>(expr);
        std::cout << "(";
        debug_expr(member->parent);
        std::cout << "." << member->member << ")";
        break;
    }
    case parser::NodeType::IntExpr:
        std::cout << "(" << std::static_pointer_cast<parser::IntExpression>(expr)->value << ")";
        break;
    case parser::NodeType::DoubleExpr:
        std::cout << "(" << std::static_pointer_cast<parser::DoubleExpression>(expr)->value << ")";
        break;
    case parser::NodeType::StringExpr:
        std::cout << "(" << std::static_pointer_cast<parser::StringExpression>(expr)->value << ")";
        break;
    case parser::NodeType::IdentifierExpr:
        std::cout << "(" << std::static_pointer_cast<parser::IdentifierExpression>(expr)->var_name << ")";
        break;
    case parser::NodeType::GetExpr:
        std::cout << "(GET \"" << std::static_pointer_cast<parser::GetExpression>(expr)->module_name << "\")";
        break;
    default:
        std::cout << "This AST node has not been supported for debugging: " << static_cast<int>(expr->node_type);
    }
}

void debug_stmt(const size_t indentation, std::shared_ptr<parser::Statement> stmt) {
    for (size_t i = 0; i < indentation; i++) {
        std::cout << "  ";
    }

    switch (stmt->node_type) {
    case parser::NodeType::BlockStmt: {
        std::shared_ptr<parser::BlockStatement> block = std::static_pointer_cast<parser::BlockStatement>(stmt);
        std::cout << "(BLOCK len=" << block->body.size() << " indentation=" << indentation << ")\n";
        for (const std::shared_ptr<parser::Statement> &stmt : block->body) {
            debug_stmt(indentation+1, stmt);
        }
        
        break;
    }
    case parser::NodeType::VariableDeclarationStmt: {
        std::shared_ptr<parser::VariableDeclarationStatement> variable_declaration = std::static_pointer_cast<parser::VariableDeclarationStatement>(stmt);
        debug_expr(variable_declaration->data_type);
        std::cout << " " << variable_declaration->var_name << " = ";
        debug_expr(variable_declaration->value);

        break;
    }
    case parser::NodeType::IfElseStmt: {
        std::shared_ptr<parser::IfElseStatement> if_else = std::static_pointer_cast<parser::IfElseStatement>(stmt);
        std::cout << "IF ";
        debug_expr(if_else->condition);
        std::cout << " THEN PERFORM\n";
        debug_stmt(indentation+1, if_else->body);
        if (if_else->next != nullptr) {
            std::cout << "\nELSE ";
            debug_stmt(indentation, if_else->next);
        }

        break;
    }
    default:
        debug_expr(std::static_pointer_cast<parser::Expression>(stmt));
    }

    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::string fn = "";
    for (int i = 1; i < argc; i++) {
        fn += argv[i];
    }

    std::ifstream src(fn);
    if (!src) {
        std::cout << "File " << fn << " cannot be found!\n";
        return 0;
    }

    std::stringstream code;
    std::string line;
    while (src.is_open() && std::getline(src, line)) {
        code << line << "\n";
    }

    std::string real = code.str();
    if (real.size() > 0) {
        real.erase(std::prev(real.end())); // Remove the ending newline
    }

    std::shared_ptr<interpreter::Scope> global_scope = std::make_shared<interpreter::Scope>(interpreter::Scope(nullptr));
    context::Context dummy_ctx = context::Context(fn, real, context::Position(0, 0), context::Position(0, 0));

    // Definitions starts from here
    global_scope->declare(dummy_ctx, true, "type", "int", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "int", {
        {"double", true}
    })));
    global_scope->declare(dummy_ctx, true, "type", "double", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "double", {})));
    global_scope->declare(dummy_ctx, true, "type", "function", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "function", {})));
    global_scope->declare(dummy_ctx, true, "type", "builtin_func", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "builtin_func", {
        {"function", true}
    })));
    global_scope->declare(dummy_ctx, true, "type", "module", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "module", {})));
    global_scope->declare(dummy_ctx, true, "type", "null_type", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "null_type", {})));
    global_scope->declare(dummy_ctx, true, "type", "string", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "string", {})));
    global_scope->declare(dummy_ctx, true, "type", "boolean", std::make_shared<interpreter::Type>(interpreter::Type(dummy_ctx, "boolean", {
        {"double", true},
        {"int", true}
    })));

    // Constants
    global_scope->declare(dummy_ctx, true, "null_type", "null", std::make_shared<interpreter::Null>(dummy_ctx));
    global_scope->declare(dummy_ctx, true, "boolean", "true", std::make_shared<interpreter::Boolean>(dummy_ctx, true));
    global_scope->declare(dummy_ctx, true, "boolean", "false", std::make_shared<interpreter::Boolean>(dummy_ctx, false));

    // and ends here

    lexer::Lexer lex = lexer::Lexer("files/main.tx", real);
    lex.tokenize();

    if (!lex.errors.empty()) {
        std::cout << "Total errors: " << lex.errors.size() << "\n";
        for (const errors::Error &err : lex.errors) {
            errors::print_error(err);
            std::cout << "\n";
        }

        return 0;
    }

    parser::Parser parse = parser::Parser("main.tx", real, lex.tokens);
    parse.parse();

    if (!parse.errors.empty()) {
        std::cout << "Total errors: " << parse.errors.size() << "\n";
        for (errors::Error &err : parse.errors) {
            errors::print_error(err);
            std::cout << "\n";
        }

        return 0;
    }

    //debug_stmt(0, parse.block);
    interpreter::Interpreter interpret = interpreter::Interpreter(global_scope, parse.block);
    interpret.run();
    if (interpret.error != nullptr) {
        errors::print_error(*interpret.error);
        std::cout << "\n";
        return 0;
    }

    return 0;
}