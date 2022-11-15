#include "scanner.h"
#include "lex.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "list.h"

typedef enum {
/* Every state in the state machine has their own enum...
 * Except: End state that have no arrow coming from them
 */
    S_START = 0,
    S_DONE,
    S_SINGLE_LINE_COMMENT,
    S_START_OF_COMMENT_OR_DIV,
    S_BLOCK_COMMENT,
    S_POSSIBLE_END_OF_BLOCK_COMMENT,
    S_QUESTION_MARK,
    S_VARIABLE_ID,
    S_GREATER_THAN,
    S_LESS_EQUAL,
    S_IDENTIFIER,
    S_INT_LITERAL,
    S_FLOAT_LITERAL_DEC,
    S_FLOAT_LITERAL_E,
    S_STRING_LITERAL_DQ,
    S_STRING_LITERAL_SQ,
    // PHP_BEGIN,
    // PHP_END,
} ScannerState;

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
        // EVERYTHING HERE IS BROKEN LOL!
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

typedef enum {
    // Continue while skipping current character.
    R_NOADD,
    // Continue while including current character.
    R_ADD,
    // Token is finished including current character.
    R_FINAL_ADD,
    // Token is finished but current character belongs to the next token.
    R_FINAL_NOADD,
    // Token is finsihed but is not valuable to the program and shoudl be discarded (comments).
    R_GARBAGE,
    // Token is invalid.
    R_ERROR,
} StateInfoResult;

typedef struct StateInfo {
    StateInfoResult result;
    union {
        // Is valid on resutl: R_NOADD, R_ADD, R_GARBAGE and R_ERROR.
        ScannerState next_state;
        // Is valid on result: R_FINAL_ADD and R_FINAL_NOADD.
        Lex lex;
    };
} StateInfo;

/**
 * Get the next state from the input character and a current state.
 * - When START state is returned, it means that the token is finished and that
 *   the last c isn't a part of it.
 * - When DONE state is returned, it means that the token is finsished and
 *   the last c is a part of it.
 *
 * @param current_state Current state
 * @param c Current character
 * @return New state
 */
StateInfo get_next_state(ScannerState current_state, char c) {
    StateInfo info;
    switch (current_state) {
        case S_START:
            // Early return for throwing out white space fluff.
            if (isspace(c)) {
                return (StateInfo) {.result = R_NOADD, .next_state = S_START};
            }

            switch (c) {
                case '+':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = ADD};

                case '-':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = SUBTRACT};

                case ',':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = COMMA};

                case '(':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = PAR_L};

                case ')':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = PAR_R};

                case '{':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = CBR_L};

                case '}':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = CBR_R};

                case ';':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = SEMICOLON};

                case '/':
                    return (StateInfo) {.result = R_ADD, .next_state = S_START_OF_COMMENT_OR_DIV};

                case '$':
                    return (StateInfo) {.result = R_ADD, .next_state = S_VARIABLE_ID};

                case '>':
                    return (StateInfo) {.result = R_ADD, .next_state = S_GREATER_THAN};

                case '<':
                    return (StateInfo) {.result = R_ADD, .next_state = S_LESS_EQUAL};

                case '"':
                    return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_DQ};

                case '\'':
                    return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_SQ};

                case '?':
                    return (StateInfo) {.result = R_ADD, .next_state = S_QUESTION_MARK};

                default:
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                        (c >= '0' && c <= '9')) {
                        return (StateInfo) {.result = R_ADD, .next_state = S_IDENTIFIER};
                    }
                    return (StateInfo) {.result = R_FINAL_NOADD, .lex = STRING_LIT};
            }

        case S_START_OF_COMMENT_OR_DIV:
            switch (c) {
                case '/':
                    return (StateInfo) {.result = R_ADD, .next_state = S_SINGLE_LINE_COMMENT};

                case '*':
                    return (StateInfo) {.result = R_ADD, .next_state = S_BLOCK_COMMENT};

                default:
                    return (StateInfo) {.result = R_ADD, .next_state = S_START};
            }

        case S_SINGLE_LINE_COMMENT:
            switch (c) {
                case '\n':
                    return (StateInfo) {.result = R_GARBAGE, .next_state = S_START};

                default:
                    return (StateInfo) {.result = R_ADD, .next_state = S_SINGLE_LINE_COMMENT};
            }

        case S_BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return (StateInfo) {.result = R_ADD, .next_state = S_POSSIBLE_END_OF_BLOCK_COMMENT};

                default:
                    return (StateInfo) {.result = R_ADD, .next_state = S_BLOCK_COMMENT};
            }

        case S_POSSIBLE_END_OF_BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return (StateInfo) {.result = R_ADD, .next_state = S_POSSIBLE_END_OF_BLOCK_COMMENT};

                case '/':
                    return (StateInfo) {.result = R_GARBAGE, .next_state = S_START};

                default:
                    return (StateInfo) {.result = R_ADD, .next_state = S_BLOCK_COMMENT};
            }

        case S_QUESTION_MARK:
            if (c == '>') {
                return (StateInfo) {.result = R_FINAL_ADD, .lex = SCRIPT_STOP};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = QUESTION_MARK};

        case S_VARIABLE_ID:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9')) {
                return (StateInfo) {.result = R_ADD, .next_state = S_VARIABLE_ID};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = VAR_ID};

        case S_GREATER_THAN:
            // FIXME Needs one more '=' buddy :-)
            if (c == '=') {
                return (StateInfo) {.result = R_FINAL_ADD, .lex = GREATER_EQUAL};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = GREATER};

        case S_LESS_EQUAL:
            switch (c) {
                case '=':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = LESS_EQUAL};

                case '?':
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = SCRIPT_START};

                default:
                    return (StateInfo) {.result = R_FINAL_ADD, .lex = LESS};
            }

        case S_IDENTIFIER:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9')) {
                return (StateInfo) {.result = R_ADD, .next_state = S_IDENTIFIER};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = FUN_ID};

        case S_INT_LITERAL:
            if (c >= '0' && c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_INT_LITERAL};
            }
            if (c == '.') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_DEC};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = INT_LIT};

        case S_FLOAT_LITERAL_DEC:
            if (c == 'E' || c == 'e') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_E};
            }
            if (c >= '0' && c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_DEC};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = FLOAT_LIT};

        case S_FLOAT_LITERAL_E:
            if (c >= '0' && c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_E};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = FLOAT_LIT};

        case S_STRING_LITERAL_DQ:
            if (c == '"') {
                return (StateInfo) {.result = R_FINAL_ADD, .lex = STRING_LIT};
            }
            return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_DQ};

        case S_STRING_LITERAL_SQ:
            if (c == '\'') {
                return (StateInfo) {.result = R_FINAL_ADD, .lex = STRING_LIT};
            }
            return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_SQ};
    }
    return (StateInfo) {.result = R_ERROR, .next_state = S_START};
}
