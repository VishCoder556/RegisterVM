#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include "linkedlist.h"


typedef struct {
    char *name;
    int ip;

    struct Pasm_Label *next;
}Pasm_Label;

char *Pasm_Syscalls[] = {
    "write"
};
#define ARR_SIZE(arr) sizeof(arr) / sizeof(arr[0])

typedef struct {
    Pasm_Token *tokens;
    int tokencap;
    int tokenlen;

    int cur;
    int ip;

    Pm *pm;

    LinkedList(Pasm_Label);
}Pasm_Parser;

Pasm_Parser *pasm_parser_init(Pasm_Tokenizer *tokenizer){
    Pasm_Parser *parser = malloc(sizeof(Pasm_Parser));
    parser->cur = 0;
    parser->ip = 0;
    parser->tokens = tokenizer->tokens;
    parser->tokencap = tokenizer->tokencap;
    parser->tokenlen = tokenizer->tokenlen;

    parser->pm = pm_init();

    InitLinkedList((*parser), Pasm_Label);
    return parser;
}

char pasm_parser_peek(Pasm_Parser *parser){
    if (parser->cur >= parser->tokenlen) return -1;
    parser->cur++;
    return 0;
}

#define pasm_parser_error(parser, ...) { \
    char error_fmt[500]; \
    snprintf(error_fmt, 500, __VA_ARGS__); \
    fprintf(stderr, "PasParserError: %s\n", error_fmt); \
    exit(-1); \
}


char pasm_parser_operand(Pasm_Parser *parser, Pm_Operand *operand){
    Pasm_Token cur_token = parser->tokens[parser->cur];
    if (cur_token.type == PASM_TOKEN_ID){
        uint8_t reg;
        uint8_t size = 8;
        if (strcmp(cur_token.value, "lr") == 0){
            reg = 253;
        }else if (strcmp(cur_token.value, "sp") == 0){
            reg = 254;
        }else if (strcmp(cur_token.value, "pc") == 0){
            reg = 255;
        }else if (cur_token.value[0] == 'r'){
            reg = atoi(cur_token.value+1);
            if (reg > 252 || reg < 0){
                fprintf(stderr, "pasm_parser_operand: got invalid register `%s`; max is `r252` and min is `r0`", cur_token.value);
                exit(-1);
            };
        };
        pasm_parser_peek(parser);
        cur_token = parser->tokens[parser->cur];

        if (cur_token.type == PASM_TOKEN_COLON){
            pasm_parser_peek(parser);
            cur_token = parser->tokens[parser->cur];
            if (cur_token.type == PASM_TOKEN_INT){
                size = atoi(cur_token.value);
                if (size == 1 || size == 2 || size == 4 || size == 8){
                }else{
                    pasm_parser_error(parser, "Unexpected size `%d` found", size);
                }
            }else{
                pasm_parser_error(parser, "Expected size to be integer");
            }
            pasm_parser_peek(parser);
        }

        *operand = (Pm_Operand){.type=PM_OPERAND_REG, .value=(reg) | (size << 8)};
        return 0;
    }else if(cur_token.type == PASM_TOKEN_INT){
        *operand = (Pm_Operand){.type=PM_OPERAND_IMM, .value=atoi(cur_token.value)};
        pasm_parser_peek(parser);
        return 0;
    }
    return -1;
}

char pasm_parser_expect(Pasm_Parser *parser, Pasm_TokenType type){
    Pasm_Token cur_token = parser->tokens[parser->cur];
    if (cur_token.type != type){
        pasm_parser_error(parser, "Expected type `%d`, got type `%d`", type, cur_token.type);
        return -1;
    }
    return pasm_parser_peek(parser);

};

Pm_Operand pasm_parser_parse_operand(Pasm_Parser *parser, char *where, char *stmnt){
    Pm_Operand dest;
    if (pasm_parser_operand(parser, &dest) == -1){
        pasm_parser_error(parser, "Could not parse %s operand of `%s` statement", where, stmnt);
    }
    return dest;
}
Pm_Operand pasm_parser_reg_operand(Pasm_Parser *parser, char *where, char *stmnt){
    Pm_Operand op = pasm_parser_parse_operand(parser, where, stmnt);
    if (op.type != PM_OPERAND_REG){
        pasm_parser_error(parser, "Expected %s of `%s` statement to be a register", where, stmnt);
    }
    return op;
}
char pasm_parser_pre_parse(Pasm_Parser *parser){
    if (parser->cur >= parser->tokenlen) {
        parser->cur = 0;
        parser->ip = 0;
        return -1;
    }
    Pasm_Token cur_token = parser->tokens[parser->cur];
    if (cur_token.type == PASM_TOKEN_ID){
        if (strcmp(cur_token.value, "mov") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "add") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "sub") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "mul") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "div") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "sete") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "setlt") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "setlte") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "setgt") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "setgte") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "halt") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "ret") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "jmp") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "jmpif") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "jmpifn") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "store") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "load") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "load_str") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "native") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "push") == 0){
            parser->ip+=2;
        }else if (strcmp(cur_token.value, "pop") == 0){
            parser->ip+=2;
        }else if (strcmp(cur_token.value, "call") == 0){
            parser->ip++;
        }else if (strcmp(cur_token.value, "syscall") == 0){
            parser->ip++;
        }else {
            // Expect to be label
            pasm_parser_peek(parser);
            if (parser->tokens[parser->cur].type == PASM_TOKEN_COLON){
                AppendToLinkedList((*parser), Pasm_Label, (Pasm_Label){.name=cur_token.value, .ip=parser->ip});
            }else {
                return 0;
            }
        }
    }
    return pasm_parser_peek(parser);
    return 0;
}

Pasm_Label *pasm_parser_find_label(Pasm_Parser *parser, char *name){
    Pasm_Label *head = GetLinkedListHead((*parser), Pasm_Label);
    while (head != NULL){
        if (strcmp(head->name, name) == 0){
            return head;
        }
        head = (Pasm_Label*)head->next;
    };
    return NULL;
}

#define pasm_parse_binary(op) \
pasm_parser_peek(parser); \
Pm_Operand dest = pasm_parser_reg_operand(parser, "destination", cur_token.value); \
pasm_parser_expect(parser, PASM_TOKEN_COMMA); \
Pm_Operand left = pasm_parser_parse_operand(parser, "left", cur_token.value); \
pasm_parser_expect(parser, PASM_TOKEN_COMMA); \
Pm_Operand right = pasm_parser_parse_operand(parser, "right", cur_token.value); \
pm_add_instruction(parser->pm, op(dest.value, left, right)); \
parser->ip++; \
return 0;

char pasm_parser_parse(Pasm_Parser *parser){
    if (parser->cur >= parser->tokenlen) return -1;
    Pasm_Token cur_token = parser->tokens[parser->cur];
    if (cur_token.type == PASM_TOKEN_ID){
        if (strcmp(cur_token.value, "mov") == 0){
            pasm_parser_peek(parser);
            Pm_Operand dest = pasm_parser_reg_operand(parser, "destination", cur_token.value);
            pasm_parser_expect(parser, PASM_TOKEN_COMMA);
            Pm_Operand left = pasm_parser_parse_operand(parser, "source", cur_token.value);
            if (left.type == PM_OPERAND_REG){
                pm_add_instruction(parser->pm, pm_instr_mov_reg(dest.value, left.value));
            }else if (left.type == PM_OPERAND_IMM){
                pm_add_instruction(parser->pm, pm_instr_mov_imm(dest.value, left.value));
            }
            parser->ip++;
            return 0;
        }else if (strcmp(cur_token.value, "add") == 0){
            pasm_parse_binary(pm_instr_add);
        }else if (strcmp(cur_token.value, "sub") == 0){
            pasm_parse_binary(pm_instr_sub);
        }else if (strcmp(cur_token.value, "mul") == 0){
            pasm_parse_binary(pm_instr_mul);
        }else if (strcmp(cur_token.value, "div") == 0){
            pasm_parse_binary(pm_instr_div);
        }else if (strcmp(cur_token.value, "sete") == 0){
            pasm_parse_binary(pm_instr_eq);
        }else if (strcmp(cur_token.value, "setlt") == 0){
            pasm_parse_binary(pm_instr_lt);
        }else if (strcmp(cur_token.value, "setlte") == 0){
            pasm_parse_binary(pm_instr_lte);
        }else if (strcmp(cur_token.value, "setgt") == 0){
            pasm_parse_binary(pm_instr_gt);
        }else if (strcmp(cur_token.value, "setgte") == 0){
            pasm_parse_binary(pm_instr_gte);
        }else if (strcmp(cur_token.value, "jmp") == 0){
            pasm_parser_peek(parser);
            Pasm_Label *label = pasm_parser_find_label(parser, parser->tokens[parser->cur].value);
            if (label == NULL){
                pasm_parser_error(parser, "Could not find label `%s`", parser->tokens[parser->cur].value);
            }
            pm_add_instruction(parser->pm, pm_instr_jmp(label->ip));
            pasm_parser_peek(parser);
            parser->ip++;
            return 0;
        }else if (strcmp(cur_token.value, "jmpif") == 0){
            pasm_parser_peek(parser);

            Pm_Operand dest = pasm_parser_parse_operand(parser, "destination", cur_token.value);
            pasm_parser_expect(parser, PASM_TOKEN_COMMA);

            Pasm_Label *label = pasm_parser_find_label(parser, parser->tokens[parser->cur].value);
            if (label == NULL){
                pasm_parser_error(parser, "Could not find label `%s`", parser->tokens[parser->cur].value);
            }
            pm_add_instruction(parser->pm, pm_instr_jmpif(dest, label->ip));
            pasm_parser_peek(parser);
            parser->ip++;
            return 0;
        }else if (strcmp(cur_token.value, "jmpifn") == 0){
            pasm_parser_peek(parser);

            Pm_Operand dest = pasm_parser_parse_operand(parser, "destination", cur_token.value);
            pasm_parser_expect(parser, PASM_TOKEN_COMMA);

            Pasm_Label *label = pasm_parser_find_label(parser, parser->tokens[parser->cur].value);
            if (label == NULL){
                pasm_parser_error(parser, "Could not find label `%s`", parser->tokens[parser->cur].value);
            }
            pm_add_instruction(parser->pm, pm_instr_jmpifn(dest, label->ip));
            pasm_parser_peek(parser);
            parser->ip++;
            return 0;
        }else if (strcmp(cur_token.value, "call") == 0){
            pasm_parser_peek(parser);
            Pasm_Label *label = pasm_parser_find_label(parser, parser->tokens[parser->cur].value);
            if (label == NULL){
                pasm_parser_error(parser, "Could not find label `%s`", parser->tokens[parser->cur].value);
            }
            pm_add_instruction(parser->pm, pm_instr_call(label->ip));
            pasm_parser_peek(parser);
            parser->ip++;
            return 0;
        }else if (strcmp(cur_token.value, "syscall") == 0){
            pasm_parser_peek(parser);
            char *str = parser->tokens[parser->cur].value;
            int idx = -1;
            for (int i=0; i<ARR_SIZE(Pasm_Syscalls); i++){
                if (strcmp(Pasm_Syscalls[i], str) == 0){
                    idx = i;
                    break;
                };
            }
            if (idx == -1){
                pasm_parser_error(parser, "Could not find syscall `%s`", str);
            }
            pm_add_instruction(parser->pm, pm_instr_syscall(idx));
            parser->ip++;
            return pasm_parser_peek(parser);
        }else if (strcmp(cur_token.value, "halt") == 0){
            pm_add_instruction(parser->pm, pm_instr_halt());
            return pasm_parser_peek(parser);
        }else if (strcmp(cur_token.value, "load_str") == 0){
            pasm_parser_peek(parser);
            Pm_Operand dest = pasm_parser_reg_operand(parser, "destination", cur_token.value);
            pasm_parser_expect(parser, PASM_TOKEN_COMMA);
            pm_add_instruction(parser->pm, pm_instr_load_str(dest.value, parser->pm->poollen));
            pm_add_to_pool(parser->pm, parser->tokens[parser->cur].value);
            return pasm_parser_peek(parser);
        }else if (strcmp(cur_token.value, "store") == 0){
            pasm_parser_peek(parser);
            Pm_Operand dest = pasm_parser_reg_operand(parser, "destination", cur_token.value);
            pasm_parser_expect(parser, PASM_TOKEN_COMMA);
            Pm_Operand left = pasm_parser_parse_operand(parser, "left", cur_token.value);
            if (parser->tokens[parser->cur].type == PASM_TOKEN_COMMA){
                if (left.type != PM_OPERAND_IMM){
                    pasm_parser_error(parser, "Expected offset of `store` statement to be a immediate");
                }
                pasm_parser_peek(parser);
                Pm_Operand right = pasm_parser_parse_operand(parser, "righthand", cur_token.value);


                Pm_Instr instr = pm_instr_store64(dest.value, left.value, right);
                switch((dest.value >> 8) & 0xFF){
                    case 1: instr = pm_instr_store8(dest.value, left.value, right); break;
                    case 2: instr = pm_instr_store16(dest.value, left.value, right); break;
                    case 4: instr = pm_instr_store32(dest.value, left.value, right); break;
                    default: break;
                }
                pm_add_instruction(parser->pm, instr);
            }else {
                Pm_Instr instr = pm_instr_store64(dest.value, 0, left);
                switch((dest.value >> 8) & 0xFF){
                    case 1: instr = pm_instr_store8(dest.value, 0, left); break;
                    case 2: instr = pm_instr_store16(dest.value, 0, left); break;
                    case 4: instr = pm_instr_store32(dest.value, 0, left); break;
                    default: break;
                }
                pm_add_instruction(parser->pm, instr);
            }
            parser->ip++;
            return 0;
        }else if (strcmp(cur_token.value, "load") == 0){
            pasm_parser_peek(parser);
            Pm_Operand dest = pasm_parser_reg_operand(parser, "destination", cur_token.value);
            pasm_parser_expect(parser, PASM_TOKEN_COMMA);
            Pm_Operand left = pasm_parser_reg_operand(parser, "left", cur_token.value);
            if (parser->tokens[parser->cur].type == PASM_TOKEN_COMMA){
                pasm_parser_peek(parser);
                Pm_Operand right = pasm_parser_parse_operand(parser, "offset", cur_token.value);
                Pm_Instr instr = pm_instr_load64(dest.value, left.value, right);
                switch((dest.value >> 8) & 0xFF){
                    case 1: instr = pm_instr_load8(dest.value, left.value, right); break;
                    case 2: instr = pm_instr_load16(dest.value, left.value, right); break;
                    case 4: instr = pm_instr_load32(dest.value, left.value, right); break;
                    default: break;
                }
                pm_add_instruction(parser->pm, instr);
            }else {
                Pm_Instr instr = pm_instr_load64(dest.value, left.value, (Pm_Operand){.type=PM_OPERAND_IMM, .value=0});
                // printf("%lld, %lld\n", (dest.value << 1) & 0xFF, dest.value);
                switch((dest.value >> 8) & 0xFF){
                    case 1: instr = pm_instr_load8(dest.value, left.value, (Pm_Operand){.type=PM_OPERAND_IMM, .value=0}); break;
                    case 2: instr = pm_instr_load16(dest.value, left.value, (Pm_Operand){.type=PM_OPERAND_IMM, .value=0}); break;
                    case 4: instr = pm_instr_load32(dest.value, left.value, (Pm_Operand){.type=PM_OPERAND_IMM, .value=0}); break;
                    default: break;
                }
                pm_add_instruction(parser->pm, instr);
            }
            parser->ip++;
            return 0;
        }else if (strcmp(cur_token.value, "ret") == 0){
            pm_add_instruction(parser->pm, pm_instr_ret());
            return pasm_parser_peek(parser);
        }else if (strcmp(cur_token.value, "push") == 0){
            pasm_parser_peek(parser);
            Pm_Operand dest = pasm_parser_parse_operand(parser, "destination", cur_token.value);
            pm_add_instruction(parser->pm, pm_instr_store64(254, -8, dest));
            pm_add_instruction(parser->pm, pm_instr_sub(254, pm_operand_reg(254), pm_operand_imm(8)));
            parser->ip+=2;
            return 0;
        }else if (strcmp(cur_token.value, "pop") == 0){
            pasm_parser_peek(parser);
            Pm_Operand dest = pasm_parser_reg_operand(parser, "destination", cur_token.value);
            pm_add_instruction(parser->pm, pm_instr_load64(dest.value, 254, (Pm_Operand){.type=PM_OPERAND_IMM, .value=0}));
            pm_add_instruction(parser->pm, pm_instr_add(254, pm_operand_reg(254), pm_operand_imm(8)));

            parser->ip+=2;
            return 0;
        }else if (strcmp(cur_token.value, "native") == 0){
            pasm_parser_peek(parser);


            pm_add_instruction(parser->pm, pm_instr_native(parser->tokens[parser->cur].value));
            pasm_parser_peek(parser);


            parser->ip++;
            return 0;
        }else {
            pasm_parser_peek(parser);
            if (parser->tokens[parser->cur].type == PASM_TOKEN_COLON){
                pasm_parser_expect(parser, PASM_TOKEN_COLON);
            }else {
                pasm_parser_error(parser, "Unknown identifier `%s` encountered", cur_token.value);
            }
            return 0;
        }
    }else {
        pasm_parser_error(parser, "Unknown token `%d` encountered", cur_token.type);
    }
    fprintf(stderr, "pas: unreachable code\n");
    exit(-1);
    return -1;
}
