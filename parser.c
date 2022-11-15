#include <stdlib.h>
#include "parser.h"
#include "scanner.h"

Token* parse_file(FILE* file) {
  //int currentChar;
    Token* tokens = malloc(100 * sizeof(Token));

    int counter = 0;
    Token token;
    do {
        token = scan_next_token(file);
        printf("adding tokens with %d %s \n", token.lex, token.string);
        tokens[counter++] = token;
    } while (token.lex != END);

    return tokens;
}
