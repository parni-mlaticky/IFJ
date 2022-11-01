#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include "token.h"


typedef enum {
/* Every state in the state machine has their own enum...
 * Except: End state that have no arrow coming from them
 */
    START = 0,
    DONE,
    SINGLE_LINE_COMMENT,
    START_OF_COMMENT_OR_DIV,
    BLOCK_COMMENT,
    POSSIBLE_END_OF_BLOCK_COMMENT,
    QUESTION_MARK,
    VARIABLE_ID,
    GREATER_THAN,
    LESSER_THAN,
    IDENTIFIER,
    INT_LITERAL,
    FLOAT_LITERAL_DEC,
    FLOAT_LITERAL_E,
    STRING_LITERAL_DQ,
    STRING_LITERAL_SQ,
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
 * - When START state is returned, it means that the token is finished and that
 *   the last c isn't a part of it.
 * - When DONE state is returned, it means that the token is finsished and
 *   the last c is a part of it.
 *
 * @param current_state Current state
 * @param c Current character
 * @return New state
 */
ScannerState get_next_state(ScannerState current_state, char c);

#endif
