#include <stdio.h>
#include <stdlib.h>
#define PM_IMPLEMENTATION
#include "pm.c"

PM_NATIVE1_VOID(pm_puts, char*)(char *str){
    printf("%s\n", str);
}

PM_NATIVE1_VOID(pm_putc, char)(char ch){
    printf("%c\n", ch);
}

PM_NATIVE1_VOID(pm_puti, int)(int num){
    printf("%d\n", num);
}

PM_NATIVE0(int, pm_geti)(){
    // printf("%d\n", num);
    char *str = malloc(1);
    return getc(stdin);
};

void pme_natives(Pm *pm){
    pm_add_native(pm, "puts", pm_puts);
    pm_add_native(pm, "putc", pm_putc);
    pm_add_native(pm, "puti", pm_puti);
    pm_add_native(pm, "geti", pm_geti);
}

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
    pme_natives(pm);

    while (pm_execute_instruction(pm) == 0){
    }
}
