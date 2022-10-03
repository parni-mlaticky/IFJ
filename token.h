/**
 * @file token.h
 * @author Petr Kolouch (xkolou05@stud.fit.vutbr.cz)
 * @brief header file for lex enum definition
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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
    int value;
    // value of double number    
    double value;
    // will be used when we dont need to store the name of the lex (+ - * /, ...) 
    void* empty; 
  };
};