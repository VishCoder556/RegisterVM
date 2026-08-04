// Dump PM format in a human-readable format
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define PM_IMPLEMENTATION
#include "pm.c"

char *pm_register(uint64_t reg){
    char str[6]; 
    snprintf(str, 6, "r%llu", reg);
    return strdup(str);
}

char *pm_operand(Pm_Operand operand){
    if (operand.type == PM_OPERAND_REG){
        return pm_register(operand.value);
    }else if (operand.type == PM_OPERAND_IMM){
        char str[10]; 
        snprintf(str, 10, "#%lld", operand.value);
        return strdup(str);
    }
    return NULL;
};


int main(int argc, char **argv){
    char *input_file = NULL;
    for (int i=0; i<argc; i++){
        input_file = argv[i];
    };
    if (input_file == NULL){
        fprintf(stderr, "pme: no input file provided\n");
        exit(-1);
    }
    Pm *pm = pm_read_from_file(input_file);
    if (pm == NULL){
        fprintf(stderr, "pme: could not get PM from `%s`\n", input_file);
        exit(-1);
    }
    printf("%%code\n");
    for (int i=0; i<pm->instrlen; i++){
        Pm_Instr instr = pm->instrs[i];
        if (instr.type == PM_INSTR_MOV_IMM){
            printf("\tmov_imm %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left));
        }else if (instr.type == PM_INSTR_MOV_REG){
            printf("\tmov_reg %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left));
        }else if (instr.type == PM_INSTR_ADD){
            printf("\tadd %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_SUB){
            printf("\tsub %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_MUL){
            printf("\tmul %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_DIV){
            printf("\tdiv %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_EQ){
            printf("\tsete %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_GT){
            printf("\tsetgt %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_GTE){
            printf("\tsetgte %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_LT){
            printf("\tsetlt %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_LTE){
            printf("\tsetlte %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_STORE8){
            printf("\tstore8 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_STORE16){
            printf("\tstore16 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_STORE32){
            printf("\tstore32 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_STORE64){
            printf("\tstore64 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_LOAD8){
            printf("\tload8 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_LOAD16){
            printf("\tload16 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_LOAD32){
            printf("\tload32 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_LOAD64){
            printf("\tload64 %s, %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left), pm_operand(instr.right));
        }else if (instr.type == PM_INSTR_LOAD_STR){
            printf("\tload_str %s, %s\n", pm_operand(instr.dest), pm_operand(instr.left));
        }else if (instr.type == PM_INSTR_JMP){
            printf("\tjmp %lld\n", instr.dest.value);
        }else if (instr.type == PM_INSTR_CALL){
            printf("\tcall %lld\n", instr.dest.value);
        }else if (instr.type == PM_INSTR_NATIVE){
            printf("\tnative %s\n", (char*)&instr.dest.value);
        }else if (instr.type == PM_INSTR_JMPIF){
            printf("\tjmpif %s, %lld\n", pm_operand(instr.dest), instr.left.value);
        }else if (instr.type == PM_INSTR_JMPIFN){
            printf("\tjmpifn %s, %lld\n", pm_operand(instr.dest), instr.left.value);
        }else if (instr.type == PM_INSTR_HALT){
            printf("\thalt\n");
        }else if (instr.type == PM_INSTR_RET){
            printf("\tret\n");
        }else {
            fprintf(stderr, "pdump: unreachable code\n");
            exit(-1);
        }
    }
    printf("%%pool\n");
    if (pm->poollen == 0){
        printf("\n");
    }else {
        printf("\t");
    }
    ;
    char *temp = malloc(6);
    for (int i=0; i<pm->poollen; i++){
        if (i+1 == pm->poollen){
            printf("%"PRIx8"\n", pm->pool[i]);
        }else if ((i % 13 == 0 && i != 0)){
            printf("%"PRIx8"\n\t", pm->pool[i]);
        }else {
            printf("%"PRIx8" ", pm->pool[i]);
        }
    }
}
