/**
 * @file htab_free.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"
#include <stdlib.h>

void htab_free(htab_t * t){
    if(!t){
        return;
    }

    // tmp, tmp2 - iteratory tabulky.
    // tmp je pouzito pro uchovani zaznamu (.next) za prave uvolnenym zaznamem
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
    }
    free(t->arr_ptr);
    free(t);
}
// destruktor tabulky
