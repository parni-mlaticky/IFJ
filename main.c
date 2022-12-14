/**
 * @file main.c
 * @author Vladimír Hucovič (xhucov00), Ondřej Zobal (xzobal01)
 * @brief main
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main() {
    FILE* source_file = stdin;
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
