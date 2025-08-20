#ifndef MAIN_H
#define MAIN_H

#include "parser.hpp"

#include <iostream>
#include <cstdio>

void loop();
void handleTopLevelExpression();
void handleExtern();
void handleDefinition();

#endif
