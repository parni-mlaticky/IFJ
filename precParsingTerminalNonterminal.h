/**
 * @file precParsingTerminalNonterminal.h
 * @author Vladimír Hucovič (xhucov00)
 * @brief header file for terminal, nonterminal structure
 * 
 */
#pragma once
#include "precParsingEnums.h"
#include "variable.h"
#include "list.h"

typedef enum NonterminalType{
    EMPTY,
    LITERAL_TERM,
    VAR_ID_TERM,
    FUNCALL_TERM,
    EXPR
} NonterminalType;

typedef struct Nonterminal{
    NonterminalType NTType; // Term or expression
    dataType dType; // If this nonterminal is a term, this is its data type
    
    // If nonterminal is a term, this is where its literal or var name/function name is stored
    union term{
        variable* var;
        struct funcall* func;
        char* stringLit;
        long long int integerLit;
        double floatLit;
    } term;

    // If this nonterminal is an expression, here are the pointers to the nonterminals it contains and the operator
    struct {
        struct Nonterminal* left;
        struct Nonterminal* right;
        terminalType op;
    } expr;

} Nonterminal;

typedef struct Terminal{
    terminalType tType;
    Token* token;
    struct nontermList* funcArgs; // If the terminal is a function call, this is where its arguments are stored
} Terminal;
