#ifndef EXPRAST_H
#define EXPRAST_H

#include <string>
#include <memory>
#include <vector>
#include <ostream>

// abtract interface for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;
};

// expression class for a literal numeric value.
class NumberExprAST : public ExprAST {
private:
    double val;

public:
    NumberExprAST(double val) : val(val) {}
};

// for a reference to a named variable.
class VariableExprAST : public ExprAST {
private:
    std::string name;

public:
    VariableExprAST(const std::string& name) : name(name) {}
};

// a binary operator application with two operands.
class BinaryExprAST : public ExprAST {
private:
    char op;
    std::unique_ptr<ExprAST> left, right;

public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> left, std::unique_ptr<ExprAST> right) 
    : op(op), left(std::move(left)), right(std::move(right)) {}
};

// a function call.
class CallExprAST : public ExprAST {
private:
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args; 

public:
    CallExprAST(const std::string& callee, std::vector<std::unique_ptr<ExprAST>> args) 
    : callee(callee), args(std::move(args)) {}
};

// the prototype for a function.
class PrototypeAST {
private:
    std::string name;
    std::vector<std::string> args;

public:
    PrototypeAST(const std::string& name, std::vector<std::string> args) 
    : name(name), args(args) {}
};

// a function definiton.
class FunctionAST {
private:
    std::unique_ptr<PrototypeAST> prototype;
    std::unique_ptr<ExprAST> body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprAST> body) 
    : prototype(std::move(prototype)), body(std::move(body)) {}
};


#endif
