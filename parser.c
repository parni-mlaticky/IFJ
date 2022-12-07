/**
 * @file parser.c
 * @author Vladimír Hucovič (xhucov00), Ondřej Zobal (xzobal01), Petr Kolouch (xkolou05), Marek Havel (xhavel46)
 * @brief implementation of syntax analyzer
 * 
 */
#include "parser.h"

static sym_table_t symtable;

void addFuncToSymtable(char* name, varList* argList, bool nullable, dataType returnType, sym_table_t* localTable){
    symtableElem* functionElem = malloc(sizeof(symtableElem));
    CHECK_ALLOCATION(functionElem)
    function* func = malloc(sizeof(function));
    CHECK_ALLOCATION(func)
    func->functionName = name;
    func->args = argList;
    func->localTable = localTable;
    func->nullable = nullable;
    func->returnType = returnType;
    functionElem->type = FUNCTION;
    functionElem->f = func;
    symtable_insert(&symtable, name, functionElem);
}

void addBuiltinFunctionsToSymtable(){
    // reads(), readi(), readf()
    varList* argList = malloc(sizeof(varList));
    CHECK_ALLOCATION(argList)
    varListInit(argList);
    addFuncToSymtable("reads", argList, true, STRING, NULL);
    addFuncToSymtable("readi", argList, true, INT, NULL);
    addFuncToSymtable("readf", argList, true, FLOAT, NULL);

    // write()
    addFuncToSymtable("write", NULL, true, NULL_T, NULL);

    // floatval(), intval(), strval()
    argList = malloc(sizeof(varList));
    CHECK_ALLOCATION(argList)
    varListInit(argList);
    variable term = {.dType = UNKNOWN, .name = "term", .nullable = true};
    varListAppend(argList, term);
    addFuncToSymtable("floatval", argList, false, FLOAT, NULL);
    addFuncToSymtable("intval", argList, false, INT, NULL);
    addFuncToSymtable("strval", argList, false, STRING, NULL);

    // strlen()
    argList = malloc(sizeof(varList));
    CHECK_ALLOCATION(argList)
    varListInit(argList);
    variable string = {.dType = STRING, .name = "s", .nullable = false};
    varListAppend(argList, string);
    addFuncToSymtable("strlen", argList, false, INT, NULL);

    // substring()
    argList = malloc(sizeof(varList));
    CHECK_ALLOCATION(argList)
    varListInit(argList);
    varListAppend(argList, string);
    variable i = {.dType = INT, .name = "i", .nullable = false};
    variable j = {.dType = INT, .name = "j", .nullable = false};
    varListAppend(argList, i);
    varListAppend(argList, j);
    addFuncToSymtable("substring", argList, true, STRING, NULL);

    // ord()
    argList = malloc(sizeof(varList));
    CHECK_ALLOCATION(argList)
    varListInit(argList);
    varListAppend(argList, string);
    addFuncToSymtable("ord", argList, false, INT, NULL);

    // chr()
    argList = malloc(sizeof(varList));
    CHECK_ALLOCATION(argList)
    varListInit(argList);
    varListAppend(argList, i);
    addFuncToSymtable("chr", argList, false, STRING, NULL);
}

// goes through the token list and finds function definitions
// adds the functions to the symtable and skips the rest of the tokens
bool firstPass(tokList* tl){
    char* funcName = NULL;
    bool fPass = true;
    dataType returnType;
    bool nullable = false;
    varList* args;
    sym_table_t* localTable;
    symtableElem* var;
    tokListFirst(tl);
    Token* t = tokListGetValue(tl);
    
    // goes through the token list until it finds the "function" keyword
    while(!compareLexTypes(t, END) && !compareLexTypes(t, SCRIPT_STOP)){
        while(!compareLexTypes(t, FUN_ID) || !compareTerminalStrings(t, "function")){
            if(compareLexTypes(t, END) || compareLexTypes(t, SCRIPT_STOP)) break;
            t = getNextToken(tl);
        }
        if(compareLexTypes(t, END) || compareLexTypes(t, SCRIPT_STOP)) break;

        // function found: expand the function definition and add its arguments to args
        args = malloc(sizeof(varList));
        CHECK_ALLOCATION(args)
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
        // if function already in symtable, exit
        if(symtable_get(&symtable, funcName)) semanticError(3);

        // create local symtable for the function
        localTable = calloc(sizeof(sym_table_t), 1);
        CHECK_ALLOCATION(localTable)
        varListFirst(args);
        variable varIter;

        // insert function arguments into symtable as local variables of the function
        while(varListIsActive(args)){
            varIter = varListGetValue(args);
            if(!varIter.name){
                syntaxError(NULL, "error in params in function declaration");
            }
            var = symtable_get(localTable, varIter.name);
            if(var) semanticError(8);
            var = malloc(sizeof(symtableElem));
            CHECK_ALLOCATION(var)
            var->type = VARIABLE;
            var->v = variable_clone(&varIter);
            symtable_insert(localTable, varIter.name, var);
            varListNext(args);
        }
            // finally, add function to symtable
            addFuncToSymtable(funcName, args, nullable, returnType, localTable);


            // skip the function body
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
        // empty stack case
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

        // operand
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
            || lexToTerm == DOLLAR || lexToTerm == RPAR){
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

// checks if elem is operator
// Operators are enums that are >= 4
bool isOperator(stackElement* elem){
    if(elem->data.terminal->tType >= 4){
        return true;
    }
    return false;
}

// checks if the stack elements that are being reduced in precedence parsing can be reduced
// according to the rules of the grammar
expressionRule precParseCheckRule(stackElement* elem1, stackElement* elem2, stackElement* elem3){
    // only elem1 is set
    if(elem1 && !elem2 && !elem3){
        // Rule E => i
        if(elem1->data.etype == TERMINAL && elem1->data.terminal->tType == OP){
            return EXP_TERM;
        }
        else syntaxError(NULL, "precParseCheckRule error");
    }
    // all elements are set
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
    // anything else is wrong
    return false;
}

// checks if elem is a value that can be assigned to
// (aka it is a variable ID)
bool checkIfLValue(stackElement* elem){
    if(elem->data.etype == NONTERMINAL){
        if(elem->data.nonterminal->NTType == VAR_ID_TERM){
            return true;
        }
    }
    return false;
}



// finds handle and reduces the stack content above the handle according to the
// expression grammar rules, or throw syntax error
void precParseReduction(stack* s, bool* relOpInExp){
    stackElement* handle = findHandle(s);
    if(!handle) syntaxError(NULL, "precParseReduction handle missing error");
    stackElement* iter = handle;
    int count = -1;
    // stard at handle, iterate through the stack towards the top
    while(iter){
        iter = iter->prev;
        count++;
    }
    // if only 1 element above handle, expect rule E => i
    if(count == 1){
        if(precParseCheckRule(handle->prev, NULL, NULL) == EXP_TERM){
            stackElement* tmp = malloc(sizeof(stackElement));
            CHECK_ALLOCATION(tmp)
            memcpy(tmp, handle->prev, sizeof(stackElement));
            stackMultiPop(s, count+1);

            // chedck the type of the terminal and create a corresponding nonterminal that gets pushed after reduction
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
   
   // 3 items above handle, expect rule E => <E> <OP> <E> or E => (E)
    else if(count == 3){
        // rule E => <E> <OP> <E>
        if(precParseCheckRule(handle->prev, handle->prev->prev, handle->prev->prev->prev) == EXP_OP_EXP){
            // since there can only be 1 relation operator in expression, set this to true
            // later another rel operator is detected, throw syntax error during grammar check
            if(isRelOperator(handle->prev->prev->data.terminal->tType)) *relOpInExp = true;
            
            // check if this is an attempt to assign to something that cant be assigned to
            if(handle->prev->prev->data.terminal->tType == AS){
                if(!checkIfLValue(handle->prev)){
                    syntaxError(NULL, "precParseReduction checkIfLValue error");
                }
            }

            // copy the stack nonterminals since they get popped before the new nonterminal gets pushed
            Nonterminal* operand1 = malloc(sizeof(Nonterminal));
            CHECK_ALLOCATION(operand1)
            Nonterminal* operand2 = malloc(sizeof(Nonterminal));
            CHECK_ALLOCATION(operand2)
            memcpy(operand1, handle->prev->data.nonterminal, sizeof(Nonterminal));
            memcpy(operand2, handle->prev->prev->prev->data.nonterminal, sizeof(Nonterminal));

            stackElement* op = malloc(sizeof(stackElement));
            CHECK_ALLOCATION(op)
            memcpy(op, handle->prev->prev, sizeof(stackElement));

            // create the new nonterminal, pop stack content above handle and push new nonterminal
            Nonterminal* nonterm = createExprNonterminal(operand1, operand2, op->data.terminal->tType);
            stackMultiPop(s, count+1);
            stackPushNonterminal(s, nonterm);
            free(op);
        }
        // Rule E => (E)
        else if(precParseCheckRule(handle->prev, handle->prev->prev, handle->prev->prev->prev) == EXP_PAR){
            Nonterminal* exp = malloc(sizeof(Nonterminal));
            CHECK_ALLOCATION(exp)
            memcpy(exp, handle->prev->prev->data.nonterminal, sizeof(Nonterminal));
            stackMultiPop(s, count+1);
            stackPushNonterminal(s, exp);
        }
    }   
    else syntaxError(NULL, "precParseReduction count error");      
}


Nonterminal* createIntLiteralNonterminal(int value){
    Nonterminal* intLit = malloc(sizeof(Nonterminal));
    CHECK_ALLOCATION(intLit)
    intLit->NTType = LITERAL_TERM;
    intLit->term.integerLit = value;
    intLit->dType = INT;


    intLit->expr.left = NULL;
    intLit->expr.right = NULL;

    return intLit;
}

Nonterminal* createStringLiteralNonterminal(char* string){
    Nonterminal* stringLit = malloc(sizeof(Nonterminal));
    CHECK_ALLOCATION(stringLit)
    stringLit->NTType = LITERAL_TERM;
    stringLit->term.stringLit = string;
    stringLit->dType = STRING;

    stringLit->expr.left = NULL;
    stringLit->expr.right = NULL;

    return stringLit;
}

Nonterminal* createFloatLiteralNonterminal(double value){
    Nonterminal* floatLit = malloc(sizeof(Nonterminal));
    CHECK_ALLOCATION(floatLit)
    floatLit->NTType = LITERAL_TERM;
    floatLit->term.floatLit = value;
    floatLit->dType = FLOAT;

    floatLit->expr.left = NULL;
    floatLit->expr.right = NULL;

    return floatLit;
}

Nonterminal* createFuncallNonterminal(char* funId, nontermList* args){
    Nonterminal* funcNonterm = malloc(sizeof(Nonterminal));
    CHECK_ALLOCATION(funcNonterm)
    funcall* func = malloc(sizeof(funcall));
    CHECK_ALLOCATION(func)
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
    CHECK_ALLOCATION(nullNonterm)

    nullNonterm->NTType = LITERAL_TERM;
    nullNonterm->dType = NULL_T;
    nullNonterm->expr.left = NULL;
    nullNonterm->expr.right = NULL;
    nullNonterm->expr.op = DOLLAR;
    
    return nullNonterm;
}

Nonterminal* createVariableNonterminal(char* varId, dataType dType){
    Nonterminal* varNonterm = malloc(sizeof(Nonterminal));
    CHECK_ALLOCATION(varNonterm)
    variable* var = malloc(sizeof(variable));
    CHECK_ALLOCATION(var)
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
    CHECK_ALLOCATION(newNonterminal)

    newNonterminal->expr.left = left;
    newNonterminal->expr.right = right;
    newNonterminal->expr.op = operator;

    newNonterminal->NTType = EXPR;
    return newNonterminal;
}

// parses expressions bottom-up using the precedence parsing method
bool precParser(tokList* tl, Nonterminal** finalNonterm, bool isFuncArg){
    bool result = false;
    stack s;
    stackInit(&s);

    // because expression like $a === $b === $c is invalid
    bool relOpInExp = false;
    // counts parentheses in the expression to differentiate between the end parentheses
    // that belong to the expression and parentheses that do not
    int parCount = 0;

    // push starting terminal (bottom of stack)
    stackPushTerminal(&s, DOLLAR, NULL);
    bool emptyExpr = true;
    Token* token;
    stackElement* top;

    // this loop reads tokens in the expression and calls other precedence parsing functions
    while(1){
        token = tokListGetValue(tl);
        if(!token) syntaxError(token, "precParser missing Token error");
        // check if a symbol that ends an expression is encountered
        if(((compareLexTypes(token, PAR_R) || compareLexTypes(token, SEMICOLON) || compareLexTypes(token, COMMA)) && parCount == 0)){
            // check if everything on the stack has been reduced, then break
            if(containsOnlyTopNonterm(&s) || stackIsEmpty(&s)){
                break;
            }
            // else continue reducing whatever is on the stack until only the root of the expression remains
            else{
                while(!containsOnlyTopNonterm(&s)){
                    precParseReduction(&s, &relOpInExp);
                    emptyExpr = false;
                }
                break;
            }
        }

        top = getTopTerminal(&s);
        // if rel operator is already in expression and this is another rel operator, semantic error
        if(isRelOperator(lexEnumToTerminalEnum(token->lex)) && relOpInExp) semanticError(7);
        opPrecedence prec = getPrecedence(top->data.terminal->tType, token->lex);

        // lower precedence (< in prec. table)
        if(prec == P_LESS){
            stackInsertHandle(&s);
            // operand is a function call - recursively call precParser on the function arguments
            // and add the resulting expressions to args list, then push funcall to stack
            if(compareLexTypes(token, FUN_ID)){
                if(compareTerminalStrings(token, "null")){
                    stackPushTerminal(&s, OP, token);
                }
                else if(isKeyword(token)){
                    syntaxError(token, "unexpected keyword in expression");
                }
                else{
                    Token *nextToken = getNextToken(tl); // name of the function
                    nontermList* args = malloc(sizeof(nontermList));
                    CHECK_ALLOCATION(args)
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

                    if(precParser(tl, &nonTerm, true)) {
                        nontermListAppend(args, nonTerm);
                        nextToken = tokListGetValue(tl);

                        if(nextToken->lex == PAR_R) {
                            break;
                        }
        
                        nextToken = getNextToken(tl);
                    } else {
                        syntaxError(nextToken, "precParser precParser returned false error");
                    }
                    }
                    stackPushFuncallTerminal(&s, token, args);
                }
            } // if token is a function
            

            // if token is not a function call, just push it to the stack
            else{
                stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            }
            if(compareLexTypes(token, PAR_L)){
                parCount++;
            }  
            getNextToken(tl);
        }
        // greater precedence (> in prec. table)
        else if(prec == P_GREATER){
            precParseReduction(&s, &relOpInExp);
            emptyExpr = false;
        }
        // equal precedence (= in prec. table)
        else if(prec == P_EQ){
            stackPushTerminal(&s, lexEnumToTerminalEnum(token->lex), token);
            parCount--;
            getNextToken(tl);
        }
    } // end of loop that parses the expression


    if(containsOnlyTopNonterm(&s) && (token->lex == PAR_R || token->lex == COMMA)){
        if(!isFuncArg && token->lex == COMMA){
            syntaxError(token, "Unexpected ,");
        }
        result = true;
    }
    if((containsOnlyTopNonterm(&s) || stackIsEmpty(&s)) && token->lex == SEMICOLON){
        result = true;
    }
    // return the final nonterminal through the pointer argument
    if(result && !emptyExpr){
        *finalNonterm = s.top->data.nonterminal;
    }
    // if expr is empty, create an empty nonterminal
    else{
        Nonterminal* empty = malloc(sizeof(Nonterminal));
        CHECK_ALLOCATION(empty)
        empty->NTType = EMPTY;
        *finalNonterm = empty;
    }
    return result;
}

// collects all tokens in the file into a list and calls first pass and recursive descent on it
// also calls some functions which generate IFJcode22 code for internal helper functions and builtin functions
// and initializes the symtable
bool parse_file(FILE* file) {
    bool result = false;
    tokList* list = malloc(sizeof(tokList));
    CHECK_ALLOCATION(list)
    tokListInit(list);
    Token token;
    bool expect_prolog = true;
    do {
        Token* new = malloc(sizeof(Token));
        CHECK_ALLOCATION(new)
        *new = scan_next_token(file, expect_prolog);
        token = *new;
        tokListAppend(list, new);
        expect_prolog = false;
    } while (token.lex != END);
    debug_print_tokens(list);
    symtable_init(&symtable);
    generateStarterAsm();
    addBuiltinFunctionsToSymtable();
    result = firstPass(list);
    tokListFirst(list);

    // "Main" function (main body of the IFJ22 program)
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
    const char* keywords[] = { "if", "else", "declare", "function",
                           "while", "int", "float", "void",
                           "string", "null", "return"};
    int keywordCount = sizeof(keywords) / sizeof(char*);
    if(!compareLexTypes(t, FUN_ID)) return false;
    for(int i = 0; i < keywordCount; i++){
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
    Token* t = tokListIsActive(tl) ? tokListGetValue(tl) : NULL;
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

// <PROG> => "<?php" <DECLARE_ST> <ST_LIST> <END_TOKEN>
bool progExpansion(tokList* tl){
    bool prog = false;
    Token* t = getNextToken(tl);
    prog = compareLexTypes(t, SCRIPT_START) && declareStExpansion(tl)
    && STListExpansion(tl, NULL, false) && endTokenExpansion(tl);
    return prog;
}

// <DECLARE_ST> => "declare(strict_types=1);" 
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

// <ST_LIST> => <ST> <ST_LIST>
// <BLOCK_ST_LIST> => <BLOCK_ST> <BLOCK_ST_LIST>
bool STListExpansion(tokList* tl, function* func, bool isInBlock){
    bool STList = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, END)) STList = true;
    else if(compareLexTypes(t, SCRIPT_STOP)) STList = true;
    else if(compareLexTypes(t, CBR_R)) STList = true;
    else STList = STExpansion(tl, func, isInBlock) && STListExpansion(tl, func, isInBlock);
    return STList;
}

// <ST> => <FUNCTION_DEF>, <IF_ST>, ...
// <BLOCK_ST> => <IF_ST>, <WHILE_ST>, etc.. but not <FUNCTION_DEF>
bool STExpansion(tokList* tl, function* func, bool isInBlock){
    bool St = false;
    sym_table_t* localSymtable = &symtable;
    if(func){
        localSymtable = func->localTable;
    }

    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, VAR_ID)){
        Nonterminal* expTree;
        St = precParser(tl, &expTree, false);

        processPossibleVariableDefinition(expTree, localSymtable);

        t = getNextToken(tl);
        generateExpressionCode(expTree, false, localSymtable, &symtable);
        St = St && compareLexTypes(t, SEMICOLON);
        printf("POPS GF@%%RAX\n");
    }
    else if(compareLexTypes(t, FUN_ID)){
        if(compareTerminalStrings(t, "if")) St = ifStExpansion(tl, func);
        else if(compareTerminalStrings(t, "while")) St = whileStExpansion(tl, func);
        else if(compareTerminalStrings(t, "return")) St = returnStExpansion(tl, func);
        else if(compareTerminalStrings(t, "function")){
            if(isInBlock){
                syntaxError(t, "nested function definition is not allowed");
            }
            else St = functionDefStExpansion(tl);
        }
        else{ 
            Nonterminal* expTree;
            St = precParser(tl, &expTree, false);
            processPossibleVariableDefinition(expTree, localSymtable);
            t = getNextToken(tl);
            generateExpressionCode(expTree, false, localSymtable, &symtable);
            St = St && compareLexTypes(t, SEMICOLON);
            printf("POPS GF@%%RAX\n");
        }    
    }
    else {
        Nonterminal* expTree;
        St = precParser(tl, &expTree, false);
        processPossibleVariableDefinition(expTree, localSymtable);
        t = getNextToken(tl);
        generateExpressionCode(expTree, false, localSymtable, &symtable);
        St = St && compareLexTypes(t, SEMICOLON);
        printf("POPS GF@%%RAX\n");
    }
    return St;
}

void processPossibleVariableDefinition(Nonterminal* expTree, sym_table_t* symtable) {
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
    symtableElem* elem = symtable_get(symtable, var->term.var->name);
    
    if (elem == NULL) {
        // CREATING VAR LIST FOR DEFINING AT THE END OF THE FUNCTION
        elem = malloc(sizeof(symtableElem));
        CHECK_ALLOCATION(elem)
        elem->type = VARIABLE;
        elem->v    = variable_clone(var->term.var);
        symtable_insert(symtable, var->term.var->name, elem);
    }
    else {
        elem->v->dType = var->dType;
    }
    //vars = definedVars;
}

// <END> => "?>" or nothing (epsilon)
bool endTokenExpansion(tokList* tl){
    bool end = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, SCRIPT_STOP) || compareLexTypes(t, END)) end = true;
    return end;
}

// <FUNCTION_DEF> => "function <ID> (<PARAMS>) : <?><TYPE> {<BLOCK>}"
bool functionDefStExpansion(tokList* tl){
    bool fDefSt = false;
    char* funcName = NULL;
    getNextToken(tl);
    Token* t = getNextToken(tl);
    funcName = t->string;
    // since function code is generated inbetween main body statements, 
    // this jump skips the function
    printf("JUMP _%s_FUNC_END\n", funcName);
    printf("LABEL _%s\n", funcName);
    // call a function that defines all local variables in the local frame
    printf("CALL _%s_VAR_DEFS\n", funcName);
    // get the function from symtable (added there in first pass)
    function* func = symtable_get(&symtable, funcName)->f;
    varListFirst(func->args);
    variable argsIter;
    // iterate through the args list and generate code for runtime checking of passed types
    // exit with code 4 if types dont match the definition
    for(int i = 0; i < func->args->len; i++){
        argsIter = varListGetValue(func->args);
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
        varListNext(func->args);
    }

    // skip the rest of the function head (any syntax errors would have been caught in first pass)
    while(tl->active->data->lex != CBR_L){
        getNextToken(tl);
    }

    // expand the body of the function
    fDefSt = blockExpansion(tl, func);

    // if the return type isnt void and there is no return statement in the function,
    // this jump is executed and the program exits with code 4
    if(func->returnType != NULL_T){
        printf("JUMP %%ERROR_4\n"); //
    }
    // if the return type is void, it actually just returns null on function body end
    else{
        printf("PUSHS nil@nil\n");
        printf("RETURN\n");
    }

    // label that the function jumps to at the start to define local variables, then jumps back
    printf("LABEL _%s_VAR_DEFS\n", func->functionName);
    defineFunctionVars(*func->localTable);
    printf("RETURN\n");
    printf("LABEL _%s_FUNC_END\n", func->functionName);
    return fDefSt;
}

// <IF_ST> => if(<EXPR>) {<BLOCK>} else {<BLOCK>}
bool ifStExpansion(tokList* tl, function* func){
    sym_table_t* localSymtab = &symtable;
    if(func){
        localSymtab = func->localTable;
    }

    // static variable used for unique label generation for different if statements
    static int ifCount = 0;
    int currentIfId = ifCount;
    ifCount++; 
    bool ifSt;
    Token* t = getNextToken(tl);
    ifSt = compareTerminalStrings(t, "if");
    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, PAR_L);

    Nonterminal* expTree;
    ifSt = ifSt && precParser(tl, &expTree, false);
    processPossibleVariableDefinition(expTree, localSymtab);
    generateExpressionCode(expTree, false, localSymtab, &symtable);

    // convert the expression to bool in runtime
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

// <WHILE_ST> => while (<EXPR>) {<BLOCK>}
bool whileStExpansion(tokList* tl, function* func){
    sym_table_t* localSymtable = &symtable;
    if(func){
        localSymtable = func->localTable;
    }
    // static variable used for unique label generation for different while statements
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
    whileSt = whileSt && precParser(tl, &expTree, false);
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

// <RETURN_ST> => "RETURN" <EXPR>
bool returnStExpansion(tokList* tl, function* func){
    sym_table_t* localSymtable = &symtable;
    if(func){
        localSymtable = func->localTable;
    }
    bool returnSt;
    Token* t;

    t = getNextToken(tl);
    returnSt = compareTerminalStrings(t, "return");

    Nonterminal* expTree;
    returnSt = returnSt && precParser(tl, &expTree, false);
    processPossibleVariableDefinition(expTree, localSymtable);
    generateExpressionCode(expTree, false, localSymtable, &symtable);

    t = getNextToken(tl);
    returnSt = returnSt && compareLexTypes(t, SEMICOLON);

    // if the return is in the main body of the program, just exit with code 0
    if(!func){
        printf("EXIT int@0\n");
    }
    // func type void but expression not empty => err 6
    else if(func->returnType == NULL_T){
        if(expTree->NTType != EMPTY){
            semanticError(6);
        }
    }
    // func type not void but expression empty => err 6
    else{
        if(expTree->NTType == EMPTY){
            semanticError(6);
        }
    }
    printf("RETURN\n");
    return returnSt;
}

// <VAR> => VAR_ID
bool varExpansion(tokList* tl, variable* var){
    Token* t = getNextToken(tl);
    var->name = t->string;
    return compareLexTypes(t, VAR_ID);
}

// <PARAMS> => <PARAM> <PARAM_LIST>
// <PARAMS> => epsilon
bool paramsExpansion(tokList* tl, varList* args){
    bool params = false;
    Token* t;
    t = tokListGetValue(tl);
    if(compareLexTypes(t, PAR_R)) params = true;
    else{
        variable* var = calloc(sizeof(variable), 1);
        CHECK_ALLOCATION(var)
        //var type
        params = typeExpansion(tl, &var->dType, &var->nullable, false);
        //other var info
        params = params && varExpansion(tl, var) && paramListExpansion(tl, args);
        varListAppend(args, *var);
    }
    return params;
}

// <PARAM_LIST> => "," <PARAM>
// <PARAM_LIST> => epsilon
bool paramListExpansion(tokList* tl, varList* args){
    bool paramList = false;
    Token* t;
    t = tokListGetValue(tl);
    // epsilon rule
    if(compareLexTypes(t, PAR_R)) paramList = true;
    // the other rule
    else{
        variable* var = calloc(sizeof(variable), 1);
        CHECK_ALLOCATION(var)
        t = getNextToken(tl);
        paramList = compareLexTypes(t, COMMA) && typeExpansion(tl, &var->dType, &var->nullable, false) && varExpansion(tl, var) && paramListExpansion(tl, args);
        varListAppend(args, *var);
    }
    return paramList;
}

// <TYPE> => <?> <TYPE_NAME>
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

// <TYPE_NAME> => "int", "float", "string", "void"
bool typeNameExpansion(tokList* tl, bool questionMark, dataType* returnType, bool isReturnType){
    bool typeName;
    Token* t;
    t = getNextToken(tl);
    typeName = compareLexTypes(t, FUN_ID);

    // if this is a return type expansion, void is allowed
    if(isReturnType) {
    typeName = typeName && (compareTerminalStrings(t, "int") || compareTerminalStrings(t, "float") || 
                            (compareTerminalStrings(t, "void") && !questionMark) || compareTerminalStrings(t, "string"));
    } 
    // otherwise void is not allowed
    else {
       typeName = typeName && (compareTerminalStrings(t, "int") || compareTerminalStrings(t, "float") || compareTerminalStrings(t, "string"));
    }
    *returnType = terminalToDataType(t);
    return typeName;                  
}

// converts string terminal to dtype enum
dataType terminalToDataType(Token* t) {
  if(compareTerminalStrings(t, "int"))    return INT;
  if(compareTerminalStrings(t, "float"))  return FLOAT;
  if(compareTerminalStrings(t, "void"))   return NULL_T;
  if(compareTerminalStrings(t, "string")) return STRING;
  return 0;
}

// <BLOCK> => <BLOCK_ST_LIST>
bool blockExpansion(tokList* tl, function* func){
    bool block = false;
    Token* t;
    t = getNextToken(tl);
    block = compareLexTypes(t, CBR_L) && STListExpansion(tl, func, true);
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
