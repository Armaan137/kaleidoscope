#include "main.hpp"

int main() {

    // insert the operators.
    binaryPrecedence['<'] = 10; // lowest.
    binaryPrecedence['+'] = 20;
    binaryPrecedence['-'] = 30;
    binaryPrecedence['*'] = 40; // highest.

    return 1;
}
