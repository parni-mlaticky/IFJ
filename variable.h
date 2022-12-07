/**
 * @file variable.h
 * @author Vladimír Hucovič
 * @brief header file for variable structure, functions for symtable
 * 
 */
#pragma once
#include <stdbool.h>
#include "precParsingEnums.h"

typedef struct variable{
    char* name;
    dataType dType;
    bool nullable;
} variable;

/**
 * @brief copies a given variable into a newly allocated memory
 * 
 * @param var variable to clone
 * @return variable* variable clone
 */
variable* variable_clone(variable* var);
