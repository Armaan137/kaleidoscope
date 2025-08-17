#ifndef LEXER_H
#define LEXER_H

#include <string>

enum Token {
  tok_eof = -1,

  // commands.
  tok_def = -2,
  tok_extern = -3,

  // primary.
  tok_identifier = -4,
  tok_number = -5,
};

// if the current token is an identifier, this stores it.
extern std::string IdentifierStr;

// if the current token is a numeric literal, this stores that.
extern double NumVal;

int getToken();

#endif
