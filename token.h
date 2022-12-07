/**
 * @file token.h
 * @author Petr Kolouch
 * @brief header file for lexeme enum definition
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include "lex.h"

/**
 * @brief 
 * 
 */
typedef struct Token {
    // state machine end states enum (Function ID, Variable ID, ...)
    Lex lex;

    union {
        // if lex is a variable, this will be the name of it
        char* string;
        // value of int number
        long long int integer;
        // value of double number
        double decimal;
    };
} Token;

#endif
