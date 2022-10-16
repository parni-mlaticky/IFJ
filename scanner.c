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
