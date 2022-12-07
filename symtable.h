/**
 * @file symtable.h
 * @author Vladimír Hucovič (xhucov00), Petr Kolouch (xkolou05), Ondřej Zobal (xzobal01), Marek Havel (xhavel46)
 * @brief header file for symtable based on hash table
 * 
 */
#pragma once

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include "variable.h"
#include "list.h"

/**
 * 
 *  hlavička hashovací tabulky převzata z předmětu IAL
 *
 */


// maximal size of hash table
#define MAX_SYMTABLE_SIZE 101

// size of implemented table
extern int SYMTABLE_SIZE;

typedef struct symtable_item {
  char* key;            // pair key
  struct symtableElem* value;          // pair value
  struct symtable_item *next; // pointer to next synonym
} symtable_item_t;


typedef symtable_item_t* sym_table_t[MAX_SYMTABLE_SIZE];
typedef struct {
  char* functionName;         // name of function
    dataType returnType;      // function return type
    bool nullable;            // is return type nullable
    varList* args;            // list of function args
    sym_table_t* localTable;   // local hashtable (symtable)
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
void symtable_init(sym_table_t* table);

/**
 * @brief searches for an item in the table
 * 
 * @param table table to search in
 * @param key key to search for
 * @return symtable_item_t* pointer to item if found, null otherwise
 */
symtable_item_t* symtable_search(sym_table_t* table, char* key);

/**
 * @brief insert an item into the table
 * 
 * @param table table to insert into
 * @param key key to insert
 * @param value value of the key
 */
void symtable_insert(sym_table_t* table, char* key, symtableElem* value);

/**
 * @brief searches for symtable elem in the table
 * 
 * @param table table to search in
 * @param key key to search for
 * @return symtableElem* pointer to symtable element if found, null otherwise
 */
symtableElem* symtable_get(sym_table_t* table, char* key);

/**
 * @brief deletes an element from the table
 * 
 * @param table table to delete in
 * @param key key to delete
 */
void symtable_delete(sym_table_t* table, char* key);

/**
 * @brief deletes all table items
 * 
 * @param table table to delete
 */
void symtable_delete_all(sym_table_t* table);

#endif
