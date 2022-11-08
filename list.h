/**
 * @file list.h
 * @authors Vladimír Hucovič
 * @brief Header file for list type
*/

#include <string.h>
#include <stdio.h>
#include "token.h"

typedef enum listType{
    CHARLIST,
    TOKENLIST
} listType;

typedef union data{
    char c;
    Token* t;
} dataUnion;

typedef struct listElem{
    dataUnion data;
    struct listElem* next;
} listElem;

typedef struct list{
    listType type;
    listElem* first;
    listElem* last;
    int len;
} list;

void listInit(list *l, listType type);
listElem* listFirst(list* l);
void listAppend(list* l, dataUnion d);
char* listToString(list* l);
dataUnion TokenToUnion(Token* t);
dataUnion charToUnion(char c);
void listDispose(list* l);

