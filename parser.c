#include "parser.h"

static ht_table_t symtable;

const char* keywords[] = { "if", "else", "declare", "function",
                           "while", "int", "float", "void",
                           "string", "null", "return"};



void addFuncToSymtable(char* name, varList* argList, bool nullable, dataType returnType, ht_table_t* localTable){
    symtableElem* functionElem = malloc(sizeof(symtableElem));
    function* func = malloc(sizeof(function));
    func->functionName = name;
    func->args = argList;
    func->localTable = localTable;
    func->nullable = nullable;
    func->returnType = returnType;
    functionElem->type = FUNCTION;
    functionElem->f = func;
    ht_insert(&symtable, name, functionElem);
}

void addBuiltinFunctionsToSymtable(){
    // reads(), readi(), readf()
    varList* argList = malloc(sizeof(varList));
    varListInit(argList);
    addFuncToSymtable("reads", argList, true, STRING, NULL);
    addFuncToSymtable("readi", argList, true, INT, NULL);
    addFuncToSymtable("readf", argList, true, FLOAT, NULL);

    // write()
    addFuncToSymtable("write", NULL, true, NULL_T, NULL);

    // floatval(), intval(), strval()
    argList = malloc(sizeof(varList));
    varListInit(argList);
    variable term = {.dType = UNKNOWN, .name = "term", .nullable = true};
    varListAppend(argList, term);
    addFuncToSymtable("floatval", argList, false, FLOAT, NULL);
    addFuncToSymtable("intval", argList, false, INT, NULL);
    addFuncToSymtable("strval", argList, false, STRING, NULL);

    // strlen()
    argList = malloc(sizeof(varList));
    varListInit(argList);
    variable string = {.dType = STRING, .name = "s", .nullable = false};
    varListAppend(argList, string);
    addFuncToSymtable("strlen", argList, false, INT, NULL);

    // substring()
    argList = malloc(sizeof(varList));
    varListInit(argList);
    varListAppend(argList, string);
    variable i = {.dType = INT, .name = "i", .nullable = false};
    variable j = {.dType = INT, .name = "j", .nullable = false};
    varListAppend(argList, i);
    varListAppend(argList, j);
    addFuncToSymtable("substring", argList, true, STRING, NULL);

    // ord()
    argList = malloc(sizeof(varList));
    varListInit(argList);
    varListAppend(argList, string);
    addFuncToSymtable("ord", argList, false, INT, NULL);

    // chr()
    argList = malloc(sizeof(varList));
    varListInit(argList);
    varListAppend(argList, i);
    addFuncToSymtable("chr", argList, false, STRING, NULL);
}

// ONE BIG FIXME
bool firstPass(tokList* tl){
    char* funcName = NULL;
    bool fPass = true;
    dataType returnType;
    bool nullable = false;
    varList* args;
    ht_table_t* localTable;
    symtableElem* var;
    tokListFirst(tl);
    Token* t = tokListGetValue(tl);
    while(!compareLexTypes(t, END) && !compareLexTypes(t, SCRIPT_STOP)){
        while(!compareLexTypes(t, FUN_ID) || !compareTerminalStrings(t, "function")){
            if(compareLexTypes(t, END) || compareLexTypes(t, SCRIPT_STOP)) break;
            t = getNextToken(tl);
        }
        if(compareLexTypes(t, END) || compareLexTypes(t, SCRIPT_STOP)) break;

        args = malloc(sizeof(varList));
        varListInit(args);
        fPass = compareTerminalStrings(t, "function");
        t = getNextToken(tl);
        fPass = fPass && compareLexTypes(t, FUN_ID) && !isKeyword(t);
        funcName = t->string;
        t = getNextToken(tl);
        fPass = fPass && compareLexTypes(t, PAR_L);
        fPass = fPass && paramsExpansion(tl, args);
        t = getNextToken(tl);
        fPass = fPass && compareLexTypes(t, PAR_R);
        t = getNextToken(tl);
        fPass = fPass && compareLexTypes(t, COLON) && typeExpansion(tl, &returnType, &nullable, true);
        if(!fPass) syntaxError(NULL, "Function definition error\n");
        if(ht_get(&symtable, funcName)) semanticError(3);

    localTable = calloc(sizeof(ht_table_t), 1);
    varListFirst(args);
    variable varIter;
    while(args->active){ // FIXME LIST ISACTIVE METHOD!!!
        varIter = varListGetValue(args);
        if(!varIter.name){
            syntaxError(NULL, "error in params in function declaration");
        }
        var = ht_get(localTable, varIter.name);
        if(var) semanticError(8);
        var = malloc(sizeof(symtableElem));
        var->type = VARIABLE;
        var->v = variable_clone(&varIter);
        ht_insert(localTable, varIter.name, var);
        varListNext(args);
    }
        addFuncToSymtable(funcName, args, nullable, returnType, localTable);

        Token* iter = getNextToken(tl);
        int count = 0;
        if(!compareLexTypes(iter, CBR_L)){
            syntaxError(NULL, "Error in function definition\n");
        }

        if(!fPass){
            syntaxError(NULL, "Error in function definition\n");
        }
        count++;
        while(count > 0){
            iter = getNextToken(tl);
            if(compareLexTypes(iter, CBR_L)){
                count++;
            }
            else if(compareLexTypes(iter, CBR_R)){
                count--;
            }
            else if(!iter){
                syntaxError(NULL, "Error in function definition\n");
            }
        }
        t = getNextToken(tl);
    }    
    return fPass;
}


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
            return L;
        case LESS_EQUAL:
            return LEQ;
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
            else syntaxError(NULL, "getPrecedence DOLLAR case error");
            break;

        case OP:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV 
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ
            || lexToTerm == NEQ || lexToTerm == GEQ || lexToTerm == LEQ 
            || lexToTerm == G || lexToTerm == L
            || lexToTerm == RPAR || lexToTerm == DOLLAR){
                return P_GREATER;
            }
            else syntaxError(NULL, "getPrecedence OP case error");
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
            else syntaxError(NULL, "getPrecedence PLUS/MINUS/CAT case error");
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
            else syntaxError(NULL, "getPrecedence MUL/DIV case error"); 
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
            else syntaxError(NULL, "getPrecedence AS case error");
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
            else syntaxError(NULL, "getPrecedence EQ/NEQ case error");
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
            else syntaxError(NULL, "getPrecedence GEQ/LEQ/G/L case error");
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
            else syntaxError(NULL, "getPrecedence LPAR case error");  
            break;
        case RPAR:
            if(lexToTerm == PLUS || lexToTerm == MINUS || lexToTerm == CAT || lexToTerm == DIV
            || lexToTerm == MUL || lexToTerm == AS || lexToTerm == EQ || lexToTerm == NEQ
            || lexToTerm == GEQ || lexToTerm == LEQ || lexToTerm == G || lexToTerm == L
            || lexToTerm == RPAR || lexToTerm == DOLLAR){
                return P_GREATER;
            }      
            else syntaxError(NULL, "getPrecedence RPAR case error");
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
        else syntaxError(NULL, "precParseCheckRule error");
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
    if(!handle) syntaxError(NULL, "precParseReduction handle missing error");
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
                    if(compareTerminalStrings(tmp->data.terminal->token, "null")){
                        Nonterminal* nullNonterm = createNullNonterminal();
                        stackPushNonterminal(s, nullNonterm);
                    }
                    else{
                        Nonterminal* funcNonterm = createFuncallNonterminal(tmp->data.terminal->token->string, tmp->data.terminal->funcArgs);
                        stackPushNonterminal(s, funcNonterm);
                    }
      
                    break;
                default:
                    break;    
            }            
        }
        else syntaxError(NULL, "precParseReduction precParseCheckRule error");
    }
   
    else if(count == 3){
        if(precParseCheckRule(handle->prev, handle->prev->prev, handle->prev->prev->prev) == EXP_OP_EXP){
            if(isRelOperator(handle->prev->prev->data.terminal->tType)) *relOpInExp = true;
            
            
            if(handle->prev->prev->data.terminal->tType == AS){
                if(!checkIfLValue(handle->prev)){
                    syntaxError(NULL, "precParseReduction checkIfLValue error");
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
        else if(precParseCheckRule(handle->prev, handle->prev->prev, handle->prev->prev->prev) == EXP_PAR){
            Nonterminal* exp = malloc(sizeof(Nonterminal));
            memcpy(exp, handle->prev->prev->data.nonterminal, sizeof(Nonterminal));
            stackMultiPop(s, count+1);
            stackPushNonterminal(s, exp);
        }
    }   
    else syntaxError(NULL, "precParseReduction count error");      
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

Nonterminal* createNullNonterminal(){
    Nonterminal* nullNonterm = malloc(sizeof(Nonterminal));

    nullNonterm->NTType = LITERAL_TERM;
    nullNonterm->dType = NULL_T;
    nullNonterm->expr.left = NULL;
    nullNonterm->expr.right = NULL;
    nullNonterm->expr.op = DOLLAR;
    
    return nullNonterm;
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
    Nonterminal* newNonterminal = calloc(sizeof(Nonterminal), 1);

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
    bool emptyExpr = true;
    Token* token;
    stackElement* top;
    while(1){
        token = tokListGetValue(tl);
        if(!token) syntaxError(token, "precParser missing Token error");
        if(((compareLexTypes(token, PAR_R) || compareLexTypes(token, SEMICOLON) || compareLexTypes(token, COMMA)) && parCount == 0)){
            if(containsOnlyTopNonterm(&s) || stackIsEmpty(&s)){
                break;
            }
            else{
                while(!containsOnlyTopNonterm(&s)){
                    precParseReduction(&s, &relOpInExp);
                    emptyExpr = false;
                }
                break;
            }
        }

        top = getTopTerminal(&s);
        if(isRelOperator(lexEnumToTerminalEnum(token->lex)) && relOpInExp) semanticError(7);
        opPrecedence prec = getPrecedence(top->data.terminal->tType, token->lex);
        if(prec == P_LESS){
            stackInsertHandle(&s);
            // TODO
            if(token->lex == FUN_ID){
                if(compareTerminalStrings(token, "null")){
                    stackPushTerminal(&s, OP, token);
                }
                else{
                    Token *nextToken = getNextToken(tl); // name of the function
                    nontermList* args = malloc(sizeof(nontermList));
                    nontermListInit(args);

                    nextToken = getNextToken(tl);       // should be '('
                            
                    Nonterminal *nonTerm;
                    
                    // there has to be a '(' while calling a function
                    if(nextToken->lex != PAR_L) {
                        syntaxError(nextToken, "precParser missing '(' while calling a function error");
                    }

                    nextToken = tokListGetValue(tl);       // should be '('

                    while(nextToken->lex != PAR_R) {

                        //unclosed function call
                        if(nextToken->lex == END) {
                            syntaxError(nextToken, "precParser ')' missing at the end of a function call");
                        }
                    //fprintf(stderr, "tl.active is: %d\n", tl->active->data->lex);
                    if(precParser(tl, &nonTerm)) {
                        nontermListAppend(args, nonTerm);
                        nextToken = tokListGetValue(tl);

                        if(nextToken->lex == PAR_R) {
                            break;
                        }
        
                        nextToken = getNextToken(tl);
                        //fprintf(stderr, "next token is: %d\n", nextToken->lex);
                    } else {
                        syntaxError(nextToken, "precParser precParser returned false error");
                    }
                    }
                    //recursively syntax check args, ',' (probably)
                    //check for rpar
                    //stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
                    stackPushFuncallTerminal(&s, token, args);
                }
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
            emptyExpr = false;
        }
        else if(prec == P_EQ){
            stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            parCount--;
            getNextToken(tl);
        }
    }
    if(containsOnlyTopNonterm(&s) && (token->lex == PAR_R || token->lex == COMMA)){
        result = true;
    }
    if((containsOnlyTopNonterm(&s) || stackIsEmpty(&s)) && token->lex == SEMICOLON){
        result = true;
    }
    if(result && !emptyExpr){
        *finalNonterm = s.top->data.nonterminal;
        // FIXME compileTimeExpressionEval(*finalNonterm);
    }
    else{
        Nonterminal* empty = malloc(sizeof(Nonterminal));
        empty->NTType = EMPTY;
        *finalNonterm = empty;
    }
    return result;
}

bool parse_file(FILE* file) {
    bool result = false;
    tokList* list = malloc(sizeof(tokList));
    tokListInit(list);
    Token token;
    do {
        Token* new = malloc(sizeof(Token));
        *new = scan_next_token(file, false);
        token = *new;
        tokListAppend(list, new);
    } while (token.lex != END);
    ht_init(&symtable);
    generateStarterAsm();
    addBuiltinFunctionsToSymtable();
    result = firstPass(list);
    tokListFirst(list);

    printf("LABEL %%PROG_START\n");
    printf("CREATEFRAME\n" "PUSHFRAME\n");
    printf("CALL %%MAIN_VAR_DEFS\n");

    result = result && recursiveDescent(list);

    printf("EXIT int@0\n");
    printf("LABEL %%MAIN_VAR_DEFS\n");
    defineFunctionVars(symtable);
    printf("RETURN\n");

    return result;
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

void syntaxError(Token* errorToken, char* errMessage){
  if(!errorToken) {
    fprintf(stderr, "Syntax error, exiting with error message:\n%s\n", errMessage);
    exit(2);
  }
    fprintf(stderr, "Syntax error, exiting with error message:\n%s with token: %d\n",errMessage, errorToken->lex);
    exit(2);
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

bool blockSTListExpansion(tokList* tl, function* func){
    bool blockSTList = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, END)) blockSTList = true;
    else if(compareLexTypes(t, SCRIPT_STOP)) blockSTList = true;
    else if(compareLexTypes(t, CBR_R)) blockSTList = true;
    else blockSTList = blockSTExpansion(tl, func) && blockSTListExpansion(tl, func);
    return blockSTList;
}

bool blockSTExpansion(tokList* tl, function* func){
    bool blockSt = false;
    ht_table_t* localSymtable = &symtable;
    if(func){
        localSymtable = func->localTable;
    }


    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, VAR_ID)){
        Nonterminal* expTree;
        blockSt = precParser(tl, &expTree);

        processPossibleVariableDefinition(expTree, localSymtable);

        t = getNextToken(tl);
        generateExpressionCode(expTree, false, localSymtable, &symtable);
        blockSt = blockSt && compareLexTypes(t, SEMICOLON);
        printf("POPS GF@%%RAX\n");
    }
    else if(compareLexTypes(t, FUN_ID)){
        if(compareTerminalStrings(t, "if")) blockSt = ifStExpansion(tl, func);
        else if(compareTerminalStrings(t, "while")) blockSt = whileStExpansion(tl, func);
        else if(compareTerminalStrings(t, "return")) blockSt = returnStExpansion(tl, func);
        else if(compareTerminalStrings(t, "function")) blockSt = false;
        else{ 
            Nonterminal* expTree;
            blockSt = precParser(tl, &expTree);
            processPossibleVariableDefinition(expTree, localSymtable);
            t = getNextToken(tl);
            generateExpressionCode(expTree, false, localSymtable, &symtable);
            blockSt = blockSt && compareLexTypes(t, SEMICOLON);
            printf("POPS GF@%%RAX\n");
        }    
    }
    else {
        Nonterminal* expTree;
        blockSt = precParser(tl, &expTree);
        processPossibleVariableDefinition(expTree, localSymtable);
        t = getNextToken(tl);
        generateExpressionCode(expTree, false, localSymtable, &symtable);
        blockSt = blockSt && compareLexTypes(t, SEMICOLON);
        printf("POPS GF@%%RAX\n");
    }
    return blockSt;
}

void processPossibleVariableDefinition(Nonterminal* expTree, ht_table_t* symtable) {
    // Check if this is really is an assignment of a variable.
    if (expTree == NULL)                           return;
    if (expTree->NTType != EXPR)                   return;
    if (expTree->expr.op != AS)                    return;
    if (expTree->expr.left == NULL)                return;
    if (expTree->expr.left->NTType != VAR_ID_TERM) return;
    processPossibleVariableDefinition(expTree->expr.right, symtable);
    // Variable assignment found
    Nonterminal* var = expTree->expr.left;
    
    // Check if variable was already added into the table
    symtableElem* elem = ht_get(symtable, var->term.var->name);

    //varList* definedVars = malloc(sizeof(varList));
    //varListInit(definedVars);
    if (elem == NULL) {
        // CREATING VAR LIST FOR DEFINING AT THE END OF THE FUNCTION
        elem = malloc(sizeof(symtableElem));
        elem->type = VARIABLE;
        elem->v    = variable_clone(var->term.var);
        ht_insert(symtable, var->term.var->name, elem);
    }
    else {
        elem->v->dType = var->dType;
    }
    //vars = definedVars;
}

bool STExpansion(tokList* tl){
    bool St = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, VAR_ID)){
        Nonterminal* expTree;
        St = precParser(tl, &expTree);
        processPossibleVariableDefinition(expTree, &symtable);
        t = getNextToken(tl);
        generateExpressionCode(expTree, false, &symtable, &symtable);
        St = St && compareLexTypes(t, SEMICOLON);
        printf("POPS GF@%%RAX\n");
    }
    else if(compareLexTypes(t, FUN_ID)){
        if(compareTerminalStrings(t, "function")) St = functionDefStExpansion(tl);
        else if(compareTerminalStrings(t, "if")) St = ifStExpansion(tl, NULL);
        else if(compareTerminalStrings(t, "while")) St = whileStExpansion(tl, NULL);
        else if(compareTerminalStrings(t, "return")) St = returnStExpansion(tl, NULL);
        else{
            Nonterminal* expTree;
            St = precParser(tl, &expTree);
            processPossibleVariableDefinition(expTree, &symtable);
            t = getNextToken(tl);
            generateExpressionCode(expTree, false, &symtable, &symtable);
            St = St && compareLexTypes(t, SEMICOLON);
            printf("POPS GF@%%RAX\n");
        }     
    }

    else{
        Nonterminal* expTree;
        St = precParser(tl, &expTree);
        processPossibleVariableDefinition(expTree, &symtable);
        t = getNextToken(tl);
        generateExpressionCode(expTree, false, &symtable, &symtable);
        St = St && compareLexTypes(t, SEMICOLON);
        printf("POPS GF@%%RAX\n");
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
    char* funcName = NULL;
    getNextToken(tl);
    Token* t = getNextToken(tl);
    funcName = t->string;
    printf("JUMP _%s_FUNC_END\n", funcName);
    printf("LABEL _%s\n", funcName);
    printf("CALL _%s_VAR_DEFS\n", funcName);
    function* funkce = ht_get(&symtable, funcName)->f;
    varListFirst(funkce->args);
    variable argsIter;
    for(int i = 0; i < funkce->args->len; i++){
        argsIter = varListGetValue(funkce->args);
        printf("POPS LF@%s\n", argsIter.name);
        printf("PUSHS LF@%s\n", argsIter.name);
        printf("PUSHS string@%s\n", enumTypeToStr(argsIter.dType));
        if(!argsIter.nullable){
            printf("CALL %%CHECK_IF_IS_TYPE\n");
        }
        else{
            printf("CALL %%CHECK_IF_IS_TYPE_OR_NULL\n");
        }
        printf("PUSHS bool@false\n");
        printf("JUMPIFEQS %%ERROR_4\n");
        varListNext(funkce->args);
    }

    while(tl->active->data->lex != CBR_L){
        getNextToken(tl);
    }

    fDefSt = blockExpansion(tl, funkce);
    if(funkce->returnType != NULL_T){
        printf("JUMP %%ERROR_6\n"); // FIXME this should be a special label that we will jump to i guess
    }
    else{
        printf("PUSHS nil@nil\n");
        printf("RETURN\n");
    }
    printf("LABEL _%s_VAR_DEFS\n", funkce->functionName);
    defineFunctionVars(*funkce->localTable);
    printf("RETURN\n");
    printf("LABEL _%s_FUNC_END\n", funkce->functionName);
    return fDefSt;
}

bool ifStExpansion(tokList* tl, function* func){
    ht_table_t* localSymtab;
    if(func){
        localSymtab = func->localTable;
    }
    else{
        localSymtab = &symtable;
    }

    static int ifCount = 0;
    int currentIfId = ifCount;
    ifCount++; 
    bool ifSt;
    Token* t = getNextToken(tl);
    ifSt = compareTerminalStrings(t, "if");
    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, PAR_L);

    Nonterminal* expTree;
    ifSt = ifSt && precParser(tl, &expTree);
    processPossibleVariableDefinition(expTree, localSymtab);
    generateExpressionCode(expTree, false, localSymtab, &symtable);

    printf("CALL %%TO_BOOL\n");
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS ELSE%d\n", currentIfId);
    printf("LABEL IF%d\n", currentIfId);

    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, PAR_R);
    ifSt = ifSt && blockExpansion(tl, func);

    printf("JUMP IF%dEND\n", currentIfId);

    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, FUN_ID);    
    if(!ifSt) return ifSt;
    ifSt = ifSt && compareTerminalStrings(t, "else");

    printf("LABEL ELSE%d\n", currentIfId);

    ifSt = ifSt && blockExpansion(tl, func);
    printf("LABEL IF%dEND\n", currentIfId); 
    return ifSt;
}

bool whileStExpansion(tokList* tl, function* func){

    ht_table_t* localSymtable = &symtable;
    if(func){
        localSymtable = func->localTable;
    }
    static int whileCount = 0;
    int currentWhileId = whileCount;
    whileCount++;
    bool whileSt;
    Token* t;

    t = getNextToken(tl);
    whileSt = compareTerminalStrings(t, "while");
    t = getNextToken(tl);
    whileSt = whileSt && compareLexTypes(t, PAR_L);

    Nonterminal* expTree;
    whileSt = whileSt && precParser(tl, &expTree);
    processPossibleVariableDefinition(expTree, localSymtable);

    printf("LABEL WHILE%d\n", currentWhileId);
    generateExpressionCode(expTree, false, localSymtable, &symtable);
    printf("CALL %%TO_BOOL\n");
    printf("PUSHS bool@false\n");

    printf("JUMPIFEQS WHILE%dEND\n", currentWhileId);

    t = getNextToken(tl);
    whileSt = whileSt && compareLexTypes(t, PAR_R) && blockExpansion(tl, func);

    printf("JUMP WHILE%d\n", currentWhileId);
    printf("LABEL WHILE%dEND\n", currentWhileId);

    return whileSt;
}

bool returnStExpansion(tokList* tl, function* func){
    ht_table_t* localSymtable = &symtable;
    if(func){
        localSymtable = func->localTable;
    }
    bool returnSt;
    Token* t;

    t = getNextToken(tl);
    returnSt = compareTerminalStrings(t, "return");

    Nonterminal* expTree;
    returnSt = returnSt && precParser(tl, &expTree);
    processPossibleVariableDefinition(expTree, localSymtable);
    generateExpressionCode(expTree, false, localSymtable, &symtable);

    t = getNextToken(tl);
    returnSt = returnSt && compareLexTypes(t, SEMICOLON);

    // if the return is in the main body of the program, just exit with code 0
    if(!func){
        printf("EXIT int@0\n");
    }
    else if(func->returnType == NULL_T){
        if(expTree->NTType != EMPTY){
            semanticError(6);
        }
    }
    else{
        if(expTree->NTType == EMPTY){
            semanticError(6);
        }
    }
    printf("RETURN\n");
    return returnSt;
}

bool varExpansion(tokList* tl, variable* var){
    Token* t = getNextToken(tl);
    //fprintf(stderr, "EXPANDING VARIABLE WITH NAME: %s\n", t->string);
    var->name = t->string;
    return compareLexTypes(t, VAR_ID);
}

bool paramsExpansion(tokList* tl, varList* args){
    bool params = false;
    Token* t;
    t = tokListGetValue(tl);
    // () blank function call
    if(compareLexTypes(t, PAR_R)) params = true;
    else{
        variable* var = calloc(sizeof(variable), 1);

        //var type
        //dataType variableType;
        params = typeExpansion(tl, &var->dType, &var->nullable, false);
        //other var info
        params = params && varExpansion(tl, var) && paramListExpansion(tl, args);
        //fprintf(stderr, "APPENDING VARIABLE TO ARGS WITH NAME: %s\n", var->name);
        varListAppend(args, *var);
    }
    return params;
}

bool paramListExpansion(tokList* tl, varList* args){
    bool paramList = false;
    Token* t;
    t = tokListGetValue(tl);
    if(compareLexTypes(t, PAR_R)) paramList = true;
    else{
        variable* var = calloc(sizeof(variable), 1);

        //dataType variableType;
        t = getNextToken(tl);
        paramList = compareLexTypes(t, COMMA) && typeExpansion(tl, &var->dType, &var->nullable, false) && varExpansion(tl, var) && paramListExpansion(tl, args);
        //fprintf(stderr, "APPENDING VARIABLE TO ARGS WITH NAME: %s\n", var->name);
        varListAppend(args, *var);
    }
    return paramList;
}

bool typeExpansion(tokList* tl, dataType* returnType, bool* nullable, bool isReturnType){
    bool type = false;
    bool questionMark = false;
    Token* t;

    t = tokListGetValue(tl);
    if(compareLexTypes(t, QUESTION_MARK)){
        questionMark = true;
        *nullable = true;
        getNextToken(tl);
    }
    type = typeNameExpansion(tl, questionMark, returnType, isReturnType);
    return type;
}


bool typeNameExpansion(tokList* tl, bool questionMark, dataType* returnType, bool isReturnType){
    bool typeName;
    Token* t;
    t = getNextToken(tl);
    typeName = compareLexTypes(t, FUN_ID);
    if(isReturnType) {
    typeName = typeName && (compareTerminalStrings(t, "int") || compareTerminalStrings(t, "float") || 
                            (compareTerminalStrings(t, "void") && !questionMark) || compareTerminalStrings(t, "string"));
    } else {
       typeName = typeName && (compareTerminalStrings(t, "int") || compareTerminalStrings(t, "float") || compareTerminalStrings(t, "string"));
    }
    //FIXME: returnType is not bool
    //TODO:
    terminalToDataType(t, returnType);
    return typeName;                  
}

void terminalToDataType(Token* t, dataType* type) {
  // fprintf(stderr, "SETTING DATATYPE OF TOKEN TO %s\n", t->string);
  if(compareTerminalStrings(t, "int"))    {*type = INT; return;}
  if(compareTerminalStrings(t, "float"))  {*type = FLOAT; return;}
  //FIXME:
  if(compareTerminalStrings(t, "void"))   {*type = NULL_T; return;}
  if(compareTerminalStrings(t, "string")) {*type = STRING; return;}
}

bool blockExpansion(tokList* tl, function* func){
    bool block = false;
    Token* t;
    t = getNextToken(tl);
    block = compareLexTypes(t, CBR_L) && blockSTListExpansion(tl, func);
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
                        fprintf(stderr, "%lld", root->term.integerLit);
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
            case AS:
                fprintf(stderr, "\t=\t");
                break;
            default: break;
        }
        debugPrintExprTree(root->expr.right);
    }
}


/* void compileTimeExpressionEval(Nonterminal* expTree){
    if(!expTree){
        return;
    }
    compileTimeExpressionEval(expTree->expr.left);
    compileTimeExpressionEval(expTree->expr.right);
    switch(expTree->NTType){
        case EXPR:
            if(expTree->expr.left->NTType == LITERAL_TERM && expTree->expr.right->NTType == LITERAL_TERM){
                switch(expTree->expr.op){
                    case PLUS: case MINUS: case MUL:
                }
            }
        default: return;
    }
} */
