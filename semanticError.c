/**
 * @file semanticError.c
 * @author Vladimír Hucovič (xhucov00)
 * @brief implementation of semantic error function
 * 
 */
#include "semanticError.h"

void semanticError(int code){
    fprintf(stderr, "Semantic error code %d, exiting...\n", code);
    exit(code);
}
