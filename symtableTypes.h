#pragma once

#include "list.h"
#include "variable.h"


typedef struct{
    dataType returnType;
    varList* args;
    struct symtableElem* localTable;
} function;


typedef struct symtableElem{
    enum type {
        FUNCTION,
        VARIABLE
    };

    union{
        variable* v;
        function* f;
    };
} symtableElem;


