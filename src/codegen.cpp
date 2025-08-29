#include "exprAST.hpp"
#include "parser.hpp"

static std::unique_ptr<llvm::LLVMContext> context;   // Object that has core LLVM data structures. Stored unique values.
static std::unique_ptr<llvm::IRBuilder<>> builder;      // Helper object that helps generate LLVM instructions. Instances of IRBuilder class template keeps track of the current place to insert and has methods to create new instructions. 
static std::unique_ptr<llvm::Module> module;         // Construct that contains functions and global variables. Will own the memeory for all the IR generated.
static std::map<std::string, llvm::Value*> namedValues; // Keeps track of which values are defined in the current scope and what their LLVM representation is; symbol table.

llvm::Value *logErrorV(const char *str) {
    logError(str);
    return nullptr;
}

llvm::Value *NumberExprAST::codegen() {
    // ConstantFP is a subclass of Constant which itself is a value. Represents a floating-point constant in LLVM IR.
    // APFloat is LLVM's internal class for representing floating-points of arbitrary precision.
    return llvm::ConstantFP::get(*context, llvm::APFloat(val));
}

llvm::Value *VariableExprAST::codegen() {
    llvm::Value *variable = namedValues[name];
    if (!variable) {
        logErrorV("Unknown variable name.");
    }

    return variable;
}

llvm::Value *BinaryExprAST::codegen() {
    llvm::Value *l = left->codegen();
    llvm::Value *r = right->codegen();
    if (!l || !r) return nullptr;

    switch (op) {
        case '+':
            return builder->CreateFAdd(l, r, "addtmp");
        case '-':
            return builder->CreateFSub(l, r, "subtmp");
        case '*':
            return builder->CreateFMul(l, r, "multmp");
        case '<':
            l = builder->CreateFCmpULT(l, r, "cmptmp");
            // Convert the int bool value from CreateFCmpULT to a double.
            return builder->CreateUIToFP(l, llvm::Type::getDoubleTy(*context), "booltmp");
        default:
            return logErrorV("Invalid binary operator.");
    }
}

llvm::Value *CallExprAST::codegen() {
    // Look up function name in global module table.
    llvm::Function *calleeFunc = module->getFunction(callee);
    if (!calleeFunc) return logErrorV("Unknown function referenced.");

    // Argument mismatch error.
    if (calleeFunc->arg_size() != args.size()) return logErrorV("Incorrect number of arguments.");

    std::vector<llvm::Value*> argsV;
    for (unsigned int i = 0, e = args.size(); i != e; i++) {
        argsV.push_back(args[i]->codegen());
        if (!argsV.back()) return nullptr;
    }

    return builder->CreateCall(calleeFunc, argsV, "calltmp");
}
