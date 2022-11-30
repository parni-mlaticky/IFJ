#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    FILE* source_file = stdin;

    generateStarterAsm();

    bool res = parse_file(source_file);
    if(res){
        //fprintf(stderr, "Syntax is OK\n");
        exit(0);
    } 
    else {
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
}
