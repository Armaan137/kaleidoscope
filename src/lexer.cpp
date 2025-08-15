#include "lexer.hpp"
#include <cstdio> 
#include <cctype>
#include <cstdlib>

std::string IdentifierStr {};
double NumVal {};

// returns the next token from standard input.
int gettok() {
    static int lastChar {};

    // skip whitespace.
    while (std::isspace(lastChar)) {
        lastChar = std::getchar();
    }

    if (std::isalpha(lastChar)) {
        IdentifierStr = lastChar;

        while (std::isalnum(lastChar = std::getchar())) {
            IdentifierStr += lastChar;
        }

        if (IdentifierStr == "def") {
            return tok_def;
        } 

        if (IdentifierStr == "extern") {
            return tok_extern;
        }

        return tok_identifier;
    }

    if (std::isdigit(lastChar) || lastChar == '.') {
        std::string numStr {};

        do {
            numStr += lastChar;
            lastChar = std::getchar();
        } while (std::isdigit(lastChar) || lastChar == '.');

        NumVal = std::strtod(numStr.c_str(), 0);

        return tok_number;
    }

    if (lastChar == '#') {
        do {
            lastChar = std::getchar();
        } while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

        if (lastChar != EOF) {
            return gettok();
        }
    }

    if (lastChar == EOF) {
        return tok_eof;
    }

    int thisChar {lastChar};
    lastChar = std::getchar();

    return thisChar;
}
