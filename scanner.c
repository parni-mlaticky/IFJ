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
    S_PROLOG,
    S_PROLOG_L,
    S_PROLOG_LQ,
    S_PROLOG_LQP,
    S_PROLOG_LQPH,
    S_SINGLE_LINE_COMMENT,
    S_START_OF_COMMENT_OR_DIV,
    S_BLOCK_COMMENT,
    S_POSSIBLE_END_OF_BLOCK_COMMENT,
    S_QUESTION_MARK,
    S_START_VARIABLE_ID,
    S_VARIABLE_ID,
    S_GREATER_THAN,
    S_LESS_EQUAL,
    S_IDENTIFIER,
    S_INT_LITERAL,
    S_FLOAT_LITERAL_DEC,
    S_FLOAT_LITERAL_E,
    S_FLOAT_LITERAL_E_NUM,
    S_STRING_LITERAL_DQ,
    S_STRING_LITERAL_DQ_ESCAPE,
    S_STRING_LITERAL_SQ,
    S_STRING_LITERAL_SQ_ESCAPE,
    S_EQUAL,
    S_CONCAT_OR_FLOAT,
    S_NOT_EQUAL,
    S_NOT_EQUAL_FINAL,
    S_STRICT_EQUAL,
} ScannerState;

typedef enum {
  // Processing an unknown escape sequence.
  ESS_UNKNOWN,
  // Processing hex escape sequence.
  ESS_HEX,
  // Processing octal escape sequence.
  ESS_OCTAL,
  // Valid escape, add byte in variable c.
  ESS_FINAL,
  // Valid escape, add byte in variable c but ungetc variable "unget".
  ESS_FINAL_NOADD,
  // Invalid escape, add '\\', contents of memory and c
  ESS_FINAL_RAW,
} EscapeSeqState;

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

typedef struct FSMMemory {
    charList memory;
    EscapeSeqState escape_seq_state;
    char unget;
} FSMMemory;

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
 * @param c Should be set to current input character, when returned state is ESS_FINAL
 * c will contain character code represented by the escape sequence.
 * @param buffer Initialized 4 char c-string buffer. This buffer shoudl not be changed between calls to this function.
 * @param current_state Should be set to previously returned state. Initial state is ESS_UNKNOWN.
 * @return next state based on given input c. Reading is not finished until ESS_FINAL is returned.
 */
EscapeSeqState process_str_escape_sequence(char* c, charList* buffer, EscapeSeqState current_state, char* unget, char quote) {
    switch (current_state) {
        case ESS_UNKNOWN:
            charListInit(buffer);
            if (*c == 'n') {
               *c = '\n';
               return ESS_FINAL;
            }
            if (*c == 't') {
                *c = '\t';
                return ESS_FINAL;
            }
            if (*c == '\\') {
                *c = '\\';
                return ESS_FINAL;
            }
            if (*c == quote) {
                *c = quote;
                return ESS_FINAL;
            }
            if (*c == '$') {
                *c = '$';
                return ESS_FINAL;
            }
            if (*c == 'x') {
                return ESS_HEX;
            }
            if (*c >= '0' && *c <= '7') {
                charListAppend(buffer, *c);
                return ESS_OCTAL;
            }
            return ESS_FINAL_RAW;

        case ESS_HEX:
            if ((*c >= 'a' && *c <= 'f') || (*c >= 'A' && *c <= 'F') || (*c >= '0' && *c <= '9')) {
                charListAppend(buffer, *c);
                if (buffer->len == 2) {
                    // Convert to byte and return it
                    char* string = charListToString(buffer);
                    *c = (char) strtol(string, NULL, 16);
                    fprintf(stderr, "\nHEX HEX HEX %c\n", *c);
                    free(string);
                    charListDispose(buffer);
                    return ESS_FINAL;
                }
                return ESS_HEX;
            }
            if (buffer->len == 0) {
                charListDispose(buffer);
                return ESS_FINAL_RAW;
            }
            else {
                // Convert to byte and return it
                char* string = charListToString(buffer);
                *unget = *c;
                *c = (char) strtol(string, NULL, 16);
                free(string);
                charListDispose(buffer);
                return ESS_FINAL_NOADD;
            }

        case ESS_OCTAL:
            if (*c >= '0' && *c <= '7') {
                charListAppend(buffer, *c);
                if (buffer->len == 3) {
                    // Convert to byte and return it
                    char* string = charListToString(buffer);
                    *c = (char) strtol(string, NULL, 8);
                    if (*c == '\0') {
                        fprintf(stderr, "\nscanner: String literal cannot contain NULL bytes!\n");
                        exit(1);
                    }
                    free(string);
                    charListDispose(buffer);
                    return ESS_FINAL;
                }
                return ESS_OCTAL;
            }
            if (buffer->len == 0) {
                charListDispose(buffer);
                return ESS_FINAL_RAW;
            }
            else {
                // Convert to byte and return it
                char* string = charListToString(buffer);
                *unget = *c;
                *c = (char) strtol(string, NULL, 8);
                if (*c == '\0') {
                    fprintf(stderr, "\nscanner: String literal cannot contain NULL bytes!\n");
                    exit(1);
                }
                free(string);
                charListDispose(buffer);
                return ESS_FINAL_NOADD;
            }
        default:
            fprintf(stderr, "\nscanner: Bad call to process_str_escape_sequence!");
            exit(1);
    }
}




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
StateInfo get_next_state(ScannerState current_state, int *c, FSMMemory* memory) {
    switch (current_state) {
        case S_PROLOG:
            if (*c == '<') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_PROLOG_L};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_START:
            // Early return for throwing out white space fluff.
            if (isspace(*c)) {
                return (StateInfo) {.result = R_SKIP, .next_state = S_START};
            }

            switch (*c) {
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
                    return (StateInfo) {.result = R_ADD, .next_state = S_START_VARIABLE_ID};

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

                case EOF:
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = END};

                default:
                    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c == '_')){
                        return (StateInfo) {.result = R_ADD, .next_state = S_IDENTIFIER};
                      } 
                      else if((*c >= '0' && *c <= '9')) {
                        return (StateInfo) {.result = R_ADD, .next_state = S_INT_LITERAL};
                      }
                    return (StateInfo) {.result = R_ERROR};
            }

        case S_START_OF_COMMENT_OR_DIV:
            switch (*c) {
                case '/':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_SINGLE_LINE_COMMENT};

                case '*':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_BLOCK_COMMENT};

                default:
                    return (StateInfo) {.result = R_FINAL_NOADD, .lex = DIVIDE};
            }

        case S_SINGLE_LINE_COMMENT:
            switch (*c) {
                case '\n':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_START};

                default:
                    return (StateInfo) {.result = R_SKIP, .next_state = S_SINGLE_LINE_COMMENT};
            }

        case S_BLOCK_COMMENT:
            switch (*c) {
                case '*':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_POSSIBLE_END_OF_BLOCK_COMMENT};
                case EOF:
                    return (StateInfo) {.result = R_ERROR};

                default:
                    return (StateInfo) {.result = R_SKIP, .next_state = S_BLOCK_COMMENT};
            }

        case S_POSSIBLE_END_OF_BLOCK_COMMENT:
            switch (*c) {
                case '*':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_POSSIBLE_END_OF_BLOCK_COMMENT};

                case '/':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_START};

                case EOF:
                    return (StateInfo) {.result = R_ERROR};

                default:
                    return (StateInfo) {.result = R_SKIP, .next_state = S_BLOCK_COMMENT};
            }

        case S_QUESTION_MARK:
            if (*c == '>') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = SCRIPT_STOP};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = QUESTION_MARK};

        case S_START_VARIABLE_ID:
            if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') ||
                (*c >= '0' && *c <= '9') || (*c == '_')) {
                return (StateInfo) {.result = R_ADD, .next_state = S_VARIABLE_ID};
            }
            // Empty variable name
            return (StateInfo) {.result = R_ERROR};

        case S_VARIABLE_ID:
            if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') ||
                (*c >= '0' && *c <= '9') || (*c == '_')) {
                return (StateInfo) {.result = R_ADD, .next_state = S_VARIABLE_ID};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = VAR_ID};

        case S_GREATER_THAN:
            if (*c == '=') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = GREATER_EQUAL};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = GREATER};

        case S_LESS_EQUAL:
            switch (*c) {
                case '=':
                    return (StateInfo) {.result = R_FINAL_SKIP, .lex = LESS_EQUAL};

                case '?':
                    return (StateInfo) {.result = R_SKIP, .next_state = S_PROLOG_LQ};

                default:
                    return (StateInfo) {.result = R_FINAL_NOADD, .lex = LESS};
            }

        case S_PROLOG_L:
            if (*c == '?' || *c == '?') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_PROLOG_LQ};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_PROLOG_LQ:
            if (*c == 'p' || *c == 'P') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_PROLOG_LQP};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_PROLOG_LQP:
            if (*c == 'h' || *c == 'H') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_PROLOG_LQPH};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_PROLOG_LQPH:
            if (*c == 'p' || *c == 'P') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = SCRIPT_START};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_IDENTIFIER:
            if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') ||
                (*c >= '0' && *c <= '9') || (*c == '_')) {
                return (StateInfo) {.result = R_ADD, .next_state = S_IDENTIFIER};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = FUN_ID};

        case S_INT_LITERAL:
            if (*c >= '0' && *c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_INT_LITERAL};
            }
            if (*c == '.') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_DEC};
            }
            if (*c == 'E' || *c == 'e') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_E};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = INT_LIT};

        case S_FLOAT_LITERAL_DEC:
            if (*c == 'E' || *c == 'e') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_E};
            }
            if (*c >= '0' && *c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_DEC};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = FLOAT_LIT};

        case S_FLOAT_LITERAL_E:
            if (*c >= '0' && *c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_E_NUM};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_FLOAT_LITERAL_E_NUM:
            if (*c >= '0' && *c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_E};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = FLOAT_LIT};

        case S_STRING_LITERAL_DQ:
            memory->escape_seq_state = ESS_UNKNOWN;
            if (*c == '"') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = STRING_LIT};
            }
            if (*c == '\\') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_DQ_ESCAPE};
            }
            if (*c == '$') {
                return (StateInfo) {.result = R_ERROR};
            }
            // EOF before string termination
            if (*c == EOF) {
                return (StateInfo) {.result = R_ERROR};
            }
            return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_DQ};

        case S_STRING_LITERAL_DQ_ESCAPE:
            // EOF before string termination
            if (*c == EOF) {
                return (StateInfo) {.result = R_ERROR};
            }

            memory->escape_seq_state = process_str_escape_sequence((char*) c, &memory->memory, memory->escape_seq_state, &memory->unget, '"');
            if (memory->escape_seq_state == ESS_FINAL) {
                return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_DQ};
            }
            if (memory->escape_seq_state == ESS_FINAL_NOADD) {
                return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_DQ};
            }
            if (memory->escape_seq_state == ESS_FINAL_RAW) {
                return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_DQ};
            }
            return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_DQ_ESCAPE};

        case S_STRING_LITERAL_SQ:
            memory->escape_seq_state = ESS_UNKNOWN;
            if (*c == '\'') {
                return (StateInfo) {.result = R_FINAL_SKIP, .lex = STRING_LIT};
            }
            if (*c == '\\') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_SQ_ESCAPE};
            }
            if (*c == '$') {
                return (StateInfo) {.result = R_ERROR};
            }
            // EOF before string termination
            if (*c == EOF) {
                return (StateInfo) {.result = R_ERROR};
            }
            return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_SQ};

        case S_STRING_LITERAL_SQ_ESCAPE:
            // EOF before string termination
            if (*c == EOF) {
                return (StateInfo) {.result = R_ERROR};
            }
            memory->escape_seq_state = process_str_escape_sequence((char*) c, &memory->memory, memory->escape_seq_state, &memory->unget, '\'');
            if (memory->escape_seq_state == ESS_FINAL) {
                return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_SQ};
            }
            if (memory->escape_seq_state == ESS_FINAL_NOADD) {
                return (StateInfo) {.result = R_ADD, .next_state = S_STRING_LITERAL_SQ};
            }
            if (memory->escape_seq_state == ESS_FINAL_RAW) {
                return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_SQ};
            }
            return (StateInfo) {.result = R_SKIP, .next_state = S_STRING_LITERAL_SQ_ESCAPE};

        case S_EQUAL:
            if(*c == '=') {
                return (StateInfo) {.result = R_SKIP, .next_state = S_STRICT_EQUAL};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = ASSIGN};
        case S_STRICT_EQUAL:
            if(*c == '=') {
              return (StateInfo) {.result = R_FINAL_SKIP, .lex = EQUAL};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_CONCAT_OR_FLOAT:
            if(*c >= '0' && *c <= '9') {
                return (StateInfo) {.result = R_ADD, .next_state = S_FLOAT_LITERAL_DEC};
            }
            return (StateInfo) {.result = R_FINAL_NOADD, .lex = CONCAT};

        case S_NOT_EQUAL:
            if(*c == '=') {
              return (StateInfo) {.result = R_SKIP, .next_state = S_NOT_EQUAL_FINAL};
            }
            return (StateInfo) {.result = R_ERROR};

        case S_NOT_EQUAL_FINAL:
            if(*c == '=') {
              return (StateInfo) {.result = R_FINAL_SKIP, .lex = NOT_EQUAL};
            }
              return (StateInfo) {.result = R_ERROR};
    }
    return (StateInfo) {.result = R_ERROR, .next_state = S_START};
}


/**
 * @brief Appends a single character to a c-string buffer. DOES NOT CHECK FOR BOUDS! Buffer has to be initialized.
 * @param c Character you want to append
 * @param buffer The buffer to append to
 **/
void append_to_c_string(char c, char* buffer) {
    int i;
    for (i=0; buffer[i] != '\0'; i++);
    buffer[i] = c;
    buffer[i+1] = '\0';
}


Token scan_next_token(FILE *file, bool expect_prolog)
{
    charList str;
    charListInit(&str);
    Lex returnLex;
    StateInfo state_info;
    ScannerState currentState = expect_prolog ? S_PROLOG : S_START;

    FSMMemory memory;
    memory.escape_seq_state = ESS_UNKNOWN;
    charListInit(&memory.memory);

    int c;
    bool seen_eof = false;
    while (!seen_eof) {
        c = fgetc(file);
        if (c == EOF) seen_eof = true;

        // Calling the lexical state machine implementation to get
        // the next state (or finsihed lex).
        state_info = get_next_state(currentState, &c, &memory);
        currentState = state_info.next_state;

        // Hacking string literal escape sequences to work.
        if(memory.escape_seq_state == ESS_FINAL_RAW) {
            charListAppend(&str, '\\');
            charListFirst(&memory.memory);
            if (memory.memory.len != 0) {
                while (charListGetValue(&memory.memory)) {
                    charListAppend(&str, charListGetValue(&memory.memory));
                    charListNext(&memory.memory);
                }
            }
            charListDispose(&memory.memory);
            charListAppend(&str, c);
        }
        else if (memory.escape_seq_state == ESS_FINAL_NOADD) {
            ungetc(memory.unget, file);
        }

        // Handling data based on the instructions from get_next_state.
        if(state_info.result == R_SKIP) {
            continue;
        }
        else if(state_info.result == R_ADD) {
            charListAppend(&str, c);
            continue;
        }
        else if(state_info.result ==  R_FINAL_ADD) {
            charListAppend(&str, c);
            returnLex = state_info.lex;
            break;
        }
        else if(state_info.result == R_FINAL_NOADD) {
            returnLex = state_info.lex;
            ungetc(c, file);
            break;
        }
        else if(state_info.result == R_FINAL_SKIP) {
            returnLex = state_info.lex;
            break;
        }
        else{
            if (expect_prolog) {
                // TODO What kind of error is this?
                fprintf(stderr, "\nscanner: Missing prolog!\n");
                exit(1);
            }
            char* string = charListToString(&str);
            fprintf(stderr, "\nscanner: Received token \"%s%c\" is invalid, lexical analysis error,"
                    " exiting...\n", string, c);
            free(string);
            exit(1);
        }
    }

    if(c == EOF) {
        return (Token) {
            .lex = END,
        };
    }

    if (returnLex == SCRIPT_STOP) {
        int c = fgetc(file);
        if (c == EOF) {

        }
        else if (c == '\n') {
            c = fgetc(file);
            if (c != EOF) {
                fprintf(stderr, "\nscanner: Expected EOF after epilogue, instead got \"%c\"\n", c);
                exit(1);
            }
        }
        else {
            fprintf(stderr, "\nscanner: Expected EOF after epilogue, instead got \"%c\"\n", c);
            exit(1);
        }
    }

    Token token = {.lex=returnLex};
    // Breaking the independence from the state machine a little.
    // The state machine should ensure that int and float literals will always be valid.
    if (returnLex == INT_LIT) {
        char *temp = charListToString(&str);
        char* endptr;
        token.integer = strtol(temp, &endptr, 10);
        free(temp);
    }
    else if (returnLex == FLOAT_LIT) {
        char* temp = charListToString(&str);
        char* endptr;
        token.decimal = strtod(temp, &endptr);
        free(temp);
    }
    else {
        token.string = charListToString(&str);
    }

    return token;
}

void debug_print_tokens(tokList* list) {
    fprintf(stderr, ">>> BEGIN TOKENS <<<\n");
    fprintf(stderr, "#\tLEX\tTYPE\tDATA\n");
    int i = 0;
    tokElem* current = tokListGetFirst(list);
    while (current != NULL) {
        fprintf(stderr, "#%d\t%d\t", i++, current->data->lex);
        if (current->data->string && current->data->lex != FLOAT_LIT && current->data->lex != INT_LIT) {
            fprintf(stderr, "(text)\t%s", current->data->string);
        }
        else if (current->data->lex == FLOAT_LIT) {
            fprintf(stderr, "(float)\t%f", current->data->decimal);
        }
        else if (current->data->lex == INT_LIT) {
            fprintf(stderr, "(int)\t%lld", current->data->integer);
        }
        fprintf(stderr ,"\n");
        current = current->next;
    }
    fprintf(stderr, ">>> END   TOKENS <<<\n");
}
