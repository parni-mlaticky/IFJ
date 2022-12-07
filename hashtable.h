#pragma once

#ifndef IAL_HASHTABLE_H
#define IAL_HASHTABLE_H

#include <stdbool.h>
#include "variable.h"
#include "list.h"

/**
 * 
 *  hlavička hashovací tabulky převzata z předmětu IAL
 *
 */


// maximal size of hash table
#define MAX_HT_SIZE 101

// size of implemented table
extern int HT_SIZE;

typedef struct ht_item {
  char* key;            // pair key
  struct symtableElem* value;          // pair value
  struct ht_item *next; // pointer to next synonym
} ht_item_t;


typedef ht_item_t* ht_table_t[MAX_HT_SIZE];
typedef struct {
  char* functionName;         // name of function
    dataType returnType;      // function return type
    bool nullable;            // is return type nullable
    varList* args;            // list of function args
    ht_table_t* localTable;   // local hashtable (symtable)
} function;

typedef struct symtableElem{
    enum type {
        FUNCTION,
        VARIABLE
    } type;

    union{
        variable* v;
        function* f;
    };
} symtableElem;

/**
 * @brief creates hash for key
 * 
 * @param key key to hash
 * @return int int hash
 */
int get_hash(char* key);

/**
 * @brief inits hash table
 * 
 * @param table table to init 
 */
void ht_init(ht_table_t* table);

/**
 * @brief searches for an item in the table
 * 
 * @param table table to search in
 * @param key key to search for
 * @return ht_item_t* pointer to item if found, null otherwise
 */
ht_item_t* ht_search(ht_table_t* table, char* key);

/**
 * @brief insert an item into the table
 * 
 * @param table table to insert into
 * @param key key to insert
 * @param value value of the key
 */
void ht_insert(ht_table_t* table, char* key, symtableElem* value);

/**
 * @brief searches for symtable elem in the table
 * 
 * @param table table to search in
 * @param key key to search for
 * @return symtableElem* pointer to symtable element if found, null otherwise
 */
symtableElem* ht_get(ht_table_t* table, char* key);

/**
 * @brief deletes an element from the table
 * 
 * @param table table to delete in
 * @param key key to delete
 */
void ht_delete(ht_table_t* table, char* key);

/**
 * @brief deletes all table items
 * 
 * @param table table to delete
 */
void ht_delete_all(ht_table_t* table);

#endif
