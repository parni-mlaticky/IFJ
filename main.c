#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    // TODO Change to stdin.
    FILE* source_file = stdin;

    bool res = parse_file(source_file);
    if(res){
        printf("Syntax is OK\n");
        exit(0);
    } 
    else {
        printf("Syntax error\n");
        exit(2);
    }
}
