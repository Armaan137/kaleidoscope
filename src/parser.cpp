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

std::unique_ptr<PrototypeAST> logErrorP(const char *str) {
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
    if (!v) return nullptr;
    
    if (currToken != ')') return logError("expected ')'");
    
    getNextToken(); // consumes the closing parantheses.

    return v;
}

// identifierExpr ::= identifier
// identfierExpr ::= '(' expression* ')'.
std::unique_ptr<ExprAST> parseIdentifierExpr() {
    std::string name = identifierStr;

    getNextToken(); // consume the identifier.

    // if the identifier is normal and not for a function.
    if (currToken != '(') return std::make_unique<VariableExprAST>(name);
    
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

            if (currToken == ')') break;
            if (currToken != ',') return logError("Expected ')' or ',' in argument list");
            
            getNextToken();
        }
    }

    getNextToken(); // consume the closing parantheses.

    return std::make_unique<CallExprAST>(name, std::move(args));
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

// holds precedence for each binary operator.
std::map<char, int> binaryPrecedence;

// helper function.
bool is_ascii(unsigned char c) {
    return c <= 127;
}

// get the precendence of the binary operator token.
int getTokenPrecedence() {
    if (!is_ascii(currToken)) return -1;
    
    // is a declared operator.
    int tokenPrecedence = binaryPrecedence[currToken];
    if (tokenPrecedence <= 0) return -1;

    return tokenPrecedence;
}

// expression ::= binaryOpRHS
std::unique_ptr<ExprAST> parseExpression() {
    auto left = parsePrimary();

    if (!left) return nullptr;

    return parseBinaryRHS(0, std::move(left));
}

// Operator precedence climbing.
// binaryOpRHS ::= ('+' primary)*.
std::unique_ptr<ExprAST> parseBinaryRHS(int exprPrec, std::unique_ptr<ExprAST> left) {

    // if this is a binary operator, find it's precedence.
    while (true) {
        int tokenPrecedence = getTokenPrecedence();

        // if this is a binary operator that binds as tightly as the current one, consume it. otherwise, return.
        if (tokenPrecedence < exprPrec) return left;

        int binaryOperator = currToken;
        getNextToken(); // consume the operator.

        // parse the primary expression after the binary operator.
        auto right = parsePrimary();

        if (!right) return nullptr;

        // if binary operator binds less tightly with the right hand side than the operator after the right hand side, 
        // let
        int nextPrecedence = getTokenPrecedence();

        if (tokenPrecedence < nextPrecedence) {
            right = parseBinaryRHS(tokenPrecedence + 1, std::move(right));

            if (!right) return nullptr;
        }
        
        // merge left/right.
        left = std::make_unique<BinaryExprAST>(binaryOperator, std::move(left), std::move(right));
    }
}

// prototype ::= id '(' id* ')'.
std::unique_ptr<PrototypeAST> parsePrototype() {
    if (currToken != tok_identifier) return logErrorP("Expected function name in prototype");

    std::string functinoName = identifierStr;
    getNextToken();

    if (currToken != '(') return logErrorP("Expected '(' in prototype");

    // read the list of argument names.
    std::vector<std::string> argumentNames;
    while (getNextToken() == tok_identifier) argumentNames.push_back(identifierStr);

    if (currToken != ')') return logErrorP("Expected ')' in prototype");

    getNextToken(); // consume the closing parentheses.

    return std::make_unique<PrototypeAST>(functinoName, std::move(argumentNames));
}

// definition ::= 'def' prototype expression.
std::unique_ptr<FunctionAST> parseDefinition() {
    getNextToken(); // eat the 'def'.

    auto prototype = parsePrototype();
    if (!prototype) return nullptr;

    if (auto expression = parseExpression()) 
        return std::make_unique<FunctionAST>(std::move(prototype), std::move(expression));

    return nullptr;
}

// external ::= 'extern' prototype.
std::unique_ptr<PrototypeAST> parseExtern() {
    getNextToken(); // consume the 'extern'.

    return parsePrototype();
}    

// topLevelExpr ::= expression.
std::unique_ptr<FunctionAST> parseTopLevelExpr() {
    if (auto expression = parseExpression()) {
        // make an anonymous prototype.
        auto prototype = std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(prototype), std::move(expression));
    }

    return nullptr;
}
