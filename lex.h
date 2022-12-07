/**
 * @file lex.h
 * @author Petr Kolouch (xkolou05)
 * @brief header file for lex enum definition
 * 
 */

#ifndef LEX_H
#define LEX_H

/**
 * @brief enum definition for lexeme types
 */
typedef enum Lex{
    END            = 0,   // EOF
    SCRIPT_START   = 1,   // <?php
    SCRIPT_STOP    = 2,   // ?>
    STRING_LIT     = 3,   // "bruh"
    INT_LIT        = 4,   // 10
    FLOAT_LIT      = 5,   // 10.3
    VAR_ID         = 6,   // $A-Za-z_0-9
    FUN_ID         = 7,   // A-Za-z_0-9
    PAR_L          = 8,   // (
    PAR_R          = 9,   // )
    GREATER        = 10,  // >
    GREATER_EQUAL  = 11,  // >=
    LESS           = 12,  // <
    LESS_EQUAL     = 13,  // <=
    ASSIGN         = 14,  // =
    EQUAL          = 15,  // ===
    MULTIPLY       = 16,  // *
    DIVIDE         = 17,  // /
    ADD            = 18,  // +
    SUBTRACT       = 19,  // -
    MODULO         = 20,  // %
    CBR_L          = 21,  // {
    CBR_R          = 22,  // }
    SEMICOLON      = 23,  // ;
    COMMA          = 24,  // ,
    QUESTION_MARK  = 25,  // ?
    CONCAT         = 26,  // .
    NOT_EQUAL      = 27,  // !==
    COLON          = 28,  // :
} Lex;

#endif
