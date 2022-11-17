/**
 * @file htab_for_each.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"

void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)){

    // tmp - iterator tabulky
    htab_item* tmp;
    for(unsigned long i = 0; i < t->arr_size; ++i){
        tmp = t->arr_ptr[i];
        while(tmp){
            (*f) (&(tmp->pair));
            tmp = tmp->next;
        }
    }
}
