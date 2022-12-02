#pragma once
#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"
#include "token.h"
#include "scanner.h"
#include "stack.h"
#include "precParsingEnums.h"
#include "codeGenerator.h"
#include "misc.h"
#include "variable.h"


#define KEYWORD_COUNT 11

typedef enum{
    P_LESS,
    P_GREATER,
    P_EQ
} opPrecedence;

typedef enum{
    EXP_TERM,
    EXP_OP_EXP,
    EXP_PAR
} expressionRule;


bool firstPass(tokList* tl);
bool parse_file(FILE* file);
bool isKeyword(Token* t);

void syntaxError(Token* errorToken, char* errMessage);

terminalType lexEnumToTerminalEnum(Lex lex);

opPrecedence getPrecedence(terminalType stackTerm, Lex nextTerm);

Token* getNextToken(tokList* tl);

bool compareLexTypes(Token* inputTerminal, Lex stackTerminal);

bool compareTerminalStrings(Token* inputTerminal, char* stackTerminal);

bool recursiveDescent(tokList* tl);

dataType dataTypeCompatibilityCheckOrConversion(Nonterminal* nt1, stackElement* operator, Nonterminal* nt2);

void processPossibleVariableDefinition(Nonterminal* expTree, ht_table_t* symtable);

bool progExpansion(tokList* tl);
bool declareStExpansion(tokList* tl);

bool STListExpansion(tokList* tl);

bool STExpansion(tokList* tl);

bool endTokenExpansion(tokList* tl);

bool blockSTListExpansion(tokList* tl, function* func);

bool blockSTExpansion(tokList* tl, function* func);

bool functionDefStExpansion(tokList* tl, bool firstPass);

bool ifStExpansion(tokList* tl, function* func);

bool whileStExpansion(tokList* tl, function* func);

bool returnStExpansion(tokList* tl, function* func);

bool varExpansion(tokList* tl, variable* var);

bool paramsExpansion(tokList* tl, varList* args);

bool paramListExpansion(tokList* tl, varList* args);

bool typeExpansion(tokList* tl, dataType* returnType, bool* nullable, bool isReturnType);

bool typeNameExpansion(tokList* tl, bool questionMark, dataType* returnType, bool isReturnType);
bool blockExpansion(tokList* tl, function* func);

bool isRelOperator(terminalType tType);

void debugPrintExprTree(Nonterminal* root);

void terminalToDataType(Token* t, dataType* type);

bool precParser(tokList* tl, Nonterminal** finalNonterm);

Nonterminal* createIntLiteralNonterminal(int value);

Nonterminal* createStringLiteralNonterminal(char* string);

Nonterminal* createFloatLiteralNonterminal(double value);

Nonterminal* createFuncallNonterminal(char* funId, nontermList* args);

Nonterminal* createVariableNonterminal(char* varId, dataType dType);

Nonterminal* createExprNonterminal(Nonterminal* left, Nonterminal* right, terminalType operator);
Nonterminal* createNullNonterminal();
#endif
