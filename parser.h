#pragma once
#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdlib.h>

#include "codeGenerator.h"
#include "hashtable.h"
#include "misc.h"
#include "precParsingEnums.h"
#include "scanner.h"
#include "stack.h"
#include "token.h"
#include "variable.h"

#define KEYWORD_COUNT 11

typedef enum {
    P_LESS,
    P_GREATER,
    P_EQ
} opPrecedence;

typedef enum {
    EXP_TERM,
    EXP_OP_EXP,
    EXP_PAR
} expressionRule;

/**
 * @brief passes through the whole source code in order to check for
 * function definition syntax errors, to fill up the symtable
 *
 * @param tl list of all tokens
 * @return true if no syntax error occurred, exits otherwise
 */
bool firstPass(tokList* tl);

/**
 * @brief parses source code into list of tokens, inits symtable,
 * generates starter ASM code
 *
 * @param file PHP source code (stdin)
 * @return true if firstPass, recursive descent occurred without any errors
 */
bool parse_file(FILE* file);

/**
 * @brief checks if token is keyword
 *
 * @param t token
 * @return true if token is keyword, false otherwise
 */
bool isKeyword(Token* t);

/**
 * @brief exits the program with specified error message on syntax error
 *
 * @param errorToken token, that caused the error
 * @param errMessage error message
 */
void syntaxError(Token* errorToken, char* errMessage);

/**
 * @brief converts lex enumerator to terminal enumerator
 *
 * @param lex lex to convert to enum
 * @return terminalType converted lex
 */
terminalType lexEnumToTerminalEnum(Lex lex);

/**
 * @brief Gets the precedence of next lex to term on stack
 *
 * @param stackTerm terminal on stack
 * @param nextTerm next terminal
 * @return opPrecedence
 */
opPrecedence getPrecedence(terminalType stackTerm, Lex nextTerm);

/**
 * @brief Gets the next token from the token list
 *
 * @param tl list of all tokens
 * @return Token* pointer to the next token
 */
Token* getNextToken(tokList* tl);

/**
 * @brief checks if stack terminal equals to input terminal
 *
 * @param inputTerminal input terminal
 * @param stackTerminal stack temrminal
 * @return true if input terminal equals to stack terminal, false otherwise
 */
bool compareLexTypes(Token* inputTerminal, Lex stackTerminal);

/**
 * @brief checks the lex type of stack and input terminal
 *
 * @param inputTerminal input terminal
 * @param stackTerminal stack terminal
 * @return true if strings equal, false otherwise
 */
bool compareTerminalStrings(Token* inputTerminal, char* stackTerminal);

/**
 * @brief descends into program expansion
 *
 * @param tl list of all tokens
 * @return true if programExpansion returns true, false otherwise
 */
bool recursiveDescent(tokList* tl);

/**
 * @brief processes variable definitions, inserts them into the symtable
 *
 * @param expTree expression tree
 * @param symtable table of symbols
 */
void processPossibleVariableDefinition(Nonterminal* expTree, ht_table_t* symtable);

/**
 * @brief expands the whole program into script start, statements, end token
 *
 * @param tl list of all tokens
 * @return true if program expansion doesnt run into any error
 */
bool progExpansion(tokList* tl);

/**
 * @brief expands statement expansion
 *
 * @param tl list of all tokens
 * @return true if statements are valid, false otherwise
 */
bool declareStExpansion(tokList* tl);

/**
 * @brief expands statement list
 *
 * @param tl list of all tokens
 * @return true if expands as expected
 */
bool STListExpansion(tokList* tl);

/**
 * @brief expands statements
 *
 * @param tl list of all tokens
 * @return true if expands as expected
 */
bool STExpansion(tokList* tl);

/**
 * @brief check if token expands to script stop or end token
 *
 * @param tl list of all tokens
 * @return true if expands as expected
 */
bool endTokenExpansion(tokList* tl);

/**
 * @brief checks if block statement expands to script stop, end token,
 * right curly brace (recursively)
 *
 * @param tl list of all tokens
 * @param func function
 * @return true if expands as expected
 */
bool blockSTListExpansion(tokList* tl, function* func);

bool blockSTExpansion(tokList* tl, function* func);

/**
 * @brief expands function definition, generates code for it
 *
 * @param tl list of all tokens
 * @return true if expands as expected
 */
bool functionDefStExpansion(tokList* tl);

/**
 * @brief expands if statement, generates code for it
 *
 * @param tl list of all tokens
 * @param func function
 * @return true if expands as expected
 */
bool ifStExpansion(tokList* tl, function* func);

/**
 * @brief expands while statement, generates code for it
 *
 * @param tl list of all tokens
 * @param func function
 * @return true if expands as expected
 */
bool whileStExpansion(tokList* tl, function* func);

/**
 * @brief expands return statement, generates code for it
 *
 * @param tl list of all tokens
 * @param func function
 * @return true if expands as expected
 */
bool returnStExpansion(tokList* tl, function* func);

/**
 * @brief expands variable
 *
 * @param tl list of all tokens
 * @param var variable
 * @return true if expands as expected
 */
bool varExpansion(tokList* tl, variable* var);

/**
 * @brief expands params
 *
 * @param tl list of all tokens
 * @param args list of parameter arguments
 * @return true if expands as expected
 */
bool paramsExpansion(tokList* tl, varList* args);

/**
 * @brief expands list of parameters
 *
 * @param tl list of all tokens
 * @param args list of parameter arguments
 * @return true if expands as expected
 */
bool paramListExpansion(tokList* tl, varList* args);

/**
 * @brief expands type (of input parameter or function return)
 *
 * @param tl lit of all tokens
 * @param returnType return type
 * @param nullable if dataType is nullable
 * @param isReturnType if type of function return
 * @return true if typeName expansion returns true
 */
bool typeExpansion(tokList* tl, dataType* returnType, bool* nullable, bool isReturnType);

/**
 * @brief
 *
 * @param tl list of all tokens
 * @param questionMark question mark before type name
 * @param returnType pointer to variable to set the type to
 * @param isReturnType if returned type is type, that function returns
 * @return true if expands as expected
 */
bool typeNameExpansion(tokList* tl, bool questionMark, dataType* returnType, bool isReturnType);

/**
 * @brief
 *
 * @param tl list of all tokens
 * @param func function
 * @return true if expands as expected
 */
bool blockExpansion(tokList* tl, function* func);

/**
 * @brief tells if terminal is a relation operator
 *
 * @param tType type of terminal
 * @return true if terminal is relation operator
 */
bool isRelOperator(terminalType tType);

/**
 * @brief print of expression tree for debug
 *
 * @param root root of all nonterminals
 */
void debugPrintExprTree(Nonterminal* root);

void terminalToDataType(Token* t, dataType* type);

/**
 * @brief 
 * 
 * @param tl list of all tokens
 * @param finalNonterm variable to return the final nonterminal to
 * @param isFuncArg is function argument
 * @return true if parsing passes without any errors
 */
bool precParser(tokList* tl, Nonterminal** finalNonterm, bool isFuncArg);

/**
 * @brief creates an integer literal nonterminal
 *
 * @param value value of the created integer
 * @return integer nonterminal
 */
Nonterminal* createIntLiteralNonterminal(int value);

/**
 * @brief adds built-in php functions to the symtable
 *
 */
void addBuiltinFunctionsToSymtable();

/**
 * @brief creates a string literal nonterminal
 *
 * @param string value of the created string
 * @return string nonterminal
 */
Nonterminal* createStringLiteralNonterminal(char* string);

/**
 * @brief creates a float literal nonterminal
 *
 * @param string value of the created float
 * @return float nonterminal
 */
Nonterminal* createFloatLiteralNonterminal(double value);

/**
 * @brief creates a function call nonterminal
 *
 * @param funId id of the function to create
 * @param args arguments of the function
 * @return function call nonterminal
 */
Nonterminal* createFuncallNonterminal(char* funId, nontermList* args);

/**
 * @brief creates a variable nonterminal
 *
 * @param varId id of the function to create
 * @param dType data type of the variable
 * @return variable nonterminal
 */
Nonterminal* createVariableNonterminal(char* varId, dataType dType);

/**
 * @brief creates an expression nonterminal
 *
 * @param left left part of the expression
 * @param right right part of the expression
 * @param operator expression operator
 * @return expression nonterminal
 */
Nonterminal* createExprNonterminal(Nonterminal* left, Nonterminal* right, terminalType operator);

// TODO: delete?
void compileTimeExpressionEval(Nonterminal* expTree);

/**
 * @brief creates a null nonterminal
 *
 * @return null nonterminal
 */
Nonterminal* createNullNonterminal();
#endif
