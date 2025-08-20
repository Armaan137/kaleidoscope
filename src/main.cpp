#include "main.hpp"

int main() {

    initPrecedence();

    // prime the first token.
    std::fprintf(stderr, "> ");
    getNextToken();

    loop();

    return 0;
}

void loop() {
    while (true) {
        std::fprintf(stderr, "> ");
        switch (currToken) {
        case tok_eof:
            return;
        case ';': // ignore top-level semicolons.
            getNextToken();
            break;
        case tok_def:
            handleDefinition();
            break;
        case tok_extern:
            handleExtern();
            break;
        default:
            handleTopLevelExpression();
            break;
        }
    }
}

void handleDefinition() {
    if (parseDefinition()) {
        std::fprintf(stderr, "Parsed a function definition.\n");
    } else {
        getNextToken();
    }   
}

void handleExtern() {
    if (parseExtern()) {
        std::fprintf(stderr, "Parsed an extern.\n");
    } else {
        getNextToken();
    }
}

void handleTopLevelExpression() {
    if (parseTopLevelExpr()) {
        std::fprintf(stderr, "Parsed a top-level expr\n");
    } else {
        getNextToken();
    }
}
