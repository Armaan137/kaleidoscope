#include "exprAST.hpp"
#include "parser.hpp"

static std::unique_ptr<llvm::LLVMContext> context;      // Object that has core LLVM data structures. Stored unique values.
static std::unique_ptr<llvm::IRBuilder<>> builder;      // Helper object that helps generate LLVM instructions. Instances of IRBuilder class template keeps track of the current place to insert and has methods to create new instructions. 
static std::unique_ptr<llvm::Module> module;            // Construct that contains functions and global variables. Will own the memory for all the IR generated.
static std::map<std::string, llvm::Value*> namedValues; // Keeps track of which values are defined in the current scope and what their LLVM representation is; symbol table.

llvm::Value *logErrorV(const char *str) {
    logError(str);
    return nullptr;
}

// Turns double into an LLVM constant double.
llvm::Value *NumberExprAST::codegen() {
    // ConstantFP is a subclass of Constant which itself is a value. Represents a floating-point constant in LLVM IR.
    // APFloat is LLVM's internal class for representing floating-points of arbitrary precision.
    return llvm::ConstantFP::get(*context, llvm::APFloat(val));
}

// Variable lookup.
llvm::Value *VariableExprAST::codegen() {
    llvm::Value *variable = namedValues[name];
    if (!variable) {
        logErrorV("Unknown variable name.");
    }

    return variable;
}

// Lowers binary expressions into LLVM IR.
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

// Creates an LLVM call instruction that produces a double Value.
llvm::Value *CallExprAST::codegen() {
    // Look up function name in global module table.
    llvm::Function *calleeFunc = module->getFunction(callee);
    if (!calleeFunc) return logErrorV("Unknown function referenced.");

    // Argument mismatch error. Don't need to check types since everything is double.
    if (calleeFunc->arg_size() != args.size()) return logErrorV("Incorrect number of arguments.");

    // Codegen each argument. args[i] is itself an expression AST node.
    std::vector<llvm::Value*> argsV;
    for (unsigned int i = 0, e = args.size(); i != e; i++) {
        argsV.push_back(args[i]->codegen());
        if (!argsV.back()) return nullptr;
    }

    // Generate an LLVM call instruction. Returns a Value* that represents the result of the call.
    return builder->CreateCall(calleeFunc, argsV, "calltmp");
}

// Creates a function signature in LLVM IR.
llvm::Function *PrototypeAST::codegen() {
    // Build parameter type lists which are all doubles.
    std::vector<llvm::Type*> doubles(args.size(), llvm::Type::getDoubleTy(*context));

    // Create the FunctionType. Describes a function's signature (return type, parameter types, variadic flag).
    llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), doubles, false);

    // Create the LLVM function. Adds a new function to the LLVM module.
    llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, name, module.get());

    // Set the name of each of the function's arguments according to the names given in the prototype.
    unsigned int index {0};
    for (auto& arg : function->args()) {
        arg.setName(args[index++]);
    }

    return function;
}

// Emitting a function defintion.
llvm::Function *FunctionAST::codegen() {
    // Check for an existing function from an 'extern' declaration.
    llvm::Function *function = module->getFunction(prototype->getName());

    // If there is no existing function, codegen one from the prototype.
    if (!function) function = prototype->codegen();

    if (!function) return nullptr;

    // If the function already has basic blocks, that means it's already been defined.
    if (!function->empty()) return (llvm::Function*)logErrorV("Function cannot be redefined.");

    // Create a new basic block that is inserted into the function.
    llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(*context, "entry", function);

    // Tells builder to append subsequent instructions into the new basic block.
    builder->SetInsertPoint(basicBlock);

    // Record the function arguments in the namedValues map.
    namedValues.clear();
    for (auto& arg: function->args()) {
        namedValues[std::string(arg.getName())] = &arg;
    }

    // Recursively emits IR for the function body expression and yields the final double value.
    if (llvm::Value *returnVal = body->codegen()) {
        // Emit the return with that value.
        builder->CreateRet(returnVal);

        llvm::verifyFunction(*function);

        return function;
    }

    // Error reading body, remove the function.
    function->eraseFromParent();
    return nullptr;
}
