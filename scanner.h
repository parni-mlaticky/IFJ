#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include "token.h"
#include "lex.h"
#include <stdbool.h>
#include "list.h"

/**
 * Reads the whole token from the input file and returns it as a token
 * @param file Input file
 * @return The new token
 */
Token scan_next_token(FILE* file);
void debug_print_tokens(tokList* list);
#endif
