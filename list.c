#include "list.h"

#ifndef GENERIC_LIST
#define GENERIC_LIST(TYPE, TYPE_NAME)                                           \
                                                                                \
    void TYPE_NAME##ListInit(TYPE_NAME##List* list) {                           \
        list->first = NULL;                                                     \
        list->last = NULL;                                                      \
        list->active = NULL;                                                    \
        list->len = 0;                                                          \
    }                                                                           \
                                                                                \
    TYPE_NAME##Elem* TYPE_NAME##ListGetFirst(TYPE_NAME##List* list){            \
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
    }                                                                           \
    void TYPE_NAME##ListFirst(TYPE_NAME##List* list){                           \
        list->active = list->first;                                             \
    }                                                                           \
    void TYPE_NAME##ListNext(TYPE_NAME##List* list){                            \
        if(!list->active){                                                      \
            return;                                                             \
        }                                                                       \
        list->active = list->active->next;                                      \
    }                                                                           \
    TYPE TYPE_NAME##ListGetValue(TYPE_NAME##List* list){                        \
        return list->active->data;                                              \
    }

GENERIC_LIST(char, char)
GENERIC_LIST(Token*, tok)

// Converts the charList to char*
char* charListToString(charList *l){
    char* str = malloc((l->len+1)*sizeof(char));
    charElem* iter = charListGetFirst(l);
    int i = 0;
    while(i < l->len){
        str[i] = iter->data;
        iter = iter->next;
        i++;
    }
    str[i] = '\0';
    return str;
}

#endif
