#include "exprAST.hpp"
#include "parser.hpp"

static std::unique_ptr<llvm::LLVMContext> theContext; // Object that has core LLVM data structures.
static std::unique_ptr<llvm::IRBuilder<>> builder; // Helper object that helps generate LLVM instructions. Instances of IRBuilder class template keeps track of the current place to insert and has methods to create new instructions. 
static std::unique_ptr<llvm::Module> theModule; // Construct that contains functions and global variables. Will own the memeory for all the IR generated.
static std::map<std::string, llvm::Value*> namedValues; // Keeps track of which values are defined in the current scope and what their LLVM representation is.

llvm::Value *logErrorV(const char *str) {
    logError(str);
    return nullptr;
}
