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

typedef enum NonterminalType{
    LITERAL_TERM,
    VAR_ID_TERM,
    FUNCALL_TERM,
    EXPR
} NonterminalType;

typedef enum dataType{
    INT,
    FLOAT,
    STRING,
    UNKNOWN
} dataType;

