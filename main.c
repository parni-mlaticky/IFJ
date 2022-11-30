#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    FILE* source_file = stdin;

    // TODO Create a separate function for the code that is always generated
    printf(".IFJcode22\n");
    printf("DEFVAR GF@%%RAX\n");
    printf("DEFVAR GF@%%RBX\n");
    printf("CREATEFRAME\nPUSHFRAME\n");
    printf("JUMP %%PROG_START\n");
    generateToBoolFunction();
    generateToFloatFunction();
    generateEnforceTypesFunction();
    generateStackSwap();

    printf("LABEL %%PROG_START\n");

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
