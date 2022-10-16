#include <stdlib.h>
#include "parser.h"
#include "scanner.h"

Token* parse_file(FILE* file) {
    Token* tokens = malloc(100 * sizeof(Token));

    int counter = 0;
    Token token;
    do {
        token = scan_next_token(file);
        tokens[counter++] = token;
    } while (token.lex != END);

    return tokens;
}
