/**
 * @file htab_lookup_add.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"
#include <string.h>
#include <stdlib.h>

#define AVG_LEN_MAX 0.75

htab_pair_t* htab_lookup_add(htab_t* t, htab_key_t key){
    if(!t){
        return NULL;
    }

    // tmp - iterator prvku tabulky
    htab_item* tmp;

    // projde bucket a zjisti, jestli se tam zaznam s danym klicem jiz nenachazi
    tmp = t->arr_ptr[htab_hash_function(key) % t->arr_size];
    while(tmp){
        if(!strcmp(tmp->pair.key, key)){
            return &(tmp->pair);
        }
        tmp = tmp->next;
    }

    // je vytvorena kopie klice, aby bylo mozne pouzit i char ze stacku
    char* keyCopy = calloc(strlen(key)+1, sizeof(char));
    if(!keyCopy){
        return NULL;
    }

    // vytvori novy item, zjisti hash, ulozi ukazatel do tabulky
    htab_item* newItem = calloc(1, sizeof(htab_item));
    if(!newItem){
        return NULL;
    }

    // pokud by pridani dalsiho itemu zpusobilo zvyseni prumerne delky bucketu nad definovane maximum, provede se resize
    double avgBucketLen = (t->size+1) / t->arr_size;
    if(avgBucketLen > AVG_LEN_MAX){
        htab_resize(t, t->arr_size*2);
    }

    strcpy(keyCopy, key);
    newItem->next = NULL;
    newItem->pair.key = keyCopy;
    newItem->pair.value = 0;


    // samotne pridani do tabulky
    size_t index = htab_hash_function(key) % t->arr_size;
    tmp = t->arr_ptr[index];
    if(!tmp){
        t->arr_ptr[index] = newItem;
        t->size++;
        return &(newItem->pair);
    }
    while(tmp) {
        if(!tmp->next){
            tmp->next = newItem;
            t->size++;
            return &(newItem->pair);
        }
        tmp = tmp->next;
    }
    return NULL;
}
