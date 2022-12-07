/**
 * @file symtable.c
 * @author Vladimír Hucovič, Ondřej Zobal, Marek Havel
 * @brief implementation of symtable
 * 
 */
#include "symtable.h"
#include <stdlib.h>
#include <string.h>

int SYMTABLE_SIZE = MAX_SYMTABLE_SIZE;

int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % SYMTABLE_SIZE);
}

void symtable_init(sym_table_t *table) {
  for (int i = 0; i < MAX_SYMTABLE_SIZE; i++){

    (*table)[i] = NULL;
  }
}

symtable_item_t *symtable_search(sym_table_t *table, char *key) {
  symtable_item_t* current = (*table)[get_hash(key)];
  while (current != NULL) {
    if (!strcmp(current->key, key)) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void symtable_insert(sym_table_t *table, char *key, symtableElem* value) {
  int hash = get_hash(key);

  // Update the element if it already exists.
  symtable_item_t** current = &((*table)[hash]);
  while (*current != NULL) {
    if (!strcmp((*current)->key, key)) {
      (*current)->value = value;
      return;
    }
    current = &((*current)->next);
  }

  // Init the new element.
  symtable_item_t* new = malloc(sizeof(symtable_item_t));
  char* new_key = malloc(strlen(key)+1);
  strcpy(new_key, key);
  new->key = new_key;
  new->value = value;

  // Link it into the list.
  new->next = (*table)[hash];
  (*table)[hash] = new;
}

symtableElem* symtable_get(sym_table_t *table, char *key) {
  symtable_item_t* item = symtable_search(table, key);
  return item == NULL ? NULL : item->value;
}

void symtable_delete(sym_table_t *table, char *key) {
  symtable_item_t** current = &(*table)[get_hash(key)];
  if (current == NULL) return;

  while (strcmp((*current)->key, key)) {
    current = &(*current)->next;
    if (current == NULL) break;
  }

  symtable_item_t* delete = *current;
  *current = (*current)->next;
  free(delete->key);
  free(delete);
}

void symtable_delete_all(sym_table_t *table) {
  for (int i = 0; i < SYMTABLE_SIZE; i++) {
    symtable_item_t* current = (*table)[i];
    while (current != NULL) {
      symtable_item_t* old = current;
      current = current->next;
      free(old->key);
      free(old);
    }
    ((*table)[i]) = NULL;
  }
}
