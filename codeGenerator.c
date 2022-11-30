#include "codeGenerator.h"

void generateStarterAsm() {
    printf(".IFJcode22\n");
    printf("DEFVAR GF@%%RAX\n");
    printf("DEFVAR GF@%%RBX\n");
    printf("CREATEFRAME\nPUSHFRAME\n");
    printf("JUMP %%PROG_START\n");
    generateToBoolFunction();
    generateToFloatFunction();
    generateEnforceTypesFunction();
    generateStackSwapFunction();
    generateNormalizeNumericTypesFunction();
    generateNullToIntFunction();
    printf("LABEL %%PROG_START\n");
}

/**
 * Expects (top to bottom) var of unknown type and string containing type name.
 * Pops only type string.
 * Exits if the variable doesn't have the given type.
 */
void generateEnforceTypesFunction() {
    printf("LABEL %%ENFORCE_TYPES\n"
           // Popping type string
           "POPS GF@%%RAX\n"
           // Popping value
           "POPS GF@%%RBX\n"
           "PUSHS GF@%%RBX\n"

           "TYPE GF@%%RBX GF@%%RBX\n"
           "JUMPIFEQ %%GEN_ENF_TYP-END GF@%%RAX GF@%%RBX\n"
           "EXIT int@7\n"
           "LABEL %%GEN_ENF_TYP-END\n"
           "RETURN\n"
    );
}

/**
 * Expects a variable of an unknown type on the stack.
 * Pops all arguments.
 * Returns on the stack the float representation of the variable or exits.
 */
void generateToFloatFunction() {
    printf("LABEL %%TRY_INT_2_FLOAT\n"
           "POPS GF@%%RAX\n"
           "TYPE GF@%%RBX GF@%%RAX\n"
           "JUMPIFEQ %%TRY_INT_2_FLOAT-CONV GF@%%RBX string@int\n"
           "JUMPIFEQ %%TRY_INT_2_FLOAT-END GF@%%RBX string@float\n"
           "EXIT int@7\n"
           "LABEL %%TRY_INT_2_FLOAT-CONV\n"
           "INT2FLOAT GF@%%RAX GF@%%RAX\n"
           "LABEL %%TRY_INT_2_FLOAT-END\n"
           "PUSHS GF@%%RAX\n"
           "RETURN\n"
    );
}

void generateNullToIntFunction() {
    printf("LABEL %%NULL_TO_INT\n"
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
           "BREAK\n"
           "RETURN\n"
    );
}

void generateNormalizeNumericTypesFunction() {
    printf("LABEL %%NORMALIZE_NUMERIC_TYPES\n"
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
           "RETURN\n"
    );
}

/**
 * Swaps two topmost values on the stack
 */
void generateStackSwapFunction() {
    printf("LABEL %%STACK_SWAP\n"
           "POPS GF@%%RAX\n"
           "POPS GF@%%RBX\n"
           "PUSHS GF@%%RAX\n"
           "PUSHS GF@%%RBX\n"
           "RETURN\n"
    );
}


void generateExpressionCode(Nonterminal* root, bool isLeftSideOfAssignment){
    // NAJIT NEJPRAVJEJSI LIST
    // PUSHNOUT NA STACK 
    // JIT NAHORU A DOLEVA
    // PUSHNOUT NA STACK
    // JIT NAHORU
    // ZJISTIT OPERATOR
    // CASE PRO VYBER OPERACE
    // PUSHNOUT TEMP NA ULOZENI VYSLEDKU
    // 
    // PROFIT???
    if(!root){
        return;
    }
    if(root->NTType != EXPR){
        switch(root->NTType){
            case VAR_ID_TERM:
                if(!isLeftSideOfAssignment){
                    printf("PUSHS LF@$%s\n", root->term.var->name);
                }
                break;
            case LITERAL_TERM:
                switch(root->dType){
                    case INT:
                        printf("PUSHS int@%lld\n", root->term.integerLit);
                        break;
                    case FLOAT:
                        printf("PUSHS float@%a\n", root->term.floatLit);
                        break;
                    case STRING:
                        printf("PUSHS string@%s\n", root->term.stringLit);
                        break;
                    case NULL_T:
                        printf("PUSHS nil@nil\n");          
                    default: break;     
                }
            case FUNCALL_TERM:
                //TODO :omegaxd:
                break;
            default: break;    
        }
        
    }
    else{
        if(root->expr.op == AS){
            // Assignments are traversed using reverse postorder.
            generateExpressionCode(root->expr.right, false);
            generateExpressionCode(root->expr.left, true);
        }
        else{
            // All other expressions are traversed using normal postorder.
            generateExpressionCode(root->expr.left, false);
            generateExpressionCode(root->expr.right, false);
        }
        switch(root->expr.op){
            case PLUS:
                printf("CALL %%NORMALIZE_NUMERIC_TYPES\n"
                       "ADDS\n"
                 );
                break;

            case MINUS:
                printf("CALL %%NORMALIZE_NUMERIC_TYPES\n"
                       "SUBS\n"
                       "POPS GF@%%RAX\n"
                       "PUSHS GF@%%RAX\n"
                       "WRITE GF@%%RAX\n"
                );
                break;

            case DIV:
                printf("CALL %%TRY_INT_2_FLOAT\n"
                       "CALL %%STACK_SWAP\n"
                       "CALL %%TRY_INT_2_FLOAT\n"
                       "CALL %%STACK_SWAP\n"
                       "DIVS\n"
                );
                break;

            case MUL:
                printf("CALL %%NORMALIZE_NUMERIC_TYPES\n"
                       "MULS\n"
                );
                break;

            case CAT:
                printf("PUSHS string@string\n"
                       "CALL %%ENFORCE_TYPES\n"
                       "CALL %%STACK_SWAP\n"

                       "PUSHS string@string\n"
                       "CALL %%ENFORCE_TYPES\n"
                       "POPS gf@%%RBX\n"
                       "POPS gf@%%RAX\n"

                       "CONCAT GF@%%RAX gf@%%RBX gf@%%RAX\n"
                       "PUSHS gf@%%RAX\n");

                break;

            case AS:
                printf("DEFVAR LF@$%s\n", root->expr.left->term.var->name);
                printf("POPS LF@$%s\n", root->expr.left->term.var->name);
                printf("PUSHS LF@$%s\n", root->expr.left->term.var->name);
                return;

            default: break;    
        }
    }
}

void generateToBoolFunction(){
    printf(
        // Pops the top of the stack into RAX and checks what type it is
        "LABEL %%TO_BOOL\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"
        "JUMPIFEQ %%BOOL GF@%%RBX string@bool\n"
        "JUMPIFEQ %%STRING GF@%%RBX string@string\n"
        "JUMPIFEQ %%INT GF@%%RBX string@int\n"
        "JUMPIFEQ %%FLOAT GF@%%RBX string@float\n"

        // If the type is bool, nothing needs to be converted, so just push back the original value
        "LABEL %%BOOL\n"
        "PUSHS GF@%%RAX\n"
        "JUMP %%RET_BOOL\n"
        "RETURN\n"

        // If the type is string and the content is "0", push false as the return value
        // Otherwise push true
        "LABEL %%STRING\n"
        "JUMPIFEQ %%VALUE_IS_FALSE GF@%%RAX string@0\n"
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
        "RETURN\n"
        );
}


void convertExpResultToBoolValue(){
    printf("CALL %%TO_BOOL\n");
}
