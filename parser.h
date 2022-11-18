#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include <stdbool.h>
#include "list.h"
#include "scanner.h"
#include "list.h"

bool parse_file(FILE* file);

bool precParser(tokList* tl);

bool isKeyword(Token* t);

void syntaxError();


Token* getNextToken(tokList* tl);

bool compareLexTypes(Token* inputTerminal, Lex stackTerminal);

bool compareTerminalStrings(Token* inputTerminal, char* stackTerminal);

bool recursiveDescent(tokList* tl);


bool progExpansion(tokList* tl);
bool declareStExpansion(tokList* tl);

bool STListExpansion(tokList* tl);

bool STExpansion(tokList* tl);

bool endTokenExpansion(tokList* tl);


bool functionDefStExpansion(tokList* tl);

bool ifStExpansion(tokList* tl);

bool whileStExpansion(tokList* tl);

bool returnStExpansion(tokList* tl);

bool varExpansion(tokList* tl);

bool paramsExpansion(tokList* tl);

bool paramListExpansion(tokList* tl);

bool typeExpansion(tokList* tl);


bool typeNameExpansion(tokList* tl);
bool blockExpansion(tokList* tl);
#endif
