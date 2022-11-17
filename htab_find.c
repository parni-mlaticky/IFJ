/**
 * @file htab_find.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"

htab_pair_t * htab_find(htab_t * t, htab_key_t key) {
    if (!t) {
        return NULL;
    }

    // tmp - iterator tabulky
    htab_item *tmp;
    tmp = t->arr_ptr[htab_hash_function(key) % t->arr_size];
    while (tmp) {
        if (!strcmp(tmp->pair.key, key)) {
            return &(tmp->pair);
        }
        tmp = tmp->next;
    }

    return NULL;
}  // hledání

