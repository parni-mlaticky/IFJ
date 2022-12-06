#pragma once
#include "Nonterminal.h"
#include "hashtable.h"
#include "funcall.h"
#include "list.h"
#include "semanticError.h"

/**
 * @brief generates code for expression
 * 
 * @param root root node of the expression
 * @param isLeftSideOfAssignment left side of assigment
 * @param localSymtable local symtable
 * @param globalSymtable global symtable
 */
void generateExpressionCode(Nonterminal *root, bool isLeftSideOfAssignment, ht_table_t *localSymtable,
                             ht_table_t* globalSymtable);

void generateToBoolFunction();
void generateToFloatFunction();
void generateEnforceTypesFunction();
void generateStackSwapFunction();
void generateNormalizeNumericTypesFunction();
void generateNullToIntFunction();
void generateStarterAsm();
void generateBuiltInFunctions();
void generateCompareDtypes();
void generateCheckIfIsType();
void generateCheckIfIsTypeOrNull();
void generateEquality();
void generateNonEquality();
void generateLess();
void generateGreat();
void generateLessEqual();
void generateGreatEqual();
void generateEmptyStringToInt();
void generateNormalizeTypes();
void generateNullToString();
void generateRelationTypecast();
void defineFunctionVars(ht_table_t symtable);

/**
 * @brief count number of escape sequences in a string
 * 
 * @param string string to check
 * @return int number of sequences
 */
int countEscapeSequences(char *string);

/**
 * @brief converts enym dataType to string
 * 
 * @param dType dataType
 * @return char* converted string
 */
char* enumTypeToStr(dataType dType);
