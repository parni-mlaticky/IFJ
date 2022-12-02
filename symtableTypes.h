#pragma once

#include "list.h"
#include "variable.h"

typedef struct{
  char* functionName;
    dataType returnType;
    bool nullable;
    varList* args;
    struct symtableElem* localTable;
} function;


typedef struct symtableElem{
    enum type {
        FUNCTION,
        VARIABLE
    } type;

    union{
        variable* v;
        function* f;
    };
} symtableElem;

