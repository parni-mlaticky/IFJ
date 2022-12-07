/**
 * @file list.h
 * @authors Vladimír Hucovič
 * @brief Macro definition for a generic list type
*/

#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "precParsingNonTerminal.h"


#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H(TYPE, TYPE_NAME)                              \
    typedef struct TYPE_NAME##Elem{                                  \
        TYPE data;                                                   \
        struct TYPE_NAME##Elem* next;                                \
        struct TYPE_NAME##Elem* prev;                                \
    } TYPE_NAME##Elem;                                               \
                                                                     \
    typedef struct TYPE_NAME##List{                                  \
        TYPE_NAME##Elem* active;                                     \
        TYPE_NAME##Elem* first;                                      \
        TYPE_NAME##Elem* last;                                       \
        int len;                                                     \
    } TYPE_NAME##List;                                               \
                                                                     \
    void TYPE_NAME##ListInit(TYPE_NAME##List* list);                 \
    TYPE_NAME##Elem* TYPE_NAME##ListGetFirst(TYPE_NAME##List* list); \
    void TYPE_NAME##ListAppend(TYPE_NAME##List* list, TYPE data);    \
    void TYPE_NAME##ListDispose(TYPE_NAME##List* list) ;             \
    void TYPE_NAME##ListFirst(TYPE_NAME##List* list);                \
    void TYPE_NAME##ListLast(TYPE_NAME##List* list);                 \
    void TYPE_NAME##ListNext(TYPE_NAME##List* list);                 \
    void TYPE_NAME##ListPrev(TYPE_NAME##List* list);                 \
    TYPE TYPE_NAME##ListGetValue(TYPE_NAME##List* list);             \


GENERIC_LIST_H(struct Nonterminal*, nonterm)
GENERIC_LIST_H(char, char)
GENERIC_LIST_H(Token*, tok)
GENERIC_LIST_H(variable, var)

char* charListToString(charList *l);

#endif
