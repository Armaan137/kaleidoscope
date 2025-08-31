#ifndef EXPRAST_H
#define EXPRAST_H 

#include <string>
#include <memory>
#include <vector>
#include <ostream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

// abtract interface for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;
    // Emit IR for that AST node and all the things it depends on. Value is the class used to represent a SSA register.
    virtual llvm::Value *codegen() = 0;
};

// expression class for a literal numeric value.
class NumberExprAST : public ExprAST {
private:
    double val;

public:
    NumberExprAST(double val) : val(val) {}

    llvm::Value *codegen() override;
};

// for a reference to a named variable.
class VariableExprAST : public ExprAST {
private:
    std::string name;

public:
    VariableExprAST(const std::string& name) : name(name) {}

    llvm::Value *codegen() override;
};

// a binary operator application with two operands.
class BinaryExprAST : public ExprAST {
private:
    char op;
    std::unique_ptr<ExprAST> left, right;

public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> left, std::unique_ptr<ExprAST> right) 
    : op(op), left(std::move(left)), right(std::move(right)) {}

    llvm::Value *codegen() override;
};

// a function call.
class CallExprAST : public ExprAST {
private:
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args; 

public:
    CallExprAST(const std::string& callee, std::vector<std::unique_ptr<ExprAST>> args) 
    : callee(callee), args(std::move(args)) {}

    llvm::Value *codegen() override;
};

// the prototype for a function.
class PrototypeAST {
private:
    std::string name;
    std::vector<std::string> args;

public:
    PrototypeAST(const std::string& name, std::vector<std::string> args) 
    : name(name), args(args) {}

    llvm::Function *codegen();
    const std::string &getName() const { return name; }
    size_t getNumArgs() const { return args.size(); }
    const std::string &getArgName(unsigned int i) const { return args[i]; }
};

// a function definiton.
class FunctionAST {
private:
    std::unique_ptr<PrototypeAST> prototype;
    std::unique_ptr<ExprAST> body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprAST> body) 
    : prototype(std::move(prototype)), body(std::move(body)) {}

    llvm::Function *codegen();
};

llvm::Value *logErrorV(const char *str);

#endif
