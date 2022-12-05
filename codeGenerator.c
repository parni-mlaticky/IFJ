#include "codeGenerator.h"



void generateStarterAsm() {
    printf(".IFJcode22\n");
    printf("DEFVAR GF@%%RAX\n");
    printf("DEFVAR GF@%%RBX\n");
    printf("JUMP %%PROG_START\n");
    printf("LABEL %%ERROR_4\n");
    printf("EXIT int@4\n");
    printf("LABEL %%ERROR_6\n");
    printf("EXIT int@6\n");
    printf("LABEL %%ERROR_5\n");
    printf("EXIT int@5\n");
    generateToBoolFunction();
    // generateToFloatFunction();
    generateEnforceTypesFunction();
    generateStackSwapFunction();
    generateNormalizeNumericTypesFunction();
    generateNullToIntFunction();
    generateBuiltInFunctions();
    generateEmptyStringToInt();
    generateCompareDtypes();
    generateCheckIfIsType();
    generateCheckIfIsTypeOrNull();
    generateNonEquality();
    generateGreatEqual();
    generateLessEqual();
    generateEquality();
    generateGreat();
    generateLess();
}

void defineFunctionVars(ht_table_t symtable) {
    ht_item_t* iter;
    for (int i = 0; i < MAX_HT_SIZE; i++) {
        iter = symtable[i];
        if (iter) {
            while(iter){
                if (symtable[i]->value->type == VARIABLE) {
                    printf("DEFVAR LF@%s\n", iter->value->v->name);
                }
                iter = iter->next;
            }
        }
    }
}
/**
 * Expects (top to bottom) var of unknown type and string containing type name.
 * Pops only type string.
 * Exits if the variable doesn't have the given type.
 */
void generateEnforceTypesFunction() {
    printf(
        "LABEL %%ENFORCE_TYPES\n"
        // Popping type string
        "POPS GF@%%RAX\n"
        // Popping value
        "POPS GF@%%RBX\n"
        "PUSHS GF@%%RBX\n"

        "TYPE GF@%%RBX GF@%%RBX\n"
        "JUMPIFEQ %%GEN_ENF_TYP-END GF@%%RAX GF@%%RBX\n"
        "EXIT int@7\n"
        "LABEL %%GEN_ENF_TYP-END\n"
        "RETURN\n");
}

void generateNullToIntFunction() {
    printf(
        "LABEL %%NULL_TO_INT\n"
        // First operand
        "POPS GF@%%RAX\n"
        "PUSHS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"
        "JUMPIFNEQ %%NULL_TO_INT_BNN GF@%%RBX string@nil\n"
        "PUSHS int@0\n"
        "JUMP %%NULL_TO_INT_T\n"

        "LABEL %%NULL_TO_INT_BNN\n"
        "PUSHS GF@%%RAX\n"

        // Second operand
        "LABEL %%NULL_TO_INT_T\n"
        "POPS GF@%%RBX\n"
        "POPS GF@%%RBX\n"
        "TYPE GF@%%RAX GF@%%RBX\n"
        "JUMPIFNEQ %%NULL_TO_INT_TNN GF@%%RAX string@nil\n"
        "PUSHS int@0\n"
        "JUMP %%NULL_TO_INT_END\n"

        "LABEL %%NULL_TO_INT_TNN\n"
        "PUSHS GF@%%RBX\n"

        "LABEL %%NULL_TO_INT_END\n"
        "RETURN\n");
}

void generateNormalizeNumericTypesFunction() {
    printf(
        "LABEL %%NORMALIZE_NUMERIC_TYPES\n"
        "CALL %%NULL_TO_INT\n"
        "POPS GF@%%RAX\n"
        "POPS GF@%%RBX\n"
        "PUSHS GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "TYPE GF@%%RAX GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RBX\n"
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-MATCH GF@%%RAX GF@%%RBX\n"
        // Types aren't mathing
        // Verify that types are numeric for RAX
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-TEST-RBX GF@%%RAX string@int\n"
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-TEST-RBX GF@%%RAX string@float\n"
        "EXIT int@7\n"

        // Verify that types are numeric for RBX
        "LABEL %%NORMALIZE_NUMERIC_TYPES-TEST-RBX\n"
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-MISSMATCH GF@%%RBX string@int\n"
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-MISSMATCH GF@%%RBX string@float\n"
        "EXIT int@7\n"

        // If types are numeric and not matching, it means that on is int and the other
        // float. In that case we want to convert to floats.
        "LABEL %%NORMALIZE_NUMERIC_TYPES-MISSMATCH\n"
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-CONV-EBX GF@%%RAX string@float\n"
        "POPS GF@%%RAX\n"
        "INT2FLOAT GF@%%RAX GF@%%RAX\n"
        "PUSHS GF@%%RAX\n"
        "JUMP %%NORMALIZE_NUMERIC_TYPES-END\n"

        "LABEL %%NORMALIZE_NUMERIC_TYPES-CONV-EBX\n"
        "POPS GF@%%RAX\n"
        "POPS GF@%%RBX\n"
        "INT2FLOAT GF@%%RBX GF@%%RBX\n"
        "PUSHS GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "JUMP %%NORMALIZE_NUMERIC_TYPES-END\n"

        // Types are already matching!
        "LABEL %%NORMALIZE_NUMERIC_TYPES-MATCH\n"
        // Check if they matching their matching type is even numeric
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-END GF@%%RAX string@int\n"
        "JUMPIFEQ %%NORMALIZE_NUMERIC_TYPES-END GF@%%RAX string@float\n"
        "EXIT int@7\n"

        "LABEL %%NORMALIZE_NUMERIC_TYPES-END\n"
        "RETURN\n");
}

/**
 * Swaps two topmost values on the stack
 */
void generateStackSwapFunction() {
    printf(
        "LABEL %%STACK_SWAP\n"
        "POPS GF@%%RAX\n"
        "POPS GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "PUSHS GF@%%RBX\n"
        "RETURN\n");
}

void generateCompareDtypes() {
    printf(
        "LABEL %%COMPARE_DTYPES\n"
        "POPS GF@%%RAX\n"
        "POPS GF@%%RBX\n"
        "PUSHS GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"

        "TYPE GF@%%RAX GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RBX\n"

        "EQ GF@%%RAX GF@%%RAX GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");
}

void generateCheckIfIsType(){
    printf(
        "LABEL %%CHECK_IF_IS_TYPE\n"
        "POPS GF@%%RBX\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RAX GF@%%RAX\n"
        "PUSHS GF@%%RAX\n"
        "PUSHS GF@%%RBX\n"
        "EQS\n"
        "RETURN\n"
    );
}

void generateCheckIfIsTypeOrNull(){
    printf(
        "LABEL %%CHECK_IF_IS_TYPE_OR_NULL\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "DEFVAR LF@TMP\n"

        "POPS GF@%%RBX\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RAX GF@%%RAX\n"
        "MOVE LF@TMP GF@%%RAX\n" 

        "PUSHS GF@%%RAX\n"
        "PUSHS GF@%%RBX\n"
        "EQS\n"

        "PUSHS string@nil\n"
        "PUSHS LF@TMP\n"
        "EQS\n"
        "ORS\n"

        "POPFRAME\n"
        "RETURN\n"
    );
}

void generateEquality() {
    printf(
        "LABEL %%EQUALITY\n"
        "CALL %%COMPARE_DTYPES\n"
        "POPS GF@%%RAX\n"
        "JUMPIFEQ %%EQUALITY-EXIT GF@%%RAX bool@false\n"

        "EQS\n"
        "RETURN\n"

        "LABEL %%EQUALITY-EXIT\n"
        "PUSHS bool@false\n"
        "RETURN\n");
}

void generateNonEquality() {
    printf(
        "LABEL %%NONEQUALITY\n"
        "CALL %%COMPARE_DTYPES\n"
        "POPS GF@%%RAX\n"
        "JUMPIFEQ %%NONEQUALITY-EXIT GF@%%RAX bool@false\n"

        "JUMPIFNEQS %%NONEQUALITY-EXIT\n"
        "PUSHS bool@false\n"
        "RETURN\n"

        "LABEL %%NONEQUALITY-TRUE\n"
        "PUSHS bool@true\n"
        "RETURN\n"

        "LABEL %%NONEQUALITY-EXIT\n"
        "PUSHS bool@true\n"
        "RETURN\n");
}

void generateLess() {
    printf(
        "LABEL %%LESS\n"
        // Compare with null.
        "POPS GF@%%RAX\n"
        "POPS GF@%%RBX\n"
        "PUSHS GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "TYPE GF@%%RAX GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RBX\n"
        "JUMPIFEQ %%LESS-FALSE GF@%%RAX nil@nil\n"
        "JUMPIFEQ %%LESS-FALSE GF@%%RBX nil@nil\n"
        "LTS\n"
        "RETURN\n"

        "LABEL %%LESS-FALSE\n"
        "PUSHS bool@false\n"
        "RETURN\n");
}

void generateEmptyStringToInt() {
    printf(
        "LABEL %%EMPTY_STRINGS_TO_INT\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"
        "JUMPIFEQ %%EMPTY_STRINGS_TO_INT-STRING GF@%%RBX string@string\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"

        "LABEL %%EMPTY_STRINGS_TO_INT-STRING\n"
        "JUMPIFNEQ %%EMPTY_STRINGS_TO_INT-NONEMPTY GF@%%RAX string@\n"
        "PUSHS int@0\n"
        "RETURN\n"

        "LABEL %%EMPTY_STRINGS_TO_INT-NONEMPTY\n"
        "EXIT int@7\n");
}

void generateLessEqual() {
    printf(
        "LABEL %%LESS_EQUAL\n"
        "CALL %%EMPTY_STRINGS_TO_INT\n"
        "CALL %%STACK_SWAP\n"
        "CALL %%EMPTY_STRINGS_TO_INT\n"
        "CALL %%STACK_SWAP\n"
        "CALL %%NULL_TO_INT\n"
        "CALL %%NORMALIZE_NUMERIC_TYPES\n"

        "GTS\n"
        "POPS GF@%%RAX\n"
        "JUMPIFEQ %%LESS_EQUAL-FALSE GF@%%RAX bool@false\n"
        "PUSHS bool@false\n"
        "RETURN\n"

        "LABEL %%LESS_EQUAL-FALSE\n"
        "PUSHS bool@true\n"
        "RETURN\n");
}

void generateGreatEqual() {
    printf(
        "LABEL %%GREAT_EQUAL\n"
        "CALL %%EMPTY_STRINGS_TO_INT\n"
        "CALL %%STACK_SWAP\n"
        "CALL %%EMPTY_STRINGS_TO_INT\n"
        "CALL %%STACK_SWAP\n"
        "CALL %%NULL_TO_INT\n"
        "CALL %%NORMALIZE_NUMERIC_TYPES\n"

        "LTS\n"
        "POPS GF@%%RAX\n"
        "JUMPIFEQ %%GREAT_EQUAL-FALSE GF@%%RAX bool@false\n"
        "PUSHS bool@false\n"
        "RETURN\n"

        "LABEL %%GREAT_EQUAL-FALSE\n"
        "PUSHS bool@true\n"
        "RETURN\n");
}

void generateGreat() {
    printf(
        "LABEL %%GREAT\n"
        // Compare with null.
        "POPS GF@%%RAX\n"
        "POPS GF@%%RBX\n"
        "PUSHS GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "TYPE GF@%%RAX GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RBX\n"
        "JUMPIFEQ %%GREAT-FALSE GF@%%RAX nil@nil\n"
        "JUMPIFEQ %%GREAT-FALSE GF@%%RBX nil@nil\n"
        "GTS\n"
        "RETURN\n"

        "LABEL %%GREAT-FALSE\n"
        "PUSHS bool@false\n"
        "RETURN\n");
}

void generateToBoolFunction() {
    printf(
        // Pops the top of the stack into RAX and checks what type it is
        "LABEL %%TO_BOOL\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"
        "JUMPIFEQ %%BOOL GF@%%RBX string@bool\n"
        "JUMPIFEQ %%STRING GF@%%RBX string@string\n"
        "JUMPIFEQ %%INT GF@%%RBX string@int\n"
        "JUMPIFEQ %%FLOAT GF@%%RBX string@float\n"
        "JUMPIFEQ %%VALUE_IS_FALSE GF@%%RBX string@nil\n"

        // If the type is bool, nothing needs to be converted, so just push back the original value
        "LABEL %%BOOL\n"
        "PUSHS GF@%%RAX\n"
        "JUMP %%RET_BOOL\n"
        "RETURN\n"

        // If the type is string and the content is "0", push false as the return value
        // Otherwise push true
        "LABEL %%STRING\n"
        "JUMPIFEQ %%VALUE_IS_FALSE GF@%%RAX string@0\n"
        "JUMPIFEQ %%VALUE_IS_FALSE GF@%%RAX string@\n"
        "PUSHS bool@true\n"
        "JUMP %%RET_BOOL\n"

        // If the type is and the content is 0, push false as the return value
        // Otherwise push true
        "LABEL %%INT\n"
        "JUMPIFEQ %%VALUE_IS_FALSE GF@%%RAX int@0\n"
        "PUSHS bool@true\n"
        "JUMP %%RET_BOOL\n"

        "LABEL %%FLOAT\n"
        "JUMPIFEQ %%VALUE_IS_FALSE GF@%%RAX float@0x0.0p+0\n"
        "PUSHS bool@true\n"
        "JUMP %%RET_BOOL\n"

        "LABEL %%VALUE_IS_FALSE\n"
        "PUSHS bool@false\n"

        "LABEL %%RET_BOOL\n"
        "RETURN\n");
}

void convertExpResultToBoolValue() {
    printf("CALL %%TO_BOOL\n");
}

int countEscapeSequences(char *string) {
    int count = 0;
    for (size_t i = 0; string[i] != '\0'; i++) {
        if ((string[i] >= 0 && string[i] <= 32) || string[i] == 35 || string[i] == 92) {
            count++;
        }
    }
    return count;
}

void generateExpressionCode(Nonterminal *root, bool isLeftSideOfAssignment, ht_table_t *localSymtable, ht_table_t* globalSymtable) {
    if (!root) {
        return;
    }
    if (root->NTType != EXPR) {
        switch (root->NTType) {
            case EMPTY:
                printf("PUSHS nil@nil\n");
                break;
            case VAR_ID_TERM:
                if (!isLeftSideOfAssignment) {
                    if(!ht_get(localSymtable, root->term.var->name)){
                        semanticError(5);
                    }
                    printf("TYPE GF@%%RBX LF@%s\n", root->term.var->name);
                    printf("JUMPIFEQ %%ERROR_5 GF@%%RBX string@\n");
                    printf("PUSHS LF@%s\n", root->term.var->name);  
                }
                break;
            case LITERAL_TERM:
                switch (root->dType) {
                    case INT:
                        printf("PUSHS int@%lld\n", root->term.integerLit);
                        break;
                    case FLOAT:
                        printf("PUSHS float@%a\n", root->term.floatLit);
                        break;
                    case STRING:
                        ;
                        int newSize = countEscapeSequences(root->term.stringLit) * 5 * sizeof(char);
                        char *newString = calloc(strlen(root->term.stringLit) * sizeof(char) + newSize + 4*sizeof(char), 1);
                        char buffer[7] = {'\0', };
                        for (size_t i = 0; root->term.stringLit[i] != '\0'; i++) {
                            if ((root->term.stringLit[i] >= 0 && root->term.stringLit[i] <= 32) || root->term.stringLit[i] == 35 || root->term.stringLit[i] == 92) {
                                sprintf(buffer, "\\0%d", root->term.stringLit[i]);
                            } else {
                                sprintf(buffer, "%c", root->term.stringLit[i]);
                            }
                            strcat(newString, buffer);
                        }

                        printf("PUSHS string@%s\n", newString);
                        break;
                    case NULL_T:
                        printf("PUSHS nil@nil\n");
                        break;
                    default:
                        break;
                }
                break;
            case FUNCALL_TERM:
                // generate semantic check of params
                // evaluate all args right to left
                // create frame
                // call function
                // check what it returned
                // clean up??
                // profit??
                ;
                symtableElem *func = ht_get(globalSymtable, root->term.func->funId);
                // Function doesnt exist
                if (!func) semanticError(3);

                // Function has the correct number of arguments OR it can have N arguments (write() for example)
                else if((!func->f->args || func->f->args->len == root->term.func->args->len)){
                    
                    // Functions that can have N arguments: we have to push in opposite order
                    if(!func->f->args){
                        nontermListLast(root->term.func->args);
                        Nonterminal* iter;    
                        for(int i = 0; i < root->term.func->args->len; i++){
                            iter = nontermListGetValue(root->term.func->args);
                            generateExpressionCode(iter, false, localSymtable, globalSymtable);
                            nontermListPrev(root->term.func->args);
                        }
                        printf("PUSHS int@%d\n", root->term.func->args->len);
                    }
                    else{
                        nontermListFirst(root->term.func->args);
                        Nonterminal* iter;    
                        for(int i = 0; i < root->term.func->args->len; i++){
                            iter = nontermListGetValue(root->term.func->args);
                            generateExpressionCode(iter, false, localSymtable, globalSymtable);
                            nontermListNext(root->term.func->args);
                        }
                    }
                    
                    printf("CREATEFRAME\n");
                    printf("PUSHFRAME\n");
                    printf("CALL _%s\n", root->term.func->funId);
                    printf(
                        "POPS GF@%%RAX\n"
                        "PUSHS GF@%%RAX\n"
                        "PUSHS GF@%%RAX\n"
                        );
                    printf("PUSHS string@%s\n", enumTypeToStr(func->f->returnType));
                    if(!func->f->nullable){
                        printf(
                            "CALL %%CHECK_IF_IS_TYPE\n"
                            "PUSHS bool@false\n"
                            "JUMPIFEQS %%ERROR_4\n"
                            );   
                    }
                    else{
                        printf(
                            "CALL %%CHECK_IF_IS_TYPE_OR_NULL\n"
                            "PUSHS bool@false\n"
                            "JUMPIFEQS %%ERROR_4\n"
                        );  
                    }
                    printf("POPFRAME\n");
                    break;                
                }

                // Function call has a different number of arguments than the definition
                else if(func->f->args->len != root->term.func->args->len){
                    semanticError(4);
                }

            default:
                break;
        }
    } else {
        if (root->expr.op == AS) {
            // Assignments are traversed using reverse postorder.
            generateExpressionCode(root->expr.right, false, localSymtable, globalSymtable);
            generateExpressionCode(root->expr.left, true, localSymtable, globalSymtable);
        } else {
            // All other expressions are traversed using normal postorder.
            generateExpressionCode(root->expr.left, false, localSymtable, globalSymtable);
            generateExpressionCode(root->expr.right, false, localSymtable, globalSymtable);
        }
        switch (root->expr.op) {
            case PLUS:
                printf(
                    "CALL %%NORMALIZE_NUMERIC_TYPES\n"
                    "ADDS\n");
                break;

            case MINUS:
                printf(
                    "CALL %%NORMALIZE_NUMERIC_TYPES\n"
                    "SUBS\n");
                break;

            case DIV:
                printf(
                    "CALL _floatval\n"
                    "CALL %%STACK_SWAP\n"
                    "CALL _floatval\n"
                    "CALL %%STACK_SWAP\n"
                    "DIVS\n");
                break;

            case MUL:
                printf(
                    "CALL %%NORMALIZE_NUMERIC_TYPES\n"
                    "MULS\n");
                break;

            case CAT:
                printf(
                    "PUSHS string@string\n"
                    "CALL %%ENFORCE_TYPES\n"
                    "CALL %%STACK_SWAP\n"

                    "PUSHS string@string\n"
                    "CALL %%ENFORCE_TYPES\n"
                    "POPS gf@%%RBX\n"
                    "POPS gf@%%RAX\n"

                    "CONCAT GF@%%RAX gf@%%RBX gf@%%RAX\n"
                    "PUSHS gf@%%RAX\n");

                break;

            case EQ:
                printf("CALL %%EQUALITY\n");
                break;

            case NEQ:
                printf("CALL %%NONEQUALITY\n");
                break;

            case L:
                printf("CALL %%LESS\n");
                break;

            case LEQ:
                printf("CALL %%LESS_EQUAL\n");
                break;

            case G:
                printf("CALL %%GREAT\n");
                break;

            case GEQ:
                printf("CALL %%GREAT_EQUAL\n");
                break;

            case AS:
                printf("POPS LF@%s\n", root->expr.left->term.var->name);
                printf("PUSHS LF@%s\n", root->expr.left->term.var->name);
                return;

            default:
                break;
        }
    }
}

void generateBuiltInFunctions() {
    // reads() READ STRING
    printf(
        "LABEL _reads\n"
        "READ GF@%%RAX string\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");

    // readi() READ INT
    printf(
        "LABEL _readi\n"
        "READ GF@%%RAX int\n"
        "TYPE GF@%%RBX GF@%%RAX\n"
        "JUMPIFEQ %%READI_RET GF@%%RBX string@int\n"
        "MOVE GF@%%RAX nil@nil\n"
        "LABEL %%READI_RET\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");
    // readf() READ FLOAT
    printf(
        "LABEL _readf\n"
        "READ GF@%%RAX float\n"
        "TYPE GF@%%RBX GF@%%RAX\n"
        "JUMPIFEQ %%READI_RET GF@%%RBX string@float\n"
        "MOVE GF@%%RAX nil@nil\n"

        "LABEL %%READF_RET\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");

    // write()
    printf(
        // Since this function can have any number of arguments,
        // the number of arguments will be pushed together with the arguments themselves
        "LABEL _write\n"

        // Pop the number of args into RAX
        "POPS GF@%%RAX\n"

        // pop the argument into RBX, write, RAX-- , loop
        "LABEL %%WRITE_LOOP\n"
        "JUMPIFEQ %%WRITE_RETURN GF@%%RAX int@0\n"
        "POPS GF@%%RBX\n"
        "WRITE GF@%%RBX\n"
        "SUB GF@%%RAX GF@%%RAX int@1\n"
        "JUMP %%WRITE_LOOP\n"

        "LABEL %%WRITE_RETURN\n"
        "PUSHS nil@nil\n"
        "RETURN\n");

    // floatval()
    printf(
        "LABEL _floatval\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"

        "JUMPIFEQ %%FLOATVAL_INT GF@%%RBX string@int\n"
        "JUMPIFEQ %%FLOATVAL_END GF@%%RBX string@float\n"
        "JUMPIFEQ %%FLOATVAL_NULL GF@%%RBX string@nil\n"

        "EXIT int@7\n"
        "LABEL %%FLOATVAL_NULL\n"
        "MOVE GF@%%RAX float@0x0.0p+0\n"
        "JUMP %%FLOATVAL_END\n"

        "LABEL %%FLOATVAL_INT\n"
        "INT2FLOAT GF@%%RAX GF@%%RAX\n"

        "LABEL %%FLOATVAL_END\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");

    // intval()
    printf(
        "LABEL _intval\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"

        "JUMPIFEQ %%INTVAL_END GF@%%RBX string@int\n"
        "JUMPIFEQ %%INTVAL_FLOAT GF@%%RBX string@float\n"
        "JUMPIFEQ %%INTVAL_NULL GF@%%RBX string@nil\n"

        "EXIT int@7\n"
        "LABEL %%INTVAL_NULL\n"
        "MOVE GF@%%RAX int@0\n"
        "JUMP %%INTVAL_END\n"

        "LABEL %%INTVAL_FLOAT\n"
        "FLOAT2INT GF@%%RAX GF@%%RAX\n"

        "LABEL %%INTVAL_END\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");

    // strval()
    printf(
        "LABEL _strval\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"

        "JUMPIFEQ %%STRVAL_END GF@%%RBX string@string\n"
        "JUMPIFEQ %%STRVAL_NULL GF@%%RBX string@nil\n"

        "EXIT int@7\n"

        "LABEL %%STRVAL_NULL\n"
        "MOVE GF@%%RAX string@\n"

        "LABEL %%STRVAL_END\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");

    // strlen()
    printf(
        "LABEL _strlen\n"
        "DEFVAR LF@$s\n"
        "POPS LF@$s\n"
        "PUSHS LF@$s\n"
        "PUSHS string@string\n"
        "CALL %%CHECK_IF_IS_TYPE_OR_NULL\n"
        "PUSHS bool@false\n"
        "JUMPIFEQS %%ERROR_4\n"

        "STRLEN GF@%%RAX LF@$s\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");


    // substring()
    printf(
        "LABEL _substring\n"
        // Define local vars
        "DEFVAR LF@$i\n"
        "DEFVAR LF@$j\n"
        "DEFVAR LF@$s\n"

        // Pop args from stack
        "POPS LF@$j\n"
        "POPS LF@$i\n"
        "POPS LF@$s\n"

        // check types
        "PUSHS LF@$s\n"
        "PUSHS string@string\n"
        "CALL %%CHECK_IF_IS_TYPE\n"
        "PUSHS bool@false\n"
        "JUMPIFEQS %%ERROR_4\n"
        

        "PUSHS LF@$i\n"
        "PUSHS string@int\n"
        "CALL %%CHECK_IF_IS_TYPE\n"
        "PUSHS bool@false\n"
        "JUMPIFEQS %%ERROR_4\n"

        "PUSHS LF@$j\n"
        "PUSHS string@int\n"
        "CALL %%CHECK_IF_IS_TYPE\n"
        "PUSHS bool@false\n"
        "JUMPIFEQS %%EXIT_4\n"



        // check if $i < 0
        "LT GF@%%RBX LF@$i int@0\n"
        "JUMPIFEQ %%SUBSTR_ERROR GF@%%RBX bool@true\n"

        // check if $j < 0
        "LT GF@%%RBX LF@$j int@0\n"
        "JUMPIFEQ %%SUBSTR_ERROR GF@%%RBX bool@true\n"

        // check if $i > $j
        "GT GF@%%RBX LF@$i LF@$j\n"
        "JUMPIFEQ %%SUBSTR_ERROR GF@%%RBX bool@true\n"

        // check if $i >= strlen($s)
        "STRLEN GF@%%RAX LF@$s\n"
        "PUSHS LF@$i\n"
        "PUSHS GF@%%RAX\n"
        "GTS\n"
        "PUSHS LF@$i\n"
        "PUSHS GF@%%RAX\n"
        "EQS\n"
        "ANDS\n"
        "PUSHS bool@true\n"
        "JUMPIFEQS %%SUBSTR_ERROR\n"

        // check if $j > strlen($s)
        "PUSHS LF@$j\n"
        "PUSHS GF@%%RAX\n"
        "GTS\n"
        "PUSHS bool@true\n"
        "JUMPIFEQS %%SUBSTR_ERROR\n"

        // move empty string to RAX
        "MOVE GF@%%RAX string@\n"

        // getchar and concat until $i = $j
        "LABEL %%SUBSTR_LOOP\n"
        "PUSHS LF@$i\n"
        "PUSHS LF@$j\n"
        "JUMPIFEQS %%SUBSTR_LOOP_END\n"
        "GETCHAR GF@%%RBX LF@$s LF@$i\n"
        "CONCAT GF@%%RAX GF@%%RAX GF@%%RBX\n"
        "ADD LF@$i LF@$i int@1\n"
        "JUMP %%SUBSTR_LOOP\n"

        // push result and return
        "LABEL %%SUBSTR_LOOP_END\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"

        // return null if error
        "LABEL %%SUBSTR_ERROR\n"
        "PUSHS nil@nil\n"
        "RETURN\n"

    );

    // ord()
    printf(
        "LABEL _ord\n"
        "DEFVAR LF@s\n"
        "POPS LF@s\n"

        "PUSHS LF@s\n"
        "PUSHS string@string\n"
        "CALL %%CHECK_IF_IS_TYPE\n"
        "PUSHS bool@false\n"
        "JUMPIFEQS %%ERROR_4\n"

        "STRLEN GF@%%RAX LF@s\n"
        "PUSHS GF@%%RAX\n"
        "PUSHS int@0\n"
        "JUMPIFEQS %%ORD_EMPTY\n"

        "STRI2INT GF@%%RAX LF@s int@0\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"

        "LABEL %%ORD_EMPTY\n"
        "PUSHS int@0\n"
        "RETURN\n");

    // chr()

    printf(
        "LABEL _chr\n"
        "DEFVAR LF@i\n"
        "POPS LF@i\n"

        "PUSHS LF@i\n"
        "PUSHS string@int\n"
        "CALL %%CHECK_IF_IS_TYPE\n"
        "PUSHS bool@false\n"
        "JUMPIFEQS %%ERROR_4\n"
        "INT2CHAR GF@%%RAX LF@i\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n");
}

char* enumTypeToStr(dataType dType){
    switch(dType){
        case INT:
            return "int";
        case FLOAT:
            return "float";
        case STRING:
            return "string";
        case NULL_T:
            return "nil";    
        case UNKNOWN:
            return NULL;      
    }
    return NULL;
}
