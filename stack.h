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

typedef struct stackData{
    elemType etype;
    terminalType tType;
    Token* token; 
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
void stackPushNonterminal(stack* s);
stackElement* findHandle(stack* s);
void stackMultiPop(stack* s, int count);
void reduction(stack* s);