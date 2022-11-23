#include "stack.h"
#include "string.h"



void stackInit(stack* s){
    s->top = NULL;
}

bool containsOnlyTopNonterm(stack* s){
    if(!s->top) return false;
    if(s->top->data.etype == NONTERMINAL && s->top->next){
        if(s->top->next->data.etype == TERMINAL && s->top->next->data.tType == DOLLAR){
            return true;
        }
    }
    return false;
}

void stackPushTerminal(stack* s, terminalType t, Token* token){
    stackElement* newElement = malloc(sizeof(stackElement));
    newElement->data.etype = TERMINAL;
    newElement->data.tType = t;
    if(t == OP){
        newElement->data.token = token;
    }
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

void stackPushNonterminal(stack* s, Nonterminal* nt, stackElement* elem){
    stackElement* newElement = malloc(sizeof(stackElement));
    newElement->data.etype = NONTERMINAL;
    if(!nt){
        if(!elem) exit(99);
        Nonterminal* newNonterminal = malloc(sizeof(Nonterminal));
        newNonterminal->isLValue = false;
        switch(elem->data.token->lex){
            case INT_LIT:
                newNonterminal->dType = INT;
                newNonterminal->integerV = elem->data.token->integer;
                break;
            case STRING_LIT:
                newNonterminal->dType = STRING;
                newNonterminal->stringV = elem->data.token->string;
                break;
            case FLOAT_LIT:
                newNonterminal->dType = FLOAT;
                newNonterminal->floatV = elem->data.token->decimal;
                break;
            case FUN_ID:
                // later when we have symtable it will instead be the return value of the function in case of FUN_ID
                // and the data type of the variable in case of VAR_ID TODO
                newNonterminal->dType = UNKNOWN;
                break;
            case VAR_ID:
                newNonterminal->dType = UNKNOWN;
                newNonterminal->isLValue = true;    
                break;
            default: exit(99);
        }
        newElement->data.nonterminal = newNonterminal;
    } // if(!nt)
    else{
        newElement->data.nonterminal = nt;
    }
    newElement->data.tType = 0;
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
    if(s->top->data.etype == TERMINAL && s->top->data.tType == DOLLAR){
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
    new->data.tType = 0;

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