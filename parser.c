#include <stdlib.h>
#include "parser.h"
#include "stack.h"
#include "list.h"
#include "keywords.h"




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
        // this will never happen    
        default:
            exit(1);                                
    }
}


// Operators are >= 4 in the enum
bool isOperator(stackElement* elem){
    if(elem->data.tType >= 4){
        return true;
    }
    return false;
}

bool precParseCheckRule(stackElement* elem1, stackElement* elem2, stackElement* elem3){
    if(elem1 && !elem2 && !elem3){
        // Rule E => i
        if(elem1->data.etype == TERMINAL && elem1->data.tType == OP){
            return true;
        }
        else syntaxError();
    }
    if(elem1 && elem2 && elem3){
        // rule E => (E)
        if(elem1->data.tType == LPAR && elem2->data.etype == NONTERMINAL && elem3->data.tType == RPAR){
            return true;
        }
        // rule E => E <OP> E
        if(elem1->data.etype == NONTERMINAL && isOperator(elem2) && elem3->data.etype == NONTERMINAL){
            return true;
        }
    }
    return false;
}

void precParseReduction(stack* s){
    stackElement* handle = findHandle(s);
    if(!handle) syntaxError();
    stackElement* iter = handle;
    int count = -1;
    while(iter){
        iter = iter->prev;
        count++;
    }
    switch (count) {
        case 0:
            syntaxError();
            break;
        case 1:
            if(precParseCheckRule(handle->prev, NULL, NULL)){
                stackMultiPop(s, count+1);
                stackPushNonterminal(s);
            }
            else syntaxError();
            break;


        case 2:
            syntaxError();
            break;    
        case 3:
            if(precParseCheckRule(handle->prev, handle->prev->prev, handle->prev->prev->prev)){
                stackMultiPop(s, count+1);
                stackPushNonterminal(s);
            }
            else syntaxError();
            break;

        default:
            // Syntax error, OR its a bunch of assignments chained together like $a = $b = $c = $d
            // I have no idea how are we gonna do this
            syntaxError();       
    }

    
}

bool precParser(tokList* tl){
    bool result = false;
    stack s;
    stackInit(&s);

    stackPushTerminal(&s, DOLLAR);
    Token* t = getNextToken(tl);
    if(!t) syntaxError();
    stackElement* top;
    while(1){
        if(compareLexTypes(t, PAR_R) || compareLexTypes(t, SEMICOLON)){
            if(containsOnlyTopNonterm(&s)){
                break;
            }
            else{
                while(!containsOnlyTopNonterm(&s)){
                    precParseReduction(&s);
                }
                break;
            }
        }
        top = getTopTerminal(&s);
        switch(top->data.tType){
            case DOLLAR:
                switch(t->lex){
                    case ADD: case SUBTRACT: case DIVIDE: case MULTIPLY:
                    case ASSIGN: case EQUAL: case NOT_EQUAL: case GREATER_EQUAL:
                    case LESS_EQUAL: case GREATER: case LESS: case DOLLAR: case PAR_R:
                    case VAR_ID: case FUN_ID: case INT_LIT: case FLOAT_LIT: case STRING_LIT: 
                        stackInsertHandle(&s);
                        stackPushTerminal(&s, lexEnumToTerminalEnum(t->lex));
                        t = getNextToken(tl);
                        break;
                    default:
                        syntaxError(); // FIXME   
                }
                break;
            case OP:
                switch(t->lex){
                    case ADD: case SUBTRACT: case DIVIDE: case MULTIPLY:
                    case ASSIGN: case EQUAL: case NOT_EQUAL: case GREATER_EQUAL:
                    case LESS_EQUAL: case GREATER: case LESS: case PAR_R: case SEMICOLON:
                        precParseReduction(&s);
                        break;
                    default:
                        syntaxError();    
                }
                break;    
            case PLUS: case MINUS:
                switch(t->lex){
                    // "<" rules    
                    case DIVIDE: case MULTIPLY: case PAR_L: case VAR_ID:
                    case FUN_ID: case INT_LIT: case STRING_LIT: case FLOAT_LIT:
                        stackInsertHandle(&s);
                        stackPushTerminal(&s, lexEnumToTerminalEnum(t->lex));
                        t = getNextToken(tl);
                        break;


                    // ">" rules
                    case ADD: case SUBTRACT: case ASSIGN: case EQUAL: case NOT_EQUAL: case GREATER_EQUAL: case LESS_EQUAL: case GREATER: case LESS: case PAR_R:
                        precParseReduction(&s);
                        break;
                }
        }
    }
    if(containsOnlyTopNonterm(&s) && (t->lex == SEMICOLON || t->lex == CBR_R)){
        result = true;
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

bool STExpansion(tokList* tl){
    bool St = false;
    Token* t = tokListGetValue(tl);
    if(compareLexTypes(t, VAR_ID)){
        St = precParser(tl);
    }
    else if(compareLexTypes(t, FUN_ID)){
        if(compareTerminalStrings(t, "function")) St = functionDefStExpansion(tl);
        else if(compareTerminalStrings(t, "if")) St = ifStExpansion(tl);
        else if(compareTerminalStrings(t, "while")) St = whileStExpansion(tl);
        else if(compareTerminalStrings(t, "return")) St = returnStExpansion(tl);
        else St = precParser(tl);
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
    fDefSt = fDefSt; // && compareLexTypes(t, COLON) && typeExpansion(tl) && blockExpansion(tl) FIXME
    return fDefSt;
}

bool ifStExpansion(tokList* tl){
    bool ifSt;
    Token* t = getNextToken(tl);
    ifSt = compareTerminalStrings(t, "if");
    t = getNextToken(tl);
    ifSt = ifSt && compareLexTypes(t, PAR_L);

    ifSt = ifSt && precParser(tl);

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

    whileSt = whileSt && precParser(tl);

    t = getNextToken(tl);
    whileSt = whileSt && compareLexTypes(t, PAR_R) && blockExpansion(tl);
    return whileSt;
}

bool returnStExpansion(tokList* tl){
    bool returnSt;
    Token* t;

    t = getNextToken(tl);
    returnSt = compareTerminalStrings(t, "return");

    returnSt = returnSt && precParser(tl);

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
    Token* t;

    t = tokListGetValue(tl);
    if(compareLexTypes(t, QUESTION_MARK)){
        getNextToken(tl);
    }
    type = typeNameExpansion(tl);
    return type;
}


bool typeNameExpansion(tokList* tl){
    bool typeName;
    Token* t;
    t = getNextToken(tl);
    typeName = compareLexTypes(t, FUN_ID); // FIXME
    typeName = typeName && (compareTerminalStrings(t, "int") || compareTerminalStrings(t, "float") || 
                            compareTerminalStrings(t, "void") || compareTerminalStrings(t, "string"));
    return typeName;                        
}

bool blockExpansion(tokList* tl){
    bool block = false;
    Token* t;
    t = getNextToken(tl);
    block = compareLexTypes(t, CBR_L) && STListExpansion(tl);
    t = getNextToken(tl);
    block = block && compareLexTypes(t, CBR_R);
    return block;
}


