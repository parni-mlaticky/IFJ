#pragma once

typedef enum elemType{
    TERMINAL,
    NONTERMINAL,
    HANDLE
} elemType;

typedef enum terminalType{
    DOLLAR,
    OP,
    LPAR,
    RPAR,
    PLUS,
    MINUS,
    DIV,
    MUL,
    AS,
    EQ,
    NEQ,
    GEQ,
    LEQ,
    G,
    L,
    CAT
} terminalType;

typedef enum dataType{
    NULL_T,
    INT,
    FLOAT,
    STRING,
    UNKNOWN
} dataType;

