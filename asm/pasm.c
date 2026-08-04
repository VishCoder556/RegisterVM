#include <stdio.h>
#include <stdlib.h>

#define PM_IMPLEMENTATION
#include "../pm.c"

#include "tokenizer.c"
#include "parser.c"


// Primitive Assembler

/* Register Convention
 *
 * r0-r10 = Arguments (r0 is also return value)
 * r11-r131 = Caller-saved registers (temporary registers)
 * r132-r253 = Callee-saved registers
 * r253 = lr (return address for functions)
 * r254 = sp (pointer to top of stack)
 * r252 = pc (current instruction number)
 *
 *
*/

char *get_buffer_from_file(char *input_file){
    FILE *file = fopen(input_file, "r");
    if (file == NULL){
        fprintf(stderr, "ERROR: Input File '%s' does not exist\n", input_file);
        exit(-1);
    };

    fseek(file, 0, SEEK_END);
    long count = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(count + 1);
    fread(buffer, 1, count, file);
    buffer[count] = '\0';
    fclose(file);
    return buffer;
}


int main(int argc, char **argv){
    char *input_file = NULL;
    char *output_file = NULL;
    int output_flag = 0;
    for (int i=0; i<argc; i++){
        if (strcmp(argv[i], "-o") == 0){
            output_flag = 1;
        }else if (output_flag == 1){
            output_file = argv[i];
        }else {
            input_file = argv[i];
        }
    };
    if (input_file == NULL){
        fprintf(stderr, "pasm: no input file provided\n");
        exit(-1);
    }
    if (output_file == NULL){
        output_file = "binary.pm";
    }

    char *buffer = get_buffer_from_file(input_file);



    Pasm_Tokenizer *tokenizer = pasm_tokenizer_init(input_file, buffer);
    while (pasm_tokenizer_token(tokenizer) == 0){
    };
    Pasm_Parser *parser = pasm_parser_init(tokenizer);
    while (pasm_parser_pre_parse(parser) == 0){
    };
    while (pasm_parser_parse(parser) == 0){
        ;
    };
    pm_write_to_file(parser->pm, output_file);
}
