/**
 * @file htab_strcuts_definition.h
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#ifndef LIBHTAB_HTAB_STRUCTS_DEFINITION_H
#define LIBHTAB_HTAB_STRUCTS_DEFINITION_H
#include "htab.h"

typedef struct htab_item{
    struct htab_item* next;
    htab_pair_t pair;
} htab_item;

typedef struct htab{
        unsigned long size;
        unsigned long arr_size;
        htab_item** arr_ptr;
} htab_t;

#endif //LIBHTAB_HTAB_STRUCTS_DEFINITION_H
