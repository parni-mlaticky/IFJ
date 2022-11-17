#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    // TODO Change to stdin.
    FILE* source_file = fopen(argv[1], "r");

    parse_file(source_file);
}
