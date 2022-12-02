#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

void ht_init(ht_table_t *table) {
  for (int i = 0; i < MAX_HT_SIZE; i++){

    (*table)[i] = NULL;
  }
}

ht_item_t *ht_search(ht_table_t *table, char *key) {
  ht_item_t* current = (*table)[get_hash(key)];
  while (current != NULL) {
    if (!strcmp(current->key, key)) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void ht_insert(ht_table_t *table, char *key, symtableElem* value) {
  int hash = get_hash(key);

  // Update the element if it already exists.
  ht_item_t** current = &((*table)[hash]);
  while (*current != NULL) {
    if (!strcmp((*current)->key, key)) {
      (*current)->value = value;
      return;
    }
    current = &((*current)->next);
  }

  // Init the new element.
  ht_item_t* new = malloc(sizeof(ht_item_t));
  char* new_key = malloc(strlen(key)+1);
  strcpy(new_key, key);
  new->key = new_key;
  new->value = value;

  // Link it into the list.
  new->next = (*table)[hash];
  (*table)[hash] = new;
}

symtableElem* ht_get(ht_table_t *table, char *key) {
  ht_item_t* item = ht_search(table, key);
  return item == NULL ? NULL : item->value;
}

void ht_delete(ht_table_t *table, char *key) {
  ht_item_t** current = &(*table)[get_hash(key)];
  if (current == NULL) return;

  while (strcmp((*current)->key, key)) {
    current = &(*current)->next;
    if (current == NULL) break;
  }

  ht_item_t* delete = *current;
  *current = (*current)->next;
  free(delete->key);
  free(delete);
}

void ht_delete_all(ht_table_t *table) {
  for (int i = 0; i < HT_SIZE; i++) {
    ht_item_t* current = (*table)[i];
    while (current != NULL) {
      ht_item_t* old = current;
      current = current->next;
      free(old->key);
      free(old);
    }
    ((*table)[i]) = NULL;
  }
}
