#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "token.h"

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
    INT,
    FLOAT,
    STRING,
    UNKNOWN
} dataType;

typedef struct Nonterminal{
    dataType dType; // contains data type of the NT if we can get it in compile-time, otherwise it's unknown
    bool isLValue; // true if the nonterminal expands to VAR_ID
    union { // value of terms, or expressions evaluated in compile-time
        char* stringV;
        int integerV;
        double floatV;
    };

    // If this nonterminal is an expression, here are the pointers to the nonterminals it contains and the operator
    struct {
        struct Nonterminal* nt1;
        struct Nonterminal* nt2;
        terminalType op;
    };

} Nonterminal;

typedef struct stackData{
    elemType etype;
    terminalType tType;
    union{
        Token* token;
        Nonterminal* nonterminal;
    };

} stackData;

typedef struct stackElement{
    struct stackElement* prev;
    stackData data;
    struct stackElement* next;
} stackElement;


typedef struct stack{
    stackElement* top;
} stack;

bool containsOnlyTopNonterm(stack* s);
void stackInit(stack* s);
void stackPushTerminal(stack* s, terminalType t, Token* token);
void stackPop(stack* s);
stackElement* stackPeek(stack* s);
void stackDispose(stack* s);
bool stackIsEmpty(stack* s);
stackElement* getTopTerminal(stack* s);
void stackInsertHandle(stack* s);
void stackPushNonterminal(stack* s, Nonterminal* nt, stackElement* elem);
stackElement* findHandle(stack* s);
void stackMultiPop(stack* s, int count);
void reduction(stack* s);