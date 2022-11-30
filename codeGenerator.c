#include "codeGenerator.h"

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
            generateExpressionCode(root->expr.right, false);
            generateExpressionCode(root->expr.left, true);
        }
        else{
            generateExpressionCode(root->expr.right, false);
            generateExpressionCode(root->expr.left, false);
        }
        char* operation; 
        switch(root->expr.op){
            case PLUS:
                operation = "ADDS";
                break;
            case MINUS:
                operation = "SUBS";
                break;
            case DIV:
                operation = "POPS gf@%%rax\nPOPS gf@%%rbx\nPUSHS gf@%%rax\nPUSHS gf@%%rbx\nDIVS";
                break;
            case MUL:
                operation = "MULS";
                break;
            case AS:
                printf("DEFVAR LF@$%s\n", root->expr.left->term.var->name);
                printf("POPS LF@$%s\n", root->expr.left->term.var->name);
                printf("PUSHS LF@$%s\n", root->expr.left->term.var->name);
                return;   
            default: break;    
        }
        printf("%s\n", operation);
    }
}