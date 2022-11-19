#include <stdlib.h>
#include "parser.h"

tokList* parse_file(FILE* file) {
    tokList* list = malloc(sizeof(tokList));
    tokListInit(list);

    scan_next_token(file, true);
    Token token;
    do {
        Token* new = malloc(sizeof(Token));
        *new = scan_next_token(file, false);
        token = *new;
        tokListAppend(list, new);
    } while (token.lex != END);

    debug_print_tokens(list);

    return list;
}
