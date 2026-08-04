#include <stdio.h>
#include <stdlib.h>
#define PM_IMPLEMENTATION
#include "pm.c"

#define pmc_error(...) \
fprintf(stderr, __VA_ARGS__); \
exit(-1);

char *pmc_translate_operand(int s, Pm *pm, Pm_Operand operand){
    char data[100];
    if (operand.type == PM_OPERAND_IMM){
        snprintf(data, 100, "%lld", operand.value);
    }else if (operand.type == PM_OPERAND_REG){
        uint8_t reg = (operand.value) & 0xFF;
        uint8_t _size = (operand.value >> 8) & 0xFF;
        if (s != -1) {_size = s;} // Can override size
        if (reg == 254){
            switch (_size){
                case 1: snprintf(data, 100, "spl");
                case 2: snprintf(data, 100, "sp");
                case 4: snprintf(data, 100, "esp");
                default: snprintf(data, 100, "rsp");
            }
        }else {
            char *size = "qword";
            switch (_size){
                case 1: size = "byte"; break;
                case 2: size = "word"; break;
                case 4: size = "dword"; break;
                default: break;
            }
            snprintf(data, 100, "%s [rbp - %d]", size, reg * 8 + 8);
        }
    }else {
        pmc_error("pmc: invalid operand type `%d` found\n", operand.type);
    };
    return strdup(data); // Make sure to free in all function calls to avoid memory leak
};


#define pm_store(size, r) \
char *dest = pmc_translate_operand(8, pm, instr.dest); \
char *left = pmc_translate_operand(8, pm, instr.left); \
char *right = pmc_translate_operand(8, pm, instr.right); \
fprintf(stdout, "\tmov rax, %s\n", dest); \
fprintf(stdout, "\tadd rax, %s\n", left); \
fprintf(stdout, "\tmov %s, %s\n", r, right); \
fprintf(stdout, "\tmov %s [rax], %s\n", size, r); \
free(dest); \
free(left); \
free(right);

#define pm_trinary(op) \
char *dest = pmc_translate_operand(8, pm, instr.dest); \
char *left = pmc_translate_operand(8, pm, instr.left); \
char *right = pmc_translate_operand(8, pm, instr.right); \
if (instr.dest.type == instr.left.type && instr.dest.value != instr.left.value){; \
    fprintf(stdout, "\tmov %s, %s\n", dest, left); \
} \
fprintf(stdout, "\t"op" %s, %s\n", dest, right); \
free(dest); \
free(left); \
free(right);

#define pm_comparison(op) \
char *dest = pmc_translate_operand(1, pm, instr.dest); \
char *left = pmc_translate_operand(8, pm, instr.left); \
char *right = pmc_translate_operand(8, pm, instr.right); \
fprintf(stdout, "\tcmp %s, %s\n", left, right); \
fprintf(stdout, "\tset"op" %s\n", dest); \
free(dest); \
free(left); \
free(right);

#define pm_load(reg, siz, n) \
char *dest = pmc_translate_operand(n, pm, instr.dest); \
char *left = pmc_translate_operand(8, pm, instr.left); \
char *right = pmc_translate_operand(8, pm, instr.right); \
fprintf(stdout, "\tmov rax, %s\n", left); \
fprintf(stdout, "\tadd rax, %s\n", right); \
fprintf(stdout, "\tmov "reg", "siz" [rax]\n"); \
fprintf(stdout, "\tmov %s, "reg"\n", dest); \
free(dest); \
free(left); \
free(right);



void pmc_translate_x86_pool(Pm *pm){
    fprintf(stdout, "\tconstant_pool: db '");
    for (int i=0; i<pm->poollen; i++){
        if (pm->pool[i] == 0){
            if (i != pm->poollen){
                fprintf(stdout, "', 0, '");
                continue;
            };
        };
        if (pm->pool[i] == '\n'){
            fprintf(stdout, "', 10, '");
        }else {
            fprintf(stdout, "%c", pm->pool[i]);
        }
    }

    fprintf(stdout, "'\n");
};

void pmc_translate_x86_instr(Pm *pm, Pm_Instr instr, int counter){
    fprintf(stdout, "instr_%d:\n", counter);
    if (instr.type == PM_INSTR_MOV_IMM){
        char *left = pmc_translate_operand(8, pm, instr.dest);
        char *right = pmc_translate_operand(8, pm, instr.left);
        fprintf(stdout, "\tmov %s, %s\n", left, right);
        free(left);
        free(right);
    }else if (instr.type == PM_INSTR_MOV_REG){
        char *left = pmc_translate_operand(8, pm, instr.dest);
        char *right = pmc_translate_operand(8, pm, instr.left);
        fprintf(stdout, "\tmov rax, %s\n", right);
        fprintf(stdout, "\tmov %s, rax\n", left);
        free(left);
        free(right);
    }else if (instr.type == PM_INSTR_CALL){
        fprintf(stdout, "\tcall instr_%lld\n", instr.dest.value);
    }else if (instr.type == PM_INSTR_JMP){
        fprintf(stdout, "\tjmp instr_%lld\n", instr.dest.value);
    }else if (instr.type == PM_INSTR_JMPIF){
        char *left = pmc_translate_operand(8, pm, instr.dest);
        fprintf(stdout, "\tmov rax, %s\n", left);
        fprintf(stdout, "\ttest rax, rax\n");
        fprintf(stdout, "\tjz jend_instr_%d\n", counter);
        fprintf(stdout, "\tjmp instr_%lld\n", instr.left.value);
        fprintf(stdout, "jend_instr_%d:\n", counter);
        free(left);
    }else if (instr.type == PM_INSTR_JMPIFN){
        char *left = pmc_translate_operand(8, pm, instr.dest);
        fprintf(stdout, "\tmov rax, %s\n", left);
        fprintf(stdout, "\ttest rax, rax\n");
        fprintf(stdout, "\tjnz jend_instr_%d\n", counter);
        fprintf(stdout, "\tjmp instr_%lld\n", instr.left.value);
        fprintf(stdout, "jend_instr_%d:\n", counter);
        free(left);
    }else if (instr.type == PM_INSTR_STORE64){
        pm_store("qword", "rbx");
    }else if (instr.type == PM_INSTR_STORE32){
        pm_store("dword", "ebx");
    }else if (instr.type == PM_INSTR_STORE16){
        pm_store("word", "bx");
    }else if (instr.type == PM_INSTR_STORE8){
        pm_store("byte", "bl");
    }else if (instr.type == PM_INSTR_LOAD8){
        pm_load("bl", "byte", 1);
    }else if (instr.type == PM_INSTR_LOAD16){
        pm_load("bx", "word", 2);
    }else if (instr.type == PM_INSTR_LOAD32){
        pm_load("ebx", "dword", 4);
    }else if (instr.type == PM_INSTR_LOAD64){
        pm_load("rbx", "qword", 8);
    }else if (instr.type == PM_INSTR_ADD){
        pm_trinary("add");
    }else if (instr.type == PM_INSTR_SUB){
        pm_trinary("sub");
    }else if (instr.type == PM_INSTR_MUL){
        char *dest = pmc_translate_operand(8, pm, instr.dest);
        char *left = pmc_translate_operand(8, pm, instr.left);
        char *right = pmc_translate_operand(8, pm, instr.right);
        fprintf(stdout, "\tmov rax, %s\n", left); // imul needs temp reg, can't directly handle memory
        fprintf(stdout, "\timul rax, %s\n", right);
        fprintf(stdout, "\tmov %s, rax\n", dest);
        free(dest);
        free(left);
        free(right);
    }else if (instr.type == PM_INSTR_DIV){
        pmc_error("pmc: haven't implemented division instruction yet\n");
    }else if (instr.type == PM_INSTR_EQ){
        pm_comparison("e");
    }else if (instr.type == PM_INSTR_LT){
        pm_comparison("l");
    }else if (instr.type == PM_INSTR_LTE){
        pm_comparison("le");
    }else if (instr.type == PM_INSTR_GT){
        pm_comparison("g");
    }else if (instr.type == PM_INSTR_GTE){
        pm_comparison("ge");
    }else if (instr.type == PM_INSTR_LOAD_STR){
        char *left = pmc_translate_operand(8, pm, instr.dest);

        fprintf(stdout, "\tlea rax, [rel constant_pool]\n");
        fprintf(stdout, "\tadd rax, %lld\n", instr.left.value);
        fprintf(stdout, "\tmov %s, rax\n", left);

        free(left);
    }else if (instr.type == PM_INSTR_NATIVE){
        char *name = (char*)&instr.dest.value;
        fprintf(stdout, "\tmov rdi, qword [rbp - 8]\n");
        fprintf(stdout, "\tmov rsi, qword [rbp - 16]\n");
        fprintf(stdout, "\tmov rdx, qword [rbp - 24]\n");
        fprintf(stdout, "\tcall _%s\n", name);
    }else if (instr.type == PM_INSTR_HALT){
        fprintf(stdout, "\tjmp main_epilogue\n");
    }else if (instr.type == PM_INSTR_RET){
        fprintf(stdout, "\tret\n");
    }
};

int main(int argc, char **argv){
    char *input_file = NULL;
    for (int i=0; i<argc; i++){
        input_file = argv[i];
    };
    if (input_file == NULL){
        pmc_error("pmc: no input file provided\n");
    }
    Pm *pm = pm_read_from_file(input_file);
    if (pm == NULL){
        pmc_error("pmc: no input file provided\n");
    }

    fprintf(stdout, "section .text\nglobal _main\ndefault rel\nextern _putchar\nextern _printf\n");
    fprintf(stdout, "_main:\n");
    fprintf(stdout, ".prologue\n\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, %d\n", 64 /* 256 * 8 + 64 */);

    for (int i=0; i<pm->instrlen; i++){
        Pm_Instr instr = pm->instrs[i];
        pmc_translate_x86_instr(pm, instr, i);
    }


    // fprintf(stdout, "main_epilogue:\n\tmov rax, qword [rbp - 8]\n\tmov rsp, rbp\n\tpop rbp\n\tret\n");
    fprintf(stdout, "main_epilogue:\n\tmov rax, 0x2000001\n\tmov rdi, qword [rbp - 8]\n\tsyscall\n");


    fprintf(stdout, "section .data\n");
    pmc_translate_x86_pool(pm);
}
