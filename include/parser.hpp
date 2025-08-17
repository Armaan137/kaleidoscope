#ifndef PARSER_H
#define PARSER_H

#include "exprAST.hpp"
#include "lexer.hpp"

#include <memory> 
#include <iostream>

extern int currToken;

int getNextToken();
std::unique_ptr<ExprAST> logError(const char *str);
std::unique_ptr<ExprAST> logErrorP(const char *str);
std::unique_ptr<ExprAST> parseNumberExpr();

#endif
