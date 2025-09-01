#ifndef PARSER_H
#define PARSER_H

#include "exprAST.hpp"

#include <memory> 

std::unique_ptr<ExprAST> parseExpression();
std::unique_ptr<ExprAST> parseBinaryRHS(int exprPrec, std::unique_ptr<ExprAST> left);

extern int currToken;
int getNextToken(); 
void initPrecedence();
std::unique_ptr<ExprAST> logError(const char *str);
std::unique_ptr<FunctionAST> parseDefinition();
std::unique_ptr<PrototypeAST> parseExtern();
std::unique_ptr<FunctionAST> parseTopLevelExpr();

#endif
