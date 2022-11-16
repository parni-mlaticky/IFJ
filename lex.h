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
    // CONTROLL LEXES
    INVALID,        // Invalid token, this is an error!

    // USEFULL LEXES
    END = 0,        // EOF (becuase the parser should probably know)
    SCRIPT_START,   // <? TODO does this also need to include the "php"?
    SCRIPT_STOP,    // ?>
    STRING_LIT,     // "bruh"
    INT_LIT,        // 10
    FLOAT_LIT,      // 10.3
    VAR_ID,         // $A-Za-z_0-9
    FUN_ID,         // A-Za-z_0-9
    PAR_L,          // (
    PAR_R,          // )
    GREATER,        // >
    GREATER_EQUAL,  // >=
    LESS,           // <
    LESS_EQUAL,      // <=
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
    QUESTION_MARK,  // ?
} Lex;

#endif
