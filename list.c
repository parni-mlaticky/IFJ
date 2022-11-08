/**
 * @file list.c
 * @authors Vladimír Hucovič
 * @brief Implementation of list functions
*/

#include "list.h"
#include <stdlib.h>

// Converts char c into dataUnion
dataUnion charToUnion(char c){
    dataUnion d;
    d.c = c;
    return d;
}

// Converts Token* into dataUnion
dataUnion TokenToUnion(Token* t){
    dataUnion d;
    d.t = t;
    return d;
}

// Initializes a list with the specified type
void listInit(list* l, listType type){
    l->type = type;
    l->first = NULL;
    l->last = NULL;
    l->len = 0;
}

// Returns the first list element
listElem* listFirst(list* l){
    return l->first;
}

// Appends a new element to the list
void listAppend(list* l, dataUnion data){

    listElem* new = malloc(sizeof(listElem));

    if(l->type == CHARLIST){
        new->data.c = data.c;
    }
    if(l->type == TOKENLIST){
        new->data.t = data.t;
    }


    new->next = NULL;

    // If the list is empty
    if(!l->len){
        l->first = new;
        l->last = new;
        l->len++;
        return;
    }

    l->last->next = new;
    l->last = new;
    l->len++;
}

// Converts the charList to char*
char* listToString(list *l){
    if(l->type != CHARLIST){
        return NULL;
    }

    char* str = malloc((l->len+1)*sizeof(char));
    listElem* iter = listFirst(l);
    int i = 0;
    while(i < l->len){
        str[i] = iter->data.c;
        iter = iter->next;
        i++;
    }
    str[i] = '\0';
    return str;
}

// Destroys all elements in the list
void listDispose(list* l){
    listElem* iter;
    listElem* deleted;

    iter = l->first;
    while(iter){
        deleted = iter;
        iter = iter->next;
        free(deleted);
    }
    l->first = NULL;
    l->last = NULL;
    l->len = 0;
}

