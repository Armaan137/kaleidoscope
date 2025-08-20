#ifndef PARSER_H
#define PARSER_H

#include "exprAST.hpp"
#include "lexer.hpp"

#include <memory> 
#include <iostream>
#include <string>
#include <vector>
#include <map>

extern int currToken;
extern std::map<char, int> binaryPrecedence;

int getNextToken();
std::unique_ptr<ExprAST> logError(const char *str);
std::unique_ptr<PrototypeAST> logErrorP(const char *str);
std::unique_ptr<ExprAST> parseNumberExpr();
std::unique_ptr<ExprAST> parseParenExpr();
std::unique_ptr<ExprAST> parseIdentifierExpr();
std::unique_ptr<ExprAST> parsePrimary();
int getTokenPrecedence();
bool is_ascii(unsigned char c);
std::unique_ptr<ExprAST> parseExpression();
std::unique_ptr<ExprAST> parseBinaryRHS(int exprPrec, std::unique_ptr<ExprAST> left);
std::unique_ptr<PrototypeAST> parsePrototype();
std::unique_ptr<FunctionAST> parseDefinition();
std::unique_ptr<PrototypeAST> parseExtern();
std::unique_ptr<FunctionAST> parseTopLevelExpr();
void initPrecedence();

#endif
