#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include "token.h"
#include "lex.h"
#include <stdbool.h>


typedef enum {
/* Every state in the state machine has their own enum...
 * Except: End state that have no arrow coming from them
 */
    S_START = 0,
    S_DONE,
    S_SINGLE_LINE_COMMENT,
    S_START_OF_COMMENT_OR_DIV,
    S_BLOCK_COMMENT,
    S_POSSIBLE_END_OF_BLOCK_COMMENT,
    S_QUESTION_MARK,
    S_VARIABLE_ID,
    S_GREATER_THAN,
    S_LESS_EQUAL,
    S_IDENTIFIER,
    S_INT_LITERAL,
    S_FLOAT_LITERAL_DEC,
    S_FLOAT_LITERAL_E,
    S_STRING_LITERAL_DQ,
    S_STRING_LITERAL_SQ,
    // PHP_BEGIN,
    // PHP_END,
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
ScannerState get_next_state(ScannerState current_state, char c, Lex* lex_out, bool* ignore, bool* skipChar);

#endif
