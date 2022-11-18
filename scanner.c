#include "scanner.h"
#include "lex.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

typedef enum
{
    /* Every state in the state machine has their own enum...
     * Except: End state that have no arrow coming from them
     */
    S_START = 0,
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
    S_EQUAL,
    S_CONCAT_OR_FLOAT,
    S_NOT_EQUAL,
    S_NOT_EQUAL_FINAL,
    S_STRICT_EQUAL,
} ScannerState;

typedef enum
{
    // Continue while skipping current character.
    R_SKIP,
    // Continue while including current character.
    R_ADD,
    // Token is finished including current character.
    R_FINAL_ADD,
    // Token is finished but current character belongs to the next token.
    R_FINAL_NOADD,
    // Token is finsihed but skip current character
    R_FINAL_SKIP,
    // Token is invalid.
    R_ERROR,
} StateInfoResult;

typedef struct StateInfo
{
    StateInfoResult result;
    union
    {
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
    switch (current_state) {
        case S_START:
            // Early return for throwing out white space fluff.
            if (isspace(c)) {
                return (StateInfo) {.result = R_SKIP, .next_state = S_START};
            }

            switch (c) {
                case '+':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = ADD};

                case '-':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = SUBTRACT};

                case ',':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = COMMA};

                case '(':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = PAR_L};

                case ')':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = PAR_R};

                case '{':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = CBR_L};

                case '}':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = CBR_R};

                case ';':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = SEMICOLON};

                case '*':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = MULTIPLY};

                case ':':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = COLON};

                case '/':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_START_OF_COMMENT_OR_DIV};

                case '$':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_VARIABLE_ID};

                case '>':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_GREATER_THAN};

                case '<':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_LESS_EQUAL};

                case '"':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_DQ};

                case '\'':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_SQ};

                case '?':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_QUESTION_MARK};

                case '=':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_EQUAL};

                case '.':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_CONCAT_OR_FLOAT};

                case '!':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_NOT_EQUAL};

                default:
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
                        return (StateInfo) {.result = R_ADD, .next_state = S_IDENTIFIER};
                      } 
                      else if((c >= '0' && c <= '9')) {
                        return (StateInfo) {.result = R_ADD, .next_state = S_INT_LITERAL};
                      }
                    return (StateInfo) {.result = R_FINAL_NOADD, .lex = STRING_LIT};
            }

        case S_START_OF_COMMENT_OR_DIV:
            switch (c) {
                case '/':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_SINGLE_LINE_COMMENT};

                case '*':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_BLOCK_COMMENT};

                default:
                    return (StateInfo) {.result = R_ADD, .next_state = S_START};
            }

        case S_SINGLE_LINE_COMMENT:
            switch (c) {
                case '\n':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_START};

                default:
                    return (StateInfo) {.result = R_SKIP, .next_state = S_SINGLE_LINE_COMMENT};
            }

        case S_BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_POSSIBLE_END_OF_BLOCK_COMMENT};

                default:
                    return (StateInfo) {.result = R_SKIP, .next_state = S_BLOCK_COMMENT};
            }

        case S_POSSIBLE_END_OF_BLOCK_COMMENT:
            switch (c) {
                case '*':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_POSSIBLE_END_OF_BLOCK_COMMENT};

                case '/':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_START};

                default:
                    return (StateInfo) {.result = R_SKIP, .next_state = S_BLOCK_COMMENT};
            }

        case S_QUESTION_MARK:
            if (c == '>') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = SCRIPT_STOP};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = QUESTION_MARK};

        case S_VARIABLE_ID:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || (c == '-' || c == '_')) {
                return (StateInfo) {.result = R_ADD, .next_state = S_VARIABLE_ID};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = VAR_ID};

        case S_GREATER_THAN:
            if (c == '=') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = GREATER_EQUAL};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = GREATER};

        case S_LESS_EQUAL:
            switch (c) {
                case '=':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = LESS_EQUAL};

                case '?':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = SCRIPT_START};

                default:
                    return (StateInfo) {.result = R_FINAL_NOADD, .lex = LESS};
            }

        case S_IDENTIFIER:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || (c == '-' || c == '_')) {
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
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = STRING_LIT};
            }
            return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_DQ};

        case S_STRING_LITERAL_SQ:
            if (c == '\'') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = STRING_LIT};
            }
            return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_SQ};

        case S_EQUAL:
            if(c == '=') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_STRICT_EQUAL};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = ASSIGN};
        case S_STRICT_EQUAL:
            if(c == '=') {
              return (StateInfo) {.result = R_FINAL_SKIP, .lex = EQUAL};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_CONCAT_OR_FLOAT:
            if(c >= '0' && c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_DEC};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = CONCAT};

        case S_NOT_EQUAL:
            if(c == '=') {
              return (StateInfo) {.result = R_SKIP, .next_state = S_NOT_EQUAL_FINAL};
            }
            return (StateInfo) {.result = R_ERROR}; // FIXME: which nextstate

        case S_NOT_EQUAL_FINAL:
            if(c == '=') {
              return (StateInfo) {.result = R_FINAL_SKIP, .lex = NOT_EQUAL};
            }
              return (StateInfo) {.result = R_ERROR}; // FIXME: which nextstate
    }
    return (StateInfo) {.result = R_ERROR, .next_state = S_START};
}


Token scan_next_token(FILE *file)
{
    charList str;
    charListInit(&str);
    Lex returnLex;

    int c;

    ScannerState currentState = S_START;
    StateInfo nextState;

    while ((c = fgetc(file)) != EOF) {
        printf("%c", c);
        nextState = get_next_state(currentState, c);
        currentState = nextState.next_state;

        if(nextState.result == R_SKIP) {
            continue;
        }
        else if(nextState.result == R_ADD) {
            charListAppend(&str, c);
            continue;
        }
        else if(nextState.result ==  R_FINAL_ADD) {
            charListAppend(&str, c);
            returnLex = nextState.lex;
            break;
        }
        else if(nextState.result == R_FINAL_NOADD) {
            returnLex = nextState.lex;
            ungetc(c, file);
            break;
        }
        else if(nextState.result == R_FINAL_SKIP) {
            returnLex = nextState.lex;
            break;
        }
        else{
            char* string = charListToString(&str);
            fprintf(stderr, "Received token \"%s%c\"is invalid, lexical analysis error,"
                    " exiting...", string, c);
            free(string);
            exit(1);
        }
    }

    if(c == EOF) {
        return (Token) {
            .lex = END,
        };
    }

    Token token = {.lex=returnLex};
    // Breaking the independence from the state machine a little.
    // The state machine should ensure that int and float literals will always be valid.
    if (returnLex == INT_LIT) {
        char *temp = charListToString(&str);
        token.integer = atoi(temp);
        free(temp);
    }
    else if (returnLex == FLOAT_LIT) {
        char* temp = charListToString(&str);
        char* endptr;
        token.decimal = strtod(temp, &endptr);
        free(temp);
    }
    else {
        token.string = str.len ? charListToString(&str) : NULL;
    }

    return token;
}

void debug_print_tokens(tokList* list) {
    printf(">>> BEGIN TOKENS <<<\n");
    printf("#\tLEX\tTYPE\tDATA\n");
    int i = 0;
    tokElem* current = tokListGetFirst(list);
    while (current != NULL) {
        printf("#%d\t%d\t", i++, current->data->lex);
        bool printable = true;
        if (current->data->string && current->data->lex != FLOAT_LIT && current->data->lex != INT_LIT) {
            for (int i=0; current->data->string[i] != '\0'; i++) {
                if (current->data->string[i] == '\0') break;
                if (!isprint(current->data->string[i])) {
                    printable = false;
                    break;
                }
            }
            if (printable) {
                printf("(text)\t%s", current->data->string);
            }
        }
        else if (current->data->lex == FLOAT_LIT) {
            printf("(float)\t%f", current->data->decimal);
        }
        else if (current->data->lex == INT_LIT) {
            printf("(int)\t%d", current->data->integer);
        }
        printf("\n");
        current = current->next;
    }
    printf(">>> END   TOKENS <<<\n");
}
