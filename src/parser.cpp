#include "parser.hpp"

int currToken {};

int getNextToken() {
    return currToken = getToken();
}

// helpers for error handling.
std::unique_ptr<ExprAST> logError(const char *str) {
    std::cerr << "Error: " << str << "\n";
    return nullptr;
}

std::unique_ptr<ExprAST> logErrorP(const char *str) {
    logError(str);
    return nullptr;
}

std::unique_ptr<ExprAST> parseNumberExpr() {
    auto result = std::make_unique<NumberExprAST>(numVal);
}