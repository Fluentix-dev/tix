#include "lexer/lexer.hpp"
#include "errors/error_handler.hpp"
#include "parser/parser.hpp"
#include "parser/nodes.hpp"
#include "parser/stmt.hpp"
#include "parser/expr.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>

void debug_expr(parser::Expression* expr) {
    switch (expr->node_type) {
    case parser::NodeType::BinaryExpr: {
        parser::BinaryExpression* binary = static_cast<parser::BinaryExpression*>(expr);
        std::cout << "(";
        debug_expr(binary->lhs);
        std::cout << " " << binary->op << " ";
        debug_expr(binary->rhs);
        std::cout << ")";
        break;
    }
    case parser::NodeType::UnaryExpr: {
        parser::UnaryExpression* unary = static_cast<parser::UnaryExpression*>(expr);
        std::cout << "(" << unary->sign;
        debug_expr(unary->value);
        std::cout << ")";
        break;
    }
    case parser::NodeType::IntExpr:
        std::cout << "(" << static_cast<parser::IntExpression*>(expr)->value << ")";
        break;
    case parser::NodeType::DoubleExpr:
        std::cout << "(" << static_cast<parser::DoubleExpression*>(expr)->value << ")";
        break;
    default:
        std::cout << "This AST node has not been supported for debugging: " << static_cast<int>(expr->node_type);
    }
}

void debug_stmt(const size_t indentation, parser::Statement* stmt) {
    for (size_t i = 0; i < indentation; i++) {
        std::cout << "  ";
    }

    switch (stmt->node_type) {
    case parser::NodeType::BlockStmt: {
        parser::BlockStatement* block = static_cast<parser::BlockStatement*>(stmt);
        std::cout << "(BLOCK len=" << block->body.size() << " indentation=" << indentation << ")\n";
        for (parser::Statement* &stmt : block->body) {
            debug_stmt(indentation+1, stmt);
        }

        break;
    }
    default:
        debug_expr(static_cast<parser::Expression*>(stmt));
    }

    std::cout << "\n";
}

int main() {
    std::ifstream src("files/main.tx");
    if (!src) {
        std::cout << "File main.tx cannot be found!\n";
        return 0;
    }
    std::stringstream code;
    std::string line;
    while (src.is_open() && std::getline(src, line)) {
        code << line << "\n";
    }

    std::string real = code.str();
    real.erase(std::prev(real.end())); // Remove the ending newline

    lexer::Lexer lex = lexer::Lexer("main.tx", real);
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

    debug_stmt(0, parse.block);
    return 0;

}
