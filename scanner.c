#include "scanner.h"
#include "lex.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "list.h"

Token scan_next_token(FILE* file) {
    charList str;
    charListInit(&str);

    int strPos = 0;
    int c;
    bool ignore = false;
    bool skipChar = false;

    ScannerState currentState = S_START;
    Lex currentLex;

    while ((c = fgetc(file)) != EOF) {
        currentState = get_next_state(currentState, c, &currentLex, &ignore, &skipChar);

        if(ignore) {
          ignore = false;
          charListDispose(&str);
          continue;
        }
        if(currentState == S_START) {
          if(currentLex )
  
          ungetc(c, file);
          
          break;
        }
          else if(currentState == S_DONE) {
            charListAppend(&str, c);
            break;
        }
        else {
            charListAppend(&str, c);
        }
  }
    Token token = {
      .lex= currentLex,
      .string=NULL,
  };
    return token;
}

//TODO: add state for concat
ScannerState get_next_state(ScannerState current_state, char c, Lex* lex_out, bool* ignore, bool* skipChar) {
    switch (current_state) {
        case S_START:
            switch (c) {
                case '+':
                    *lex_out = ADD;
                    return S_DONE;
                case '-':
                    *lex_out = SUBTRACT;
                    return S_DONE;
                case ',':
                    *lex_out = COMMA;
                    return S_DONE;
                case '(':
                    *lex_out = PAR_L;
                    return S_DONE;
                case ')':
                    *lex_out = PAR_R;
                    return S_DONE;
                case '{':
                    *lex_out = CBR_L;
                    return S_DONE;
                case '}':
                    *lex_out = CBR_R;
                    return S_DONE;
                case ';':
                    *lex_out = SEMICOLON;
                    return S_DONE;
                case '/':
                    return S_START_OF_COMMENT_OR_DIV;
                case '$':
                    return S_VARIABLE_ID;
                case '>':
                    return S_GREATER_THAN;
                case '<':
                    return S_LESS_EQUAL;
                case '"':
                    return S_STRING_LITERAL_DQ;
                case '\'':
                    return S_STRING_LITERAL_SQ;
                case '?':
                    return S_QUESTION_MARK;
                default:
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                        (c >= '0' && c <= '9')) {
                        return S_IDENTIFIER;
                    } else if(isspace(c)) {
                      return S_START;
                    }
                    // TODO ???
                    return S_START;
            }
        case S_START_OF_COMMENT_OR_DIV:
            switch (c) {
                case '/':
                    return S_SINGLE_LINE_COMMENT;
                case '*':
                    return S_BLOCK_COMMENT;
                default:
                    return S_START;
            }
        case S_SINGLE_LINE_COMMENT:
            switch (c) {
                case '\n':
                    *ignore = true;
                    return S_START;
                default:
                    return S_SINGLE_LINE_COMMENT;
            }
        case S_BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return S_POSSIBLE_END_OF_BLOCK_COMMENT;
                default:
                    return S_BLOCK_COMMENT;
            }
        case S_POSSIBLE_END_OF_BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return S_POSSIBLE_END_OF_BLOCK_COMMENT;
                case '/':
                    *ignore = true;
                    return S_START;
                default:
                    return S_BLOCK_COMMENT;
            }
        case S_QUESTION_MARK:
            if (c == '>') {
                *lex_out = SCRIPT_STOP;
                return S_DONE;
            }
            // Type that can be null
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                return S_IDENTIFIER;
            }
            *lex_out = INVALID;
            return S_START;
        case S_VARIABLE_ID:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9')) {
                return S_VARIABLE_ID;
            }
            *lex_out = VAR_ID;
            return S_START;
        case S_GREATER_THAN:
            if (c == '=') {
                *lex_out = GREATER_EQUAL;
                return S_DONE;
            }

            *lex_out = GREATER;
            return S_START;
        case S_LESS_EQUAL:
            switch (c) {
                case '=':
                    *lex_out = LESS_EQUAL;
                    return S_DONE;
                case '?':
                    *lex_out = SCRIPT_START;
                    return S_DONE;
                default:
                    *lex_out = LESS;
                    return S_START;
            }
        case S_IDENTIFIER:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9')) {
                return S_IDENTIFIER;
            }
            *lex_out = FUN_ID;
            return S_START;
        case S_INT_LITERAL:
            if (c >= '0' && c <= '9') {
                return S_INT_LITERAL;
            }
            if (c == '.') {
                return S_FLOAT_LITERAL_DEC;
            }

            *lex_out = INT_LIT;
            return S_START;
        case S_FLOAT_LITERAL_DEC:
            if (c == 'E' || c == 'e') {
                return S_FLOAT_LITERAL_E;
            }
            if (c >= '0' && c <= '9') {
                return S_FLOAT_LITERAL_DEC;
            }

            *lex_out = FLOAT_LIT;
            return S_START;
        case S_FLOAT_LITERAL_E:
            if (c >= '0' && c <= '9') {
                return S_FLOAT_LITERAL_E;
            }
            *lex_out = FLOAT_LIT;
            return S_START;
        case S_STRING_LITERAL_DQ:
            if (c == '"') {
                *lex_out = STRING_LIT;
                return S_DONE;
            }
            return S_STRING_LITERAL_DQ;
        case S_STRING_LITERAL_SQ:
            if (c == '\'') {
                *lex_out = STRING_LIT;
                return S_DONE;
            }
            return S_STRING_LITERAL_SQ;
        case S_DONE:
            fprintf(stderr, "scanner: get_next_state called with state \"S_DONE\"");
            return S_DONE;
    }
    *ignore = true;
    return S_START;

}
