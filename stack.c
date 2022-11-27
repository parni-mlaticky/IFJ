#include "stack.h"

void stackInit(stack* s){
    s->top = NULL;
}

bool containsOnlyTopNonterm(stack* s){
    if(!s->top) return false;
    if(s->top->data.etype == NONTERMINAL && s->top->next){
        if(s->top->next->data.etype == TERMINAL && s->top->next->data.terminal->tType == DOLLAR){
            return true;
        }
    }
    return false;
}

void stackPushTerminal(stack* s, terminalType tType, Token* token){
    stackElement* newElement = malloc(sizeof(stackElement));
    Terminal* terminal = malloc(sizeof(Terminal));
    terminal->tType = tType;
    terminal->token = token;
    newElement->data.terminal = terminal;
    newElement->data.etype = TERMINAL;
    if(!s->top){
        newElement->prev = NULL;
        newElement->next = NULL;
        s->top = newElement;
        return;
    }
    newElement->next = s->top;
    s->top->prev = newElement;
    newElement->prev = NULL;
    s->top = newElement;
}


void stackPushNonterminal(stack* s, Nonterminal* nonterm){
    stackElement* newElement = malloc(sizeof(stackElement));
    
    newElement->data.etype = NONTERMINAL;
    newElement->data.nonterminal = nonterm;

    newElement->next = s->top;
    s->top->prev = newElement;
    newElement->prev = NULL;
    s->top = newElement;
}

void stackPop(stack* s){
    stackElement* tmp = s->top;
    s->top = s->top->next;
    s->top->prev = NULL;
    free(tmp);
}

stackElement* stackPeek(stack* s){
    return s->top;
}

void stackDispose(stack* s){
    while(s->top){
        stackPop(s);
    }
}

bool stackIsEmpty(stack* s){
    if(s->top->data.etype == TERMINAL && s->top->data.terminal->tType == DOLLAR){
        return true;
    }
    return false;
}

stackElement* getTopTerminal(stack* s){
    stackElement* t = s->top;
    while(t->data.etype != TERMINAL){
        t = t->next;
    }
    return t;
}


stackElement* findHandle(stack* s){
    stackElement* t = s->top;
    while(t && t->data.etype != HANDLE){
        t = t->next;
    }
    if(!t) return NULL;
    if(t->data.etype != HANDLE){
        return NULL;
    }
    return t;
}

void stackMultiPop(stack* s, int count){
    for(int i = 0; i < count; i++){
        stackPop(s);
    }
}

void stackInsertHandle(stack* s){
    stackElement* termTop = getTopTerminal(s);
    stackElement* new = malloc(sizeof(stackElement));
    new->data.etype = HANDLE;
    new->data.terminal = NULL;

    if(termTop == s->top){
        new->next = s->top;
        new->prev = NULL;
        s->top = new;
        termTop->prev = new;
        return;
    }

    new->next = termTop;
    new->prev = termTop->prev;
    termTop->prev->next = new;
    termTop->prev = new;
}   