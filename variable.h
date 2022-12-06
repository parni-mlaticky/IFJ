#pragma once
#include <stdbool.h>
#include "precParsingEnums.h"

typedef struct variable{
    char* name;
    dataType dType;
    bool nullable;
} variable;

/**
 * @brief creates a clone of a variable
 * 
 * @param var variable to clone
 * @return variable* variable clone
 */
variable* variable_clone(variable* var);
