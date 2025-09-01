#include "parser.hpp"
#include "exprAST.hpp"
#include "lexer.hpp"

#include "llvm/Support/raw_ostream.h"

static void loop();
static void initializeModule();
static void handleDefinition();
static void handleExtern();
static void handleTopLevelExpression();

int main() {

    initPrecedence();

    // prime the first token.
    std::fprintf(stderr, "> ");
    getNextToken();

    initializeModule();

    loop();

    return 0;
}

static void loop() {
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

static void initializeModule() {
    // Open a new context and module.
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("epic jit", *context);

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

static void handleDefinition() {
    if (auto funcAST = parseDefinition()) {
        if (auto *funcIR = funcAST->codegen()) {
            std::fprintf(stderr, "Read a function definition:\n");
            funcIR->print(llvm::errs());
        }
    } else {
        // Skips token for error recovery.
        getNextToken();
    }   
}

static void handleExtern() {
    if (auto protoAST = parseExtern()) {
        if (auto *funcIR = protoAST->codegen()) {
            std::fprintf(stderr, "Read an extern:\n");
            funcIR->print(llvm::errs());
        }
    } else {
        getNextToken();
    }
}

static void handleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (auto funcAST = parseTopLevelExpr()) {
        if (auto *funcIR = funcAST->codegen()) {
            std::fprintf(stderr, "Read a top-level expr:\n");
            funcIR->print(llvm::errs());

            // Remove the anonymous expression.
            funcIR->eraseFromParent();
        }
    } else {
        getNextToken();
    }
}
