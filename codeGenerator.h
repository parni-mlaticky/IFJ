#pragma once
#include "Nonterminal.h"
#include "hashtable.h"
#include "funcall.h"
#include "list.h"
#include "semanticError.h"

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
void defineFunctionVars(ht_table_t symtable);
int countEscapeSequences(char *string);
char* enumTypeToStr(dataType dType);
