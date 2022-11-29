#pragma once
#include <stdbool.h>
#include "precParsingEnums.h"

typedef struct variable{
    char* name;
    dataType dType;
    bool nullable;
} variable;

variable* variable_clone(variable* var);
