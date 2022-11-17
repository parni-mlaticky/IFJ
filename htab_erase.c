/**
 * @file htab_erase.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"
#include <string.h>
#include <stdlib.h>

#define AVG_LEN_MIN 0.01

bool htab_erase(htab_t * t, htab_key_t key){
    bool returnValue = false;
    htab_item* tmp;
    htab_item* tmp2;
    size_t index = htab_hash_function(key) % t->arr_size;
    tmp = t->arr_ptr[index];
    if(tmp){
        if (!strcmp(tmp->pair.key, key)) {
            if(tmp->next){
                tmp2 = t->arr_ptr[index]->next;
                free((char*) t->arr_ptr[index]->pair.key);
                free(t->arr_ptr[index]);
                t->size--;
                t->arr_ptr[index] = tmp2;
                returnValue = true;
            }
            if(!returnValue){
                free((char*) t->arr_ptr[index]->pair.key);
                free(t->arr_ptr[index]);
                t->size--;
                t->arr_ptr[index] = NULL;
                returnValue = true;
            }
        }
    }

    if(!returnValue) {
        tmp2 = t->arr_ptr[index];
        while (tmp) {
            if (!strcmp(tmp->pair.key, key)) {
                if (tmp->next) {
                    tmp2->next = tmp->next;
                    free((char *) tmp->pair.key);
                    free(tmp);
                    t->size--;
                    tmp = NULL;
                    returnValue = true;
                    break;
                }
                free((char *) tmp->pair.key);
                free(tmp);
                t->size--;
                tmp2->next = NULL;
                tmp = NULL;
                returnValue = true;
                break;
            }
            tmp2 = tmp;
            tmp = tmp->next;
        }
    }
    if(returnValue){
        double avgBucketLen = t->size / t->arr_size;
        if(avgBucketLen < AVG_LEN_MIN){
            htab_resize(t, t->arr_size/2);
        }
    }
    return returnValue;
}    // ruší zadaný záznam
