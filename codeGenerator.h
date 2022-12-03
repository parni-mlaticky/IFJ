#pragma once
#include "Nonterminal.h"
#include "hashtable.h"
#include "funcall.h"
#include "list.h"
#include "semanticError.h"

void generateExpressionCode(Nonterminal* root, bool isLeftSideOfAssignment, ht_table_t* symtable);
void generateToBoolFunction();
void generateToFloatFunction();
void generateEnforceTypesFunction();
void generateStackSwapFunction();
void generateNormalizeNumericTypesFunction();
void generateNullToIntFunction();
void generateStarterAsm();
void generateBuiltInFunctions();
void generateCompareDtypes();
void generateEquality();
void generateNonEquality();
void generateLess();
void generateGreat();
void generateLessEqual();
void generateGreatEqual();
void generateEmptyStringToInt();
void defineFunctionVars(ht_table_t symtable);
