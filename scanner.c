#include "scanner.h"
#include <stdlib.h>

Token scan_next_token(FILE* file) {
    char *str = malloc(100);
    (void) str; // HACK

    ScannerState currentState = START;
    (void) currentState; // HACK
    char c;
    while ((c = fgetc(file)) != EOF) {
        // switch(currentState) {
        // }
    }

    Token token = {
        .lex=END,
        .string=NULL,
    };
    return token;
}

ScannerState get_next_state(ScannerState current_state, char c) {
    switch (current_state) {
        case START:
            switch (c) {
                case '/':
                    return START_OF_COMMENT_OR_DIV;
                case '$':
                    return VARIABLE_ID;
                case '>':
                    return GREATER_THAN;
                case '<':
                    return LESSER_THAN;
                case '"':
                    return STRING_LITERAL_DQ;
                case '\'':
                    return STRING_LITERAL_SQ;
                case '?':
                    return QUESTION_MARK;
                default:
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c >= 'Z') ||
                        (c >= '0' && c <= '9')) {
                        return IDENTIFIER;
                    }
                    return START;
            }
        case START_OF_COMMENT_OR_DIV:
            switch (c) {
                case '/':
                    return SINGLE_LINE_COMMENT;
                case '*':
                    return BLOCK_COMMENT;
                default:
                    return START;
            }
        case SINGLE_LINE_COMMENT:
            switch (c) {
                case '\n':
                    return DONE;
                default:
                    return SINGLE_LINE_COMMENT;
            }
        case BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return POSSIBLE_END_OF_BLOCK_COMMENT;
                default:
                    return BLOCK_COMMENT;
            }
        case POSSIBLE_END_OF_BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return POSSIBLE_END_OF_BLOCK_COMMENT;
                case '/':
                    return DONE;
                default:
                    return BLOCK_COMMENT;
            }
        case QUESTION_MARK:
            if (c == '>') {
                return PHP_END;
            }
            // Type that can be null
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c >= 'Z')) {
                return IDENTIFIER;
            }
            return START;
        case VARIABLE_ID:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c >= 'Z') ||
                (c >= '0' && c <= '9')) {
                return VARIABLE_ID;
            }
            return START;
        case GREATER_THAN:
            if (c == '=') {
                return DONE;
            }
            return START;
        case LESSER_THAN:
            switch (c) {
                case '=':
                    return DONE;
                case '?':
                    return PHP_BEGIN;
                default:
                    return START;
            }
        case IDENTIFIER:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c >= 'Z') ||
                (c >= '0' && c <= '9')) {
                return IDENTIFIER;
            }
            return START;
        case INT_LITERAL:
            if (c >= '0' && c <= '9') {
                return INT_LITERAL;
            }
            if (c == '.') {
                return FLOAT_LITERAL_DEC;
            }
            return START;
        case FLOAT_LITERAL_DEC:
            if (c == 'E' || c == 'e') {
                return FLOAT_LITERAL_E;
            }
            if (c >= '0' && c <= '9') {
                return FLOAT_LITERAL_DEC;
            }
            return START;
        case FLOAT_LITERAL_E:
            if (c >= '0' && c <= '9') {
                return FLOAT_LITERAL_E;
            }
            return START;
        case STRING_LITERAL_DQ:
            if (c == '"') {
                return DONE;
            }
            return STRING_LITERAL_DQ;
        case STRING_LITERAL_SQ:
            if (c == '\'') {
                return DONE;
            }
            return STRING_LITERAL_SQ;
        case PHP_BEGIN:
            if (c == '?') {
                return PHP_BEGIN;
            }
            return START;
        case PHP_END:
            if (c == '>') {
                return DONE;
            }
            return START;

        case DONE:
            fprintf(stderr, "scanner: get_next_state called with state \"DONE\"");
            return DONE;
    }
    fprintf(stderr, "scanner: get_next_state failed to assign propper state."
            "This should not be possible! 💀");
    return START;

}
