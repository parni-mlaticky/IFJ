/**
 * @file htab_clear.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"
#include <stdlib.h>

void htab_clear(htab_t * t){
    if(!t){
        return;
    }

    htab_item* tmp;
    htab_item* tmp2;
    for(unsigned long i = 0; i < t->arr_size; ++i){
        tmp = t->arr_ptr[i];

        while(tmp){
            tmp2 = tmp;
            tmp = tmp->next;
            free((char*) tmp2->pair.key);
            free(tmp2);
        }
        if(t->arr_ptr[i]){
            t->arr_ptr[i] = NULL;
        }
    }
    t->size = 0;
}    // ruší všechny záznamy

