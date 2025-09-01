#include "lexer.hpp"

#include <cstdio> 
#include <cctype>
#include <cstdlib>

std::string identifierStr {};
double numVal {};

// returns the next token from standard input.
int getToken() {
    static int lastChar {' '};

    // skip whitespace.
    while (std::isspace(lastChar)) {
        lastChar = std::getchar();
    }

    if (std::isalpha(lastChar)) {
        identifierStr = lastChar;

        while (std::isalnum(lastChar = std::getchar())) {
            identifierStr += lastChar;
        }

        if (identifierStr == "def") {
            return tok_def;
        } 

        if (identifierStr == "extern") {
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

        numVal = std::strtod(numStr.c_str(), 0);

        return tok_number;
    }

    if (lastChar == '#') {
        do {
            lastChar = std::getchar();
        } while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

        if (lastChar != EOF) {
            return getToken();
        }
    }

    if (lastChar == EOF) {
        return tok_eof;
    }

    int thisChar {lastChar};
    lastChar = std::getchar();

    return thisChar;
}
