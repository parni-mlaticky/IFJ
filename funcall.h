/**
 * @file funcall.h
 * @author Vladimír Hucovič (xhucov00)
 * @brief header file for funcall structure
 * 
 */
#pragma once
#include "list.h"

typedef struct funcall{
    char* funId;
    nontermList* args;
} funcall;