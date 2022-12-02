#include "codeGenerator.h"

void generateStarterAsm() {
    printf(".IFJcode22\n");
    printf("DEFVAR GF@%%RAX\n");
    printf("DEFVAR GF@%%RBX\n");
    printf("CREATEFRAME\nPUSHFRAME\n");
    printf("JUMP %%PROG_START\n");
    generateToBoolFunction();
    //generateToFloatFunction();
    generateEnforceTypesFunction();
    generateStackSwapFunction();
    generateNormalizeNumericTypesFunction();
    generateNullToIntFunction();
    generateBuiltInFunctions();
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


void generateBuiltInFunctions(){
    // reads() READ STRING
    printf(
        "LABEL %%READS\n"
        "READ GF@%%RAX string\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"
        );

    // readi() READ INT    
    printf(
        "LABEL %%READI\n"
        "READ GF@%%RAX int\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"
        );
    // readf() READ FLOAT    
    printf(
        "LABEL %%READF\n"
        "READ GF@%%RAX float\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"
        );

    // write()    
    printf(
        // Since this function can have any number of arguments,
        // the number of arguments will be pushed together with the arguments themselves
        "LABEL %%WRITE\n"

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
        "RETURN\n"
    );

    // floatval()
    printf(
        "LABEL %%FLOATVAL\n"
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
        "RETURN\n"
    );

    // intval()
    printf(
        "LABEL %%INTVAL\n"
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
        "RETURN\n"
    );

    // strval()
    printf(
        "LABEL %%STRVAL\n"
        "POPS GF@%%RAX\n"
        "TYPE GF@%%RBX GF@%%RAX\n"

        "JUMPIFEQ %%STRVAL_END GF@%%RBX string@string\n"
        "JUMPIFEQ %%STRVAL_NULL GF@%%RBX string@nil\n"

        "EXIT int@7\n"

        "LABEL %%STRVAL_NULL\n"
        "MOVE GF@%%RAX string@\n"

        "LABEL %%STRVAL_END\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"
    );

    // strlen()
    printf(
        "LABEL %%STRLEN\n"
        "POPS GF@%%RAX\n"
        "STRLEN GF@%%RAX GF@%%RAX\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"
    );

    // TODO create frames for these builtin functions

    // substring()
    printf(
        // Define local vars
        "DEFVAR LF@$i\n"
        "DEFVAR LF@$j\n"
        "DEFVAR LF@$s\n"

        // Pop args from stack
        "POPS LF@$s\n"
        "POPS LF@$i\n"
        "POPS LF@$j\n"

        // check if $i < 0
        "LT GF@%%RBX LF@$i int@0\n"
        "JUMPIFEQ %%SUBSTR_ERROR GF@%%RBX bool@true\n"

        // check if $j < 0
        "LT GF@%%RBX LF@$j int@0\n"
        "JUMPIFEQ %%SUBSTR_ERROR GF@%%RBX bool@true\n"

        // check if $i > $j
        "GT GF@%%RBX LF@$i LF@$j\n"
        "JUMPIFEQ %%SUBSTR_ERROR GF@%%RBX bool@true\n"

        //check if $i >= strlen($s)
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
        "LABEL %%ORD\n"
        "POPS GF@%%RBX\n"

        "STRLEN GF@%%RAX GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "PUSHS int@0\n"
        "JUMPIFEQS %%ORD_EMPTY\n"

        "STRI2INT GF@%%RAX GF@%%RBX int@0\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"

        "LABEL %%ORD_EMPTY\n"
        "PUSHS int@0\n"
        "RETURN\n"
    );

    // chr()

    printf(
        "LABEL %%CHR\n"
        "POPS GF@%%RBX\n"

        "INT2CHAR GF@%%RAX GF@%%RBX\n"
        "PUSHS GF@%%RAX\n"
        "RETURN\n"
    );    
}

