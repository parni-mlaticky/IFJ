/**
 * @file htab_bucket_count.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"

size_t htab_bucket_count(const htab_t* t){
    return t->arr_size;
} // velikost pole
