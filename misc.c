/**
 * @file misc.c
 * @author Vladimír Hucovič (xhucov00)
 * @brief implementation of miscellaneous functions
 * 
 */
#include "misc.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

char* str_clone(char* src) {
    int len = strlen(src)+1;
    char* new = malloc(len * sizeof(char));
    CHECK_ALLOCATION(new)
    memcpy(new, src, len);
    return new;
}
