#include "codeGenerator.h"

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

void generateStackSwap() {
    printf("LABEL %%STACK_SWAP\n"
           "BREAK\n"
           "POPS GF@%%RAX\n"
           "POPS GF@%%RBX\n"
           "PUSHS GF@%%RAX\n"
           "PUSHS GF@%%RBX\n"
           "BREAK\n"
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
                        printf("PUSHS int@%ld\n", (long int) root->term.integerLit);
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
                //TODO                    
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
                printf("ADDS\n");
                break;

            case MINUS:
                printf("SUBS\n");
                break;

            case DIV:
                printf("CALL %%TRY_INT_2_FLOAT\n"
                       "CALL %%STACK_SWAP\n"
                       "CALL %%TRY_INT_2_FLOAT\n"
                       "CALL %%STACK_SWAP\n"
                       "DIVS\n");
                break;

            case MUL:
                printf("MULS\n");
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
