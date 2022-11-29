
#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "token.h"
#include "variable.h"
#include "string.h"
#include "precParsingEnums.h"
#include "Nonterminal.h"


typedef struct stackData{
    elemType etype;
    union{
        Terminal* terminal;
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
void stackPushFuncallTerminal(stack* s, Token* token, nontermList* args);

void stackPop(stack* s);
stackElement* stackPeek(stack* s);
void stackDispose(stack* s);
bool stackIsEmpty(stack* s);
stackElement* getTopTerminal(stack* s);
void stackInsertHandle(stack* s);
void stackPushNonterminal(stack* s, Nonterminal* nonterm);
stackElement* findHandle(stack* s);
void stackMultiPop(stack* s, int count);
void reduction(stack* s);

