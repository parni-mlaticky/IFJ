#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include <stdbool.h>
#include "list.h"
#include "scanner.h"
#include "stack.h"

#define KEYWORD_COUNT 9

typedef enum{
    P_LESS,
    P_GREATER,
    P_EQ
} opPrecedence;

bool parse_file(FILE* file);

bool precParser(tokList* tl);

bool isKeyword(Token* t);

void semanticError(int code);
void syntaxError();

terminalType lexEnumToTerminalEnum(Lex lex);

opPrecedence getPrecedence(terminalType stackTerm, Lex nextTerm);

Token* getNextToken(tokList* tl);

bool compareLexTypes(Token* inputTerminal, Lex stackTerminal);

bool compareTerminalStrings(Token* inputTerminal, char* stackTerminal);

bool recursiveDescent(tokList* tl);

dataType dataTypeCompatibilityCheckOrConversion(Nonterminal* nt1, stackElement* operator, Nonterminal* nt2);

bool progExpansion(tokList* tl);
bool declareStExpansion(tokList* tl);

bool STListExpansion(tokList* tl);

bool STExpansion(tokList* tl);

bool endTokenExpansion(tokList* tl);

bool blockSTListExpansion(tokList* tl);

bool blockSTExpansion(tokList* tl);

bool functionDefStExpansion(tokList* tl);

bool ifStExpansion(tokList* tl);

bool whileStExpansion(tokList* tl);

bool returnStExpansion(tokList* tl);

bool varExpansion(tokList* tl);

bool paramsExpansion(tokList* tl);

bool paramListExpansion(tokList* tl);

bool typeExpansion(tokList* tl);


bool typeNameExpansion(tokList* tl, bool questionMark);
bool blockExpansion(tokList* tl);

bool isRelOperator(terminalType tType);
#endif
