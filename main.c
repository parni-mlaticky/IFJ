#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    // TODO Change to stdin.
    FILE* source_file = fopen(argv[1], "r");

    Token* tokens = parse_file(source_file);

    printf(">>> TOKENS <<<\n");
    while (tokens->lex) {
        printf("%d\n", tokens++->lex);
    }
    printf(">>> END OF TOKENS <<<\n");
    free(tokens);

}
