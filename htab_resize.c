/**
 * @file htab_resize.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"
#include <stdlib.h>

void htab_resize(htab_t* t, size_t newn){
    if(!t) {
        return;
    }

    if(newn <= 0){
        return;
    }

    htab_item** newArray = calloc(newn, sizeof(htab_item*));
    if(!newArray){
        return;
    }

    // tmp - iterator puvodniho pole
    // tmp2 - iterator noveho pole
    htab_item* tmp;
    htab_item* tmp2;

    int newIndex;
    int broken = 0;

    // projde starou tabulku, rehashuje zaznamy a premisti ukazatele do noveho pole
    for (size_t i = 0; i < t->arr_size; ++i) {
        tmp = t->arr_ptr[i];
        while(tmp){
            broken = 0;
            newIndex = htab_hash_function(tmp->pair.key) % newn;
            tmp2 = newArray[newIndex];
            if(tmp2 == NULL){
                newArray[newIndex] = tmp;
                if(tmp->next){
                    tmp = tmp->next;
                    newArray[newIndex]->next = NULL;
                    continue;
                }
            }
            else{
                while(tmp2){
                    if(!tmp2->next){
                        tmp2->next = tmp;
                        if(tmp->next){
                            tmp = tmp->next;
                            tmp2->next->next = NULL;
                            broken = 1;
                            break;
                        }
                         break;
                    }
                    tmp2 = tmp2->next;
                }
            }
            if(!broken){
                tmp = tmp->next;
            }
        }
    }
    free(t->arr_ptr);
    t->arr_size = newn;
    t->arr_ptr = newArray;
}
