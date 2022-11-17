/**
 * @file htab_size.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include "htab.h"
#include "htab_structs_definition.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define AVG_LEN_MIN 0.01
#define AVG_LEN_MAX 0.75

size_t htab_bucket_count(const htab_t* t){
    return t->arr_size;
} // velikost pole

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

size_t htab_hash_function(const char *str) {
    uint32_t h=0;     // musí mít 32 bitů
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++){
        h = 65599*h + *p;
    }
    return h;
}

htab_t *htab_init(size_t n){
    htab_t* ht = calloc(1, sizeof(htab_t));
    if(!ht){
        return NULL;
    }
    ht->arr_size = n;
    ht->size = 0;
    ht->arr_ptr = calloc(n, sizeof(htab_item*));
    if(!ht->arr_ptr){
        return NULL;
    }
    return ht;
}

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

size_t htab_size(const htab_t* t){
    return t->size;
}
