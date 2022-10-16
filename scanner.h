#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include "token.h"

typedef enum {
/* Every state in the state machine has their own enum...
 * Except: End state that have no arrow coming from them
 */
    START = 0,
    SINGLE_LINE_COMMENT,
    START_OF_COMMENT,
    BLOCK_COMMENT,
    POSSIBLE_END_OF_BLOCK_COMMENT,
    START_OF_VARIABLE_ID,
    VARIABLE_ID,
    GREATER_THAN,
    LESSER_THAN,
    IDENTIFIER,
    INT_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    PHP_BEGIN,
    PHP_END,
} ScannerState;

/**
 * Reads the whole token from the input file and returns it as a token
 * @param file Input file
 * @return The new token
 */
Token scan_next_token(FILE* file);

/**
 * Get the next state from the input character and a current state.
 * @param current_state Current state
 * @param c Current character
 * @return New state
 */
ScannerState get_next_state(ScannerState current_state, char c);

#endif
