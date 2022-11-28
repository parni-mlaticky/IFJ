#include <stdlib.h>
#include "parser.h"

const char* keywords[] = { "if", "else", "declare", "function",
                           "while", "int", "float", "void",
                           "string", "null", "return"};

terminalType lexEnumToTerminalEnum(Lex lex){
    switch(lex){
        case VAR_ID: case FLOAT_LIT: case STRING_LIT: case INT_LIT: case FUN_ID:
            return OP;
        case ADD:
            return PLUS;
        case SUBTRACT:
            return MINUS;
        case MULTIPLY:
            return MUL;
        case DIVIDE:
            return DIV;
        case ASSIGN:
            return AS;
        case EQUAL:
            return EQ;
        case GREATER:
            return G;
        case GREATER_EQUAL:
            return GEQ;
        case LESS:
            return LEQ;
        case LESS_EQUAL:
            return L;
        case NOT_EQUAL:
            return NEQ;
        case CONCAT:
            return CAT;
        case PAR_L:
            return LPAR; 
        case PAR_R:
            return RPAR;  
        default:
            return DOLLAR;                                
    }
}



/* dataType dataTypeCompatibilityCheckOrConversion(Nonterminal* nt1, stackElement* operator, Nonterminal* nt2){
    // In this case it is not an operator but a nonterminal with parentheses around it - (E)
    if(operator->data.etype == NONTERMINAL){
        return operator->data.nonterminal.;
    }

    switch(operator->data.tType){
        case PLUS: case MINUS: case MUL:
            if(nt1->dType == INT && nt2->dType == INT)
                return INT;
            if((nt1->dType == FLOAT || nt2->dType == FLOAT) && (nt1->dType == INT || nt2->dType == INT))
                return FLOAT;
            if(nt1->dType == UNKNOWN || nt2->dType == UNKNOWN)
                return UNKNOWN;    
            else semanticError(7); 
        break;
        case DIV:
            if((nt1->dType == INT || nt1->dType == FLOAT) && (nt2->dType == INT || nt2->dType == FLOAT)){
                return FLOAT;
            }
            else semanticError(7);
        break;

        case CAT:
            if(nt1->dType == STRING && nt2->dType == STRING){
                return STRING;
            }
            else semanticError(7);
        break;    
        case AS:
            return nt2->dType;

        // WE WILL NEED TO IMPLEMENT ANOTHER TYPE (BOOL) FOR THESE (i guess)
        case EQ: case GEQ: case LEQ: case NEQ: case L: case G:
            if((nt1->dType == FLOAT || nt1->dType == INT) && (nt2->dType == FLOAT || nt2->dType == INT)) return INT;
            if(nt1->dType == STRING && nt2->dType == STRING) return INT;     
            else semanticError(7);   
        break;    
        default: exit(99);
    }
    exit(99);
} */

// Implementation of the precedence table
opPrecedence getPrecedence(terminalType stackTerm, Lex nextTerm){
    terminalType lexToTerm = lexEnumToTerminalEnum(nextTerm);
    switch (stackTerm){
        case DOLLAR:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ
            || lexToTerm == NEQ || lexToTerm == GEQ || lexToTerm == LEQ
            || lexToTerm == G || lexToTerm == L || lexToTerm == RPAR
            || lexToTerm == OP || lexToTerm == LPAR){
                return P_LESS;
            }
            else syntaxError();
            break;

        case OP:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV 
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ
            || lexToTerm == NEQ || lexToTerm == GEQ || lexToTerm == LEQ 
            || lexToTerm == G || lexToTerm == L
            || lexToTerm == RPAR || lexToTerm == DOLLAR){
                return P_GREATER;
            }
            else syntaxError();
            break;

        case PLUS: case MINUS: case CAT:
            if(lexToTerm == DIV || lexToTerm == MUL || lexToTerm == LPAR
            || lexToTerm == OP){
                return P_LESS;
            }
            else if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT|| lexToTerm == AS
            || lexToTerm == EQ || lexToTerm == NEQ || lexToTerm == GEQ
            || lexToTerm == LEQ || lexToTerm == G || lexToTerm == L
            || lexToTerm == RPAR || lexToTerm == DOLLAR){
                return P_GREATER;
            }
            else syntaxError();
            break;

        case MUL: case DIV:
            if(lexToTerm == LPAR || lexToTerm == OP){
                return P_LESS;
            }
            else if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ || lexToTerm == NEQ
            || lexToTerm == GEQ || lexToTerm == LEQ || lexToTerm == G || lexToTerm == L
            || lexToTerm == RPAR || lexToTerm == DOLLAR){
                return P_GREATER;
            }
            else syntaxError(); 
            break;

        case AS:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ
            || lexToTerm == NEQ || lexToTerm == GEQ || lexToTerm == LEQ
            || lexToTerm == G || lexToTerm == L || lexToTerm == LPAR
            || lexToTerm == OP){
                return P_LESS;
            }
            else if(lexToTerm == RPAR || lexToTerm == DOLLAR){
                return P_GREATER;
            }
            else syntaxError();
            break;

        case EQ: case NEQ:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == GEQ || lexToTerm == LEQ
            || lexToTerm == G || lexToTerm == L || lexToTerm == LPAR
            || lexToTerm == OP){
                return P_LESS;
            }
            else if(lexToTerm == AS || lexToTerm == EQ || lexToTerm == NEQ
            || lexToTerm == DOLLAR){
                return P_GREATER;
            }
            else syntaxError();
            break;

        case GEQ: case LEQ: case G: case L:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == LPAR || lexToTerm == OP){
                return P_LESS;
            }
            else if(lexToTerm == AS || lexToTerm == EQ || lexToTerm == NEQ || lexToTerm == GEQ
            || lexToTerm == LEQ || lexToTerm == L || lexToTerm == G || lexToTerm == RPAR){
                return P_GREATER;
            }
            else syntaxError();
            break;

        case LPAR:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ || lexToTerm == NEQ
            || lexToTerm == GEQ || lexToTerm == LEQ || lexToTerm == G || lexToTerm == L
            || lexToTerm == LPAR || lexToTerm == OP){
                return P_LESS;
            }
            else if(lexToTerm == RPAR){
                return P_EQ;
            }
            else syntaxError();  
            break;
        case RPAR:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ || lexToTerm == NEQ
            || lexToTerm == GEQ || lexToTerm == LEQ || lexToTerm == G || lexToTerm == L
            || lexToTerm == RPAR || lexToTerm == DOLLAR){
                return P_GREATER;
            }      
            else syntaxError();
            break;
    }

    exit(99);
}

// Operators are >= 4 in the enum
bool isOperator(stackElement* elem){
    if(elem->data.terminal->tType >= 4){
        return true;
    }
    return false;
}

expressionRule precParseCheckRule(stackElement* elem1, stackElement* elem2, stackElement* elem3){
    if(elem1 && !elem2 && !elem3){
        // Rule E => i
        if(elem1->data.etype == TERMINAL && elem1->data.terminal->tType == OP){
            return EXP_TERM;
        }
        else syntaxError();
    }
    if(elem1 && elem2 && elem3){
        // rule E => (E)
        if(elem1->data.terminal->tType == LPAR && elem2->data.etype == NONTERMINAL && elem3->data.terminal->tType == RPAR){
            return EXP_PAR;
        }
        // rule E => E <OP> E
        if(elem1->data.etype == NONTERMINAL && isOperator(elem2) && elem3->data.etype == NONTERMINAL){
            return EXP_OP_EXP;
        }
    }
    return false;
}

bool checkIfLValue(stackElement* elem){
    if(elem->data.etype == NONTERMINAL){
        if(elem->data.nonterminal->NTType == VAR_ID_TERM){
            return true;
        }
    }
    return false;
}



void precParseReduction(stack* s, bool* relOpInExp){
    stackElement* handle = findHandle(s);
    if(!handle) syntaxError();
    stackElement* iter = handle;
    int count = -1;
    while(iter){
        iter = iter->prev;
        count++;
    }
    if(count == 1){
        if(precParseCheckRule(handle->prev, NULL, NULL) == EXP_TERM){
            stackElement* tmp = malloc(sizeof(stackElement));
            memcpy(tmp, handle->prev, sizeof(stackElement));
            stackMultiPop(s, count+1);
            switch(tmp->data.terminal->token->lex){
                case INT_LIT:
                    ;
                    Nonterminal* intLit = createIntLiteralNonterminal(tmp->data.terminal->token->integer);
                    stackPushNonterminal(s, intLit);
                    break;

                case FLOAT_LIT:
                    ;  
                    Nonterminal* floatLit = createFloatLiteralNonterminal(tmp->data.terminal->token->decimal);
                    stackPushNonterminal(s, floatLit);
                    break;

                case STRING_LIT: 
                    ; 
                    Nonterminal* stringLit = createStringLiteralNonterminal(tmp->data.terminal->token->string);
                    stackPushNonterminal(s, stringLit);
                    break;

                case VAR_ID:
                    ;
                    Nonterminal* varNonterm = createVariableNonterminal(tmp->data.terminal->token->string, UNKNOWN); // FIXME
                    stackPushNonterminal(s, varNonterm);
                    break;

                case FUN_ID:
                    ;
                    Nonterminal* funcNonterm = createFuncallNonterminal(tmp->data.terminal->token->string, NULL);
                    (void) funcNonterm; // FIXME Cast to void to supress warning.
                    break;
                default:
                    break;    
            }            
        }
        else syntaxError();
    }
   
    else if(count == 3){
        if(precParseCheckRule(handle->prev, handle->prev->prev, handle->prev->prev->prev) == EXP_OP_EXP){
            if(isRelOperator(handle->prev->prev->data.terminal->tType)) *relOpInExp = true;
            
            
            if(handle->prev->prev->data.terminal->tType == AS){
                if(!checkIfLValue(handle->prev)){
                    syntaxError();
                }
                else{
                    // put variable in symtable TODO
                }
            }

            Nonterminal* operand1 = malloc(sizeof(Nonterminal));
            Nonterminal* operand2 = malloc(sizeof(Nonterminal));
            memcpy(operand1, handle->prev->data.nonterminal, sizeof(Nonterminal));
            memcpy(operand2, handle->prev->prev->prev->data.nonterminal, sizeof(Nonterminal));

            stackElement* op = malloc(sizeof(stackElement));
            memcpy(op, handle->prev->prev, sizeof(stackElement));

            
            Nonterminal* nonterm = createExprNonterminal(operand1, operand2, op->data.terminal->tType);
            stackMultiPop(s, count+1);
            stackPushNonterminal(s, nonterm);
            free(op);
        }
    }   
    else syntaxError();      
}

Nonterminal* createIntLiteralNonterminal(int value){
    Nonterminal* intLit = malloc(sizeof(Nonterminal));
    intLit->NTType = LITERAL_TERM;
    intLit->term.integerLit = value;
    intLit->dType = INT;


    intLit->expr.left = NULL;
    intLit->expr.right = NULL;

    return intLit;
}

Nonterminal* createStringLiteralNonterminal(char* string){
    Nonterminal* stringLit = malloc(sizeof(Nonterminal));
    stringLit->NTType = LITERAL_TERM;
    stringLit->term.stringLit = string;
    stringLit->dType = STRING;

    stringLit->expr.left = NULL;
    stringLit->expr.right = NULL;

    return stringLit;
}

Nonterminal* createFloatLiteralNonterminal(double value){
    Nonterminal* floatLit = malloc(sizeof(Nonterminal));
    floatLit->NTType = LITERAL_TERM;
    floatLit->term.floatLit = value;
    floatLit->dType = FLOAT;

    floatLit->expr.left = NULL;
    floatLit->expr.right = NULL;

    return floatLit;
}

Nonterminal* createFuncallNonterminal(char* funId, nontermList* args){
    Nonterminal* funcNonterm = malloc(sizeof(Nonterminal));
    funcall* func = malloc(sizeof(funcall));
    func->funId = funId;
    func->args = args;


    funcNonterm->NTType = FUNCALL_TERM;

    funcNonterm->expr.left = NULL;
    funcNonterm->expr.right = NULL;
    funcNonterm->expr.op = DOLLAR;

    funcNonterm->term.func = func;
    
    return funcNonterm;
}

Nonterminal* createVariableNonterminal(char* varId, dataType dType){
    Nonterminal* varNonterm = malloc(sizeof(Nonterminal));
    variable* var = malloc(sizeof(variable));
    var->name = varId;
    var->dType = dType;
    

    varNonterm->NTType = VAR_ID_TERM;
    varNonterm->expr.left = NULL;
    varNonterm->expr.right = NULL;
    varNonterm->expr.op = DOLLAR;
    varNonterm->term.var = var;
    return varNonterm;
}

Nonterminal* createExprNonterminal(Nonterminal* left, Nonterminal* right, terminalType operator){
    Nonterminal* newNonterminal = malloc(sizeof(Nonterminal));

    newNonterminal->expr.left = left;
    newNonterminal->expr.right = right;
    newNonterminal->expr.op = operator;

    newNonterminal->NTType = EXPR;
    return newNonterminal;
}


bool precParser(tokList* tl, Nonterminal** finalNonterm){
    bool result = false;
    stack s;
    stackInit(&s);

    // because expression like $a === $b === $c is invalid
    bool relOpInExp = false;
    // counts parentheses in the expression to differentiate between the end par of if() and parentheses in the expression
    int parCount = 0;

    stackPushTerminal(&s, DOLLAR, NULL);
    Token* token;
    stackElement* top;
    while(1){
        token = tokListGetValue(tl);
        if(!token) syntaxError();
        if(((compareLexTypes(token, PAR_R) || compareLexTypes(token, SEMICOLON)) && parCount == 0)){
            if(containsOnlyTopNonterm(&s) || stackIsEmpty(&s)){
                break;
            }
            else{
                while(!containsOnlyTopNonterm(&s)){
                    precParseReduction(&s, &relOpInExp);
                }
                break;
            }
        }

        top = getTopTerminal(&s);
        if(isRelOperator(lexEnumToTerminalEnum(token->lex)) && relOpInExp) syntaxError();
        opPrecedence prec = getPrecedence(top->data.terminal->tType, token->lex);
        if(prec == P_LESS){
            stackInsertHandle(&s);
            // TODO
            if(token->lex == FUN_ID){
                stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            }
            else if(token->lex == STRING_LIT || token->lex == INT_LIT || token->lex == FLOAT_LIT){
                stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            }
            else if(token->lex == VAR_ID){
                stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            }
            else if(token->lex == PAR_L){
                stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
                parCount++;
            }
            else if(token->lex == PAR_R){
                stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
                parCount--;
            }
            else{
                stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            }   
            getNextToken(tl);
        }
        else if(prec == P_GREATER){
            precParseReduction(&s, &relOpInExp);
        }
        else if(prec == P_EQ){
            stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            parCount--;
            getNextToken(tl);
        }
    }
    if(containsOnlyTopNonterm(&s) && token->lex == PAR_R){
        result = true;
    }
    if((containsOnlyTopNonterm(&s) || stackIsEmpty(&s)) && token->lex == SEMICOLON){
        result = true;
    }
    if(result){
        *finalNonterm = s.top->data.nonterminal;
    }
    else{
        *finalNonterm = NULL;
    }
    return result;
}

bool parse_file(FILE* file) {
    tokList* list = malloc(sizeof(tokList));
    tokListInit(list);
    Token token;
    do {
        Token* new = malloc(sizeof(Token));
        *new = scan_next_token(file, false);
        token = *new;
        tokListAppend(list, new);
    } while (token.lex != END);
    debug_print_tokens(list);
    return recursiveDescent(list);
}



bool isKeyword(Token* t){
    if(!compareLexTypes(t, FUN_ID)) return false;
    for(int i = 0; i < KEYWORD_COUNT; i++){
        if(!strcmp(keywords[i], t->string)){
            return true;
        }
    }
    return false;
}

void syntaxError(){
    fprintf(stderr, "Syntax error, exiting...\n");
    exit(2);
}

void semanticError(int code){
    fprintf(stderr, "Semantic error code %d, exiting...\n", code);
    exit(code);
}


Token* getNextToken(tokList* tl){
    Token* t = tl->active ? tokListGetValue(tl) : NULL;
    tokListNext(tl);
    return t;
}

bool compareLexTypes(Token* inputTerminal, Lex stackTerminal){
    if(!inputTerminal){
        return false;
    }
    return (inputTerminal->lex == stackTerminal);
}

bool compareTerminalStrings(Token* inputTerminal, char* stackTerminal){
    if(!inputTerminal) return false;
    if(inputTerminal->lex != FUN_ID) return false;
    int result = strcmp(inputTerminal->string, stackTerminal);
    if(result == 0) return true;
    return false;
}

bool recursiveDescent(tokList* tl){
    tokListFirst(tl);
    return progExpansion(tl);
}


bool progExpansion(tokList* tl){
    bool prog = false;
    Token* t = getNextToken(tl);
    prog = compareLexTypes(t, SCRIPT_START) && declareStExpansion(tl)
    && STListExpansion(tl) && endTokenExpansion(tl);
    return prog;
}

bool declareStExpansion(tokList* tl){
    bool declareSt;
    Token* t;

    t = getNextToken(tl);
    declareSt = compareLexTypes(t, FUN_ID) && compareTerminalStrings(t, "declare");

    t = getNextToken(tl);
    declareSt = declareSt && compareLexTypes(t, PAR_L);

    t = getNextToken(tl);
    declareSt = declareSt && compareLexTypes(t, FUN_ID) && compareTerminalStrings(t, "strict_types");

    t = getNextToken(tl);
    declareSt = declareSt && compareLexTypes(t, ASSIGN);

    t = getNextToken(tl);
    declareSt = declareSt && compareLexTypes(t, INT_LIT);
    if(t) declareSt = declareSt && (t->integer == 1);

    t = getNextToken(tl);
    declareSt = declareSt && compareLexTypes(t, PAR_R);

    t = getNextToken(tl);
    declareSt = declareSt && compareLexTypes(t, SEMICOLON);

    return declareSt;
}

bool STListExpansion(tokList* tl){
    bool STList = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, END)) STList = true;
    else if(compareLexTypes(t, SCRIPT_STOP)) STList = true;
    else if(compareLexTypes(t, CBR_R)) STList = true;
    else STList = STExpansion(tl) && STListExpansion(tl);
    return STList;
}

bool blockSTListExpansion(tokList* tl){
    bool blockSTList = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, END)) blockSTList = true;
    else if(compareLexTypes(t, SCRIPT_STOP)) blockSTList = true;
    else if(compareLexTypes(t, CBR_R)) blockSTList = true;
    else blockSTList = blockSTExpansion(tl) && blockSTListExpansion(tl);
    return blockSTList;
}

bool blockSTExpansion(tokList* tl){
    bool blockSt = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, VAR_ID)){
        Nonterminal* expTree;
        blockSt = precParser(tl, &expTree);
        t = getNextToken(tl);
        blockSt = blockSt && compareLexTypes(t, SEMICOLON);
    }
    else if(compareLexTypes(t, FUN_ID)){
        if(compareTerminalStrings(t, "if")) blockSt = ifStExpansion(tl);
        else if(compareTerminalStrings(t, "while")) blockSt = whileStExpansion(tl);
        else if(compareTerminalStrings(t, "return")) blockSt = returnStExpansion(tl);
        else if(compareTerminalStrings(t, "function")) blockSt = false;
        else{ 
            Nonterminal* expTree;
            blockSt = precParser(tl, &expTree);
            t = getNextToken(tl);
            blockSt = blockSt && compareLexTypes(t, SEMICOLON);
        }    
    }
    else if(compareLexTypes(t, INT_LIT) || compareLexTypes(t, FLOAT_LIT) || compareLexTypes(t, STRING_LIT)){
        Nonterminal* expTree;
        blockSt = precParser(tl, &expTree);
        t = getNextToken(tl);
        blockSt = blockSt && compareLexTypes(t, SEMICOLON);
    }
    return blockSt;
}

bool STExpansion(tokList* tl){
    bool St = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, VAR_ID)){
        Nonterminal* expTree;
        St = precParser(tl, &expTree);
        t = getNextToken(tl);
        St = St && compareLexTypes(t, SEMICOLON);
    }
    else if(compareLexTypes(t, FUN_ID)){
        if(compareTerminalStrings(t, "function")) St = functionDefStExpansion(tl);
        else if(compareTerminalStrings(t, "if")) St = ifStExpansion(tl);
        else if(compareTerminalStrings(t, "while")) St = whileStExpansion(tl);
        else if(compareTerminalStrings(t, "return")) St = returnStExpansion(tl);
        else{
            Nonterminal* expTree;
             St = precParser(tl, &expTree);
             t = getNextToken(tl);
             St = St && compareLexTypes(t, SEMICOLON);
        }     
    }

    else{
        Nonterminal* expTree;
        St = precParser(tl, &expTree);
        t = getNextToken(tl);
        St = St && compareLexTypes(t, SEMICOLON);
    }    
    return St;
}

bool endTokenExpansion(tokList* tl){
    bool end = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, SCRIPT_STOP) || compareLexTypes(t, END)) end = true;
    return end;
}


bool functionDefStExpansion(tokList* tl){
    bool fDefSt = false;
    Token* t = getNextToken(tl);
    fDefSt = compareTerminalStrings(t, "function");
    t = getNextToken(tl);
    fDefSt = fDefSt && compareLexTypes(t, FUN_ID) && !isKeyword(t);
    t = getNextToken(tl);
    fDefSt = fDefSt && compareLexTypes(t, PAR_L);
    fDefSt = fDefSt && paramsExpansion(tl);
    t = getNextToken(tl);
    fDefSt = fDefSt && compareLexTypes(t, PAR_R);
    t = getNextToken(tl);
    fDefSt = fDefSt && compareLexTypes(t, COLON) && typeExpansion(tl) && blockExpansion(tl);
    return fDefSt;
}

bool ifStExpansion(tokList* tl){
    bool ifSt;
    Token* t = getNextToken(tl);
    ifSt = compareTerminalStrings(t, "if");
    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, PAR_L);

    Nonterminal* expTree;
    ifSt = ifSt && precParser(tl, &expTree);

    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, PAR_R);
    ifSt = ifSt && blockExpansion(tl);

    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, FUN_ID);    
    if(!ifSt) return ifSt;
    ifSt = ifSt && compareTerminalStrings(t, "else");
    ifSt = ifSt && blockExpansion(tl);  
    return ifSt;
}

bool whileStExpansion(tokList* tl){
    bool whileSt;
    Token* t;

    t = getNextToken(tl);
    whileSt = compareTerminalStrings(t, "while");
    t = getNextToken(tl);
    whileSt = whileSt && compareLexTypes(t, PAR_L);

    Nonterminal* expTree;
    whileSt = whileSt && precParser(tl, &expTree);

    t = getNextToken(tl);
    whileSt = whileSt && compareLexTypes(t, PAR_R) && blockExpansion(tl);
    return whileSt;
}

bool returnStExpansion(tokList* tl){
    bool returnSt;
    Token* t;

    t = getNextToken(tl);
    returnSt = compareTerminalStrings(t, "return");

    Nonterminal* expTree;
    returnSt = returnSt && precParser(tl, &expTree);

    t = getNextToken(tl);
    returnSt = returnSt && compareLexTypes(t, SEMICOLON);
    return returnSt;
}

bool varExpansion(tokList* tl){
    Token* t = getNextToken(tl);
    return compareLexTypes(t, VAR_ID);
}

bool paramsExpansion(tokList* tl){
    bool params = false;
    Token* t;
    t = tokListGetValue(tl);
    if(compareLexTypes(t, PAR_R)) params = true;
    else{
        params = typeExpansion(tl);
        params = params && varExpansion(tl) && paramListExpansion(tl);
    }
    return params;
}

bool paramListExpansion(tokList* tl){
    bool paramList = false;
    Token* t;
    t = tokListGetValue(tl);
    if(compareLexTypes(t, PAR_R)) paramList = true;
    else{
        t = getNextToken(tl);
        paramList = compareLexTypes(t, COMMA) && typeExpansion(tl) && varExpansion(tl) && paramListExpansion(tl);
    }
    return paramList;
}

bool typeExpansion(tokList* tl){
    bool type = false;
    bool questionMark = false;
    Token* t;

    t = tokListGetValue(tl);
    if(compareLexTypes(t, QUESTION_MARK)){
        questionMark = true;
        getNextToken(tl);
    }
    type = typeNameExpansion(tl, questionMark);
    return type;
}


bool typeNameExpansion(tokList* tl, bool questionMark){
    bool typeName;
    Token* t;
    t = getNextToken(tl);
    typeName = compareLexTypes(t, FUN_ID);
    typeName = typeName && (compareTerminalStrings(t, "int") || compareTerminalStrings(t, "float") || 
                            (compareTerminalStrings(t, "void") && !questionMark) || compareTerminalStrings(t, "string"));
    return typeName;                        
}

bool blockExpansion(tokList* tl){
    bool block = false;
    Token* t;
    t = getNextToken(tl);
    block = compareLexTypes(t, CBR_L) && blockSTListExpansion(tl);
    t = getNextToken(tl);
    block = block && compareLexTypes(t, CBR_R);
    return block;
}


bool isRelOperator(terminalType tType){
    if(tType == EQ || tType == NEQ || tType == G
    || tType == L || tType == LEQ || tType == GEQ){
        return true;
    }
    return false;
}

void debugPrintExprTree(Nonterminal* root){
    if(!root->expr.left && !root->expr.right){
        switch(root->NTType){
            case VAR_ID_TERM:
                printf("%s", root->term.var->name);
                break;
            case LITERAL_TERM:
                switch(root->dType){
                    case STRING:
                        fprintf(stderr, "%s", root->term.stringLit);
                        break;
                    case INT:
                        fprintf(stderr, "%d", root->term.integerLit);
                        break;
                    case FLOAT:
                        fprintf(stderr, "%lf", root->term.floatLit);
                        break;  
                    default:
                        break;        
                }
            default: break;    
        }
    }
    else{
        printf("\tEXPR\n");
        debugPrintExprTree(root->expr.left);
        switch(root->expr.op){
            case PLUS:
                fprintf(stderr, "\t+\t");
                break;
            case MINUS:
                fprintf(stderr, "\t-\t");
                break;
            default: break;       
        }
        debugPrintExprTree(root->expr.right);
    }
}
