/**
 * @file list.h
 * @authors Vladimír Hucovič
 * @brief Macro definition for a generic list type
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "token.h"


#define GENERIC_LIST(TYPE, TYPE_NAME)                                           \
    typedef struct TYPE_NAME##Elem{                                             \
        TYPE data;                                                              \
        struct TYPE_NAME##Elem* next;                                           \
    } TYPE_NAME##Elem;                                                          \
                                                                                \
    typedef struct TYPE_NAME##List{                                             \
        TYPE_NAME##Elem* first;                                                 \
        TYPE_NAME##Elem* last;                                                  \
        int len;                                                                \
    } TYPE_NAME##List;                                                          \
                                                                                \
    void TYPE_NAME##ListInit(TYPE_NAME##List* list) {                           \
        list->first = NULL;                                                     \
        list->last = NULL;                                                      \
        list->len = 0;                                                          \
    }                                                                           \
                                                                                \
    TYPE_NAME##Elem* TYPE_NAME##ListFirst(TYPE_NAME##List* list){               \
        return list->first;                                                     \
    }                                                                           \
    void TYPE_NAME##ListAppend(TYPE_NAME##List* list, TYPE data){               \
        TYPE_NAME##Elem* new = malloc(sizeof(TYPE_NAME##Elem));                 \
        new->data = data;                                                       \
        new->next = NULL;                                                       \
        if(!list->len){                                                         \
            list->first = new;                                                  \
            list->last = new;                                                   \
            list->len++;                                                        \
            return;                                                             \
        }                                                                       \
        list->last->next = new;                                                 \
        list->last = new;                                                       \
        list->len++;                                                            \
    }                                                                           \
    void TYPE_NAME##ListDispose(TYPE_NAME##List* list){                         \
        TYPE_NAME##Elem* iter;                                                  \
        TYPE_NAME##Elem* deleted;                                               \
        iter = list->first;                                                     \
        while(iter){                                                            \
            deleted = iter;                                                     \
            iter = iter->next;                                                  \
            free(deleted);                                                      \
        }                                                                       \
        list->first = NULL;                                                     \
        list->last = NULL;                                                      \
        list->len = 0;                                                          \
                                                                                \
    }



GENERIC_LIST(Token*, tok);
GENERIC_LIST(char, char);


// Converts the charList to char*
char* charListToString(charList *l){
    char* str = malloc((l->len+1)*sizeof(char));
    charElem* iter = charListFirst(l);
    int i = 0;
    while(i < l->len){
        str[i] = iter->data;
        iter = iter->next;
        i++;
    }
    str[i] = '\0';
    return str;
}