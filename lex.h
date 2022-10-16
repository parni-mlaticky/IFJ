/**
 * @file lex.h
 * @author Petr Kolouch (xkolou05@stud.fit.vutbr.cz)
 * @brief header file for lex enum definition
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef LEX_H
#define LEX_H

/**
 * @brief enum definition for lexeme types
 * 
 */
typedef enum Lex{
    END = 0,        // EOF
    VAR_ID,         // $A-Za-z_0-9
    FUN_ID,         // A-Za-z_0-9
    PAR_L,          // (
    PAR_R,          // )
    GREATER,        // >
    GREATER_EQUAL,  // >=
    LESS,           // <
    LESS_THAN,      // <=
    ASSIGN,         // =
    EQUAL,          // ==
    MULTIPLY,       // *
    DIVIDE,         // /
    ADD,            // +
    SUBTRACT,       // -
    MODULO,         // %
    CBR_L,          // {
    CBR_R,          // }
    SEMICOLON,      // ;
    COMMA,          // ,
} Lex;

#endif
