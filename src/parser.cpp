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

// parses a number. 
// numberExpr ::= number.
std::unique_ptr<ExprAST> parseNumberExpr() {
    auto result = std::make_unique<NumberExprAST>(numVal);
    getNextToken();
    return std::move(result);
}

// parentExpr ::= '(' expression ')'.
std::unique_ptr<ExprAST> parseParenExpr() {
    getNextToken(); // consumes the opening parentheses.

    auto v = parseExpression();
    if (!v) {
        return nullptr;
    }

    if (currToken != ')') {
        return logError("expected ')'");
    }

    getNextToken(); // consumes the closing parantheses.

    return v;
    
}

// identifierExpr ::= identifier
// identfierExpr ::= '(' expression* ')'.
std::unique_ptr<ExprAST> parseIdentifierExpr() {
    std::string name = identifierStr;

    getNextToken(); // consume the identifier.

    // if the identifier is normal and not for a function.
    if (currToken != '(') {
        return std::make_unique<VariableExprAST>(name);
    }

    // is a function.
    getNextToken(); // consume the opening parantheses.

    std::vector<std::unique_ptr<ExprAST>> args;
    if (currToken != ')') {
        while (true) {
            if (auto arg = parseExpression()) {
                args.push_back(std::move(arg));
            } else {
                return nullptr;
            }

            if (currToken == ')') {
                break;
            }

            if (currToken != ',') {
                return logError("Expected ')' or ',' in argument list");
            }

            getNextToken();
        }

        getNextToken(); // consume the closing parantheses.

        return std::make_unique<CallExprAST>(name, std::move(args));
        
    }
}

// primary ::= identifierExpr
// primary ::= numberExpr
// primary ::= parenExpr
std::unique_ptr<ExprAST> parsePrimary() {
    switch (currToken) {
        default:
            return logError("Unknown token when expecting an expression.");
        case tok_identifier:
            return parseIdentifierExpr();
        case tok_number:
            return parseNumberExpr();
        case '(':
            return parseParenExpr();
    }
}