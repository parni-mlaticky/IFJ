/**
 * @file stack.h
 * @author Vladimír Hucovič
 * @brief header file for stack structure, functions
 * 
 */
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

/**
 * @brief check if stack contains only top terminal
 * 
 * @param s stack
 * @return true if contains only top terminal, false otherwise
 */
bool containsOnlyTopNonterm(stack* s);

/**
 * @brief inits the stack
 * 
 * @param s stack to init
 */
void stackInit(stack* s);

/**
 * @brief pushes token into the stack
 * 
 * @param s stack to push to
 * @param t terminal type
 * @param token token to push
 */
void stackPushTerminal(stack* s, terminalType t, Token* token);

/**
 * @brief pushes function terminal into the stack
 * 
 * @param s stack to push to
 * @param token token to push to
 * @param args function args
 */
void stackPushFuncallTerminal(stack* s, Token* token, nontermList* args);

/**
 * @brief pops the stack
 * 
 * @param s stack to pop
 */
void stackPop(stack* s);

/**
 * @brief returns element at the top of the stack, without popping it
 * 
 * @param s stack to peek
 * @return stackElement* element at the top
 */
stackElement* stackPeek(stack* s);

/**
 * @brief disposes the whole stack
 * 
 * @param s stack to dispose
 */
void stackDispose(stack* s);

/**
 * @brief tells if the stack is empty
 * 
 * @param s stack
 * @return true if stack is empty, false otherwise
 */
bool stackIsEmpty(stack* s);

/**
 * @brief gets the terminal at the top of the stack
 * 
 * @param s stack to get the item from
 * @return stackElement* top terminal if found, null otherwise
 */
stackElement* getTopTerminal(stack* s);

/**
 * @brief inserts handle into the stack
 * 
 * @param s stack to insert into
 */
void stackInsertHandle(stack* s);

/**
 * @brief pushes a nonterminal into the stack
 * 
 * @param s stack to push to
 * @param nonterm nonterminal to push
 */
void stackPushNonterminal(stack* s, Nonterminal* nonterm);

/**
 * @brief finds handle in the stack
 * 
 * @param s stack to search in
 * @return stackElement* handle if found, null otherwise
 */
stackElement* findHandle(stack* s);

/**
 * @brief pops multiple times from the stack
 * 
 * @param s stack to pop
 * @param count times to pop the stack
 */
void stackMultiPop(stack* s, int count);
