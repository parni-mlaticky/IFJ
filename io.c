/**
 * @file io.c
 * @name IJC - Domácí úkol 2, příklad b), 14.4.2022
 * @author Vladimír Hucovič, FIT
 * Přeloženo pomocí GCC verze 11.2.0
 */

#include <ctype.h>
#include <stdio.h>

// cte 1 slovo do stringu s a vraci jeho delku. slovo se usekne po presahnuti limitu max, ale pocitani delky pokracuje az do bileho znaku.
int read_word(char *s, int max, FILE *f){
    for (int i = 0; i < max; ++i) {
        s[i] = '\0';
    }
    char c;
    int len = 0;
    while(len < max){
        c = getc(f);
        if(c == EOF && len == 0){
            return EOF;
        }
        if(isspace(c) || c == EOF){
            return len;
        }
        s[len] = c;
        len++;
    }
    s[127] = '\0';
    while(1){
        c = getc(f);
        if(isspace(c) || c == EOF){
            return len;
        }
        len++;
    }
}

