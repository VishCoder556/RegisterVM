// Register-based Virtual Machine
// PM -- `Primitive Machine`

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {
    PM_OPERAND_REG,
    PM_OPERAND_IMM
}Pm_Operand_Type;

typedef struct {
    Pm_Operand_Type type;
    int64_t value;
}Pm_Operand;

typedef enum {
    PM_INSTR_MOV_IMM,
    PM_INSTR_MOV_REG,
    PM_INSTR_ADD,
    PM_INSTR_SUB,
    PM_INSTR_MUL,
    PM_INSTR_DIV,
    PM_INSTR_EQ,
    PM_INSTR_LT,
    PM_INSTR_LTE,
    PM_INSTR_GT,
    PM_INSTR_GTE,
    PM_INSTR_JMP,
    PM_INSTR_JMPIF,
    PM_INSTR_JMPIFN,
    PM_INSTR_CALL,
    PM_INSTR_SYSCALL,
    PM_INSTR_HALT,
    PM_INSTR_RET,
    PM_INSTR_STORE8,
    PM_INSTR_STORE16,
    PM_INSTR_STORE32,
    PM_INSTR_STORE64,
    PM_INSTR_LOAD8,
    PM_INSTR_LOAD16,
    PM_INSTR_LOAD32,
    PM_INSTR_LOAD64,
    PM_INSTR_LOAD_STR,
    PM_INSTR_NATIVE
}Pm_Instr_Type;

typedef struct {
    Pm_Instr_Type type;
    Pm_Operand dest;
    Pm_Operand left;
    Pm_Operand right;
}Pm_Instr;
typedef struct {
    int64_t value;
}Pm_Register;
struct Pm;
typedef struct {
    char *name;
    void (*callback)(struct Pm *pm);
}Pm_Native;
typedef struct {
    Pm_Instr *instrs;
    long instrlen;
    long instrcap;


    Pm_Native *natives;
    int nativelen;
    int nativecap;

    char *pool;
    int poolcap;
    int poollen;


    Pm_Register registers[256];
}Pm;
Pm *pm_init();
void pm_add_instruction(Pm *pm, Pm_Instr instr);
void pm_write_to_file(Pm *pm, char *file);
Pm *pm_read_from_file(char *file);
void pm_add_native(Pm *pm, char *name, void (*callback)(struct Pm *pm));


Pm_Operand pm_operand_reg(uint64_t reg);
Pm_Operand pm_operand_imm(int64_t imm);

Pm_Instr pm_instr_mov_imm(uint64_t reg, int64_t val);
Pm_Instr pm_instr_mov_reg(uint64_t reg, uint64_t reg1);
Pm_Instr pm_instr_print(uint64_t reg);
Pm_Instr pm_instr_store8(uint64_t ptr, int64_t offset, Pm_Operand value);
Pm_Instr pm_instr_store16(uint64_t ptr, int64_t offset, Pm_Operand value);
Pm_Instr pm_instr_store32(uint64_t ptr, int64_t offset, Pm_Operand value);
Pm_Instr pm_instr_store64(uint64_t ptr, int64_t offset, Pm_Operand value);
Pm_Instr pm_instr_load8(uint64_t reg, uint64_t ptr, Pm_Operand offset);
Pm_Instr pm_instr_load16(uint64_t reg, uint64_t ptr, Pm_Operand offset);
Pm_Instr pm_instr_load32(uint64_t reg, uint64_t ptr, Pm_Operand offset);
Pm_Instr pm_instr_load64(uint64_t reg, uint64_t ptr, Pm_Operand offset);
Pm_Instr pm_instr_load_str(uint64_t reg, uint64_t addr);
Pm_Instr pm_instr_halt();
Pm_Instr pm_instr_ret();
Pm_Instr pm_instr_add(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_sub(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_mul(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_div(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_eq(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_lt(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_lte(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_gt(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_gte(int64_t reg, Pm_Operand left, Pm_Operand right);
Pm_Instr pm_instr_jmp(uint64_t location);
Pm_Instr pm_instr_call(uint64_t location);
Pm_Instr pm_instr_syscall(uint64_t num);
Pm_Instr pm_instr_native(char *name);
Pm_Instr pm_instr_jmpif(Pm_Operand operand, uint64_t location);
Pm_Instr pm_instr_jmpifn(Pm_Operand operand, uint64_t location);


#define PM_BIND_NATIVE(func, ...) \
void func(struct Pm *_pm) { \
    Pm *pm = (Pm*)_pm; \
    __VA_ARGS__; \
}

#define PM_BIND_NATIVE1_VOID(func, native, type1) PM_BIND_NATIVE(func, \
    type1 arg1 = (type1)pm->registers[0].value; \
    native(arg1); \
)
#define PM_BIND_NATIVE1(func, native, type1) PM_BIND_NATIVE(func, \
    type1 arg1 = (type1)pm->registers[0].value; \
    pm->registers[0].value = (uint64_t)native(arg1); \
)
#define PM_BIND_NATIVE2_VOID(func, native, type1, type2) PM_BIND_NATIVE(func, \
    type1 arg1 = (type1)pm->registers[0].value; \
    type2 arg2 = (type2)pm->registers[1].value; \
    native(arg1, arg2); \
)
#define PM_BIND_NATIVE2(func, native, type1, type2) PM_BIND_NATIVE(func, \
    type1 arg1 = (type1)pm->registers[0].value; \
    type2 arg2 = (type2)pm->registers[1].value; \
    pm->registers[0].value = (uint64_t)native(arg1, arg2); \
)

#define PM_NATIVE1_VOID(name, typ1) \
void wrapper_##name(typ1); \
PM_BIND_NATIVE1_VOID(name, wrapper_##name, typ1) \
void wrapper_##name

#define PM_NATIVE1(ret, name, typ1) \
ret wrapper_##name(typ1); \
PM_BIND_NATIVE1(name, wrapper_##name, typ1) \
ret wrapper_##name

#define PM_NATIVE0(ret, name) \
ret wrapper_##name(); \
PM_BIND_NATIVE(name, \
    pm->registers[0].value = (uint64_t)wrapper_##name() \
) \
ret wrapper_##name

#define PM_NATIVE0_VOID(ret, name) \
void wrapper_##name(); \
PM_BIND_NATIVE(name, wrapper_##name()) \
void wrapper_##name


#ifdef PM_IMPLEMENTATION
Pm *pm_init(){
    Pm *pm = malloc(sizeof(*pm));
    pm->instrcap = 10;
    pm->instrlen = 0;
    pm->instrs = malloc(sizeof(Pm_Instr) * pm->instrcap);

    pm->nativecap = 10;
    pm->nativelen = 0;
    pm->natives = malloc(sizeof(Pm_Native) * pm->nativecap);


    pm->poolcap = 10;
    pm->pool = malloc(pm->poolcap);
    pm->poollen = 0;


    pm->registers[255].value = 0;
    int stack_size = 2000;
    pm->registers[254].value = (uint64_t)malloc(stack_size) + stack_size;
    return pm;
}
void pm_add_to_pool(Pm *pm, char *str){
    int strl = strlen(str);
    if (pm->poollen + strl > pm->poolcap){
        pm->poolcap += strl + 5;
        pm->pool = realloc(pm->pool, pm->poolcap);
    };
    strncpy(pm->pool + pm->poollen, str, strl);
    pm->poollen += strl+1;
}

void pm_add_instruction(Pm *pm, Pm_Instr instr){
    if (pm->instrlen >= pm->instrcap){
        pm->instrcap += 5;
        pm->instrs = realloc(pm->instrs, sizeof(Pm_Instr) * pm->instrcap);
    };
    pm->instrs[pm->instrlen++] = instr;
}
Pm_Register *pm_get_register(Pm *pm, uint8_t reg){
    if (reg > 255){
        fprintf(stderr, "pm_get_register: could not access register `r%hhd`, max is `r255`\n", reg);
        exit(-1);
    }
    return pm->registers + reg;
};
int64_t pm_operand_get_value(Pm *pm, Pm_Operand operand){
    if (operand.type == PM_OPERAND_IMM){
        return operand.value;
    }else if (operand.type == PM_OPERAND_REG){
        return pm_get_register(pm, operand.value & 0xFF)->value;
    };
    return -1;
}
void pm_operand_set_value(Pm *pm, uint64_t r, int64_t val){
    uint8_t reg = r & 0xFF;
    uint8_t size = (r >> 8) & 0xFF;
    Pm_Register *_reg = pm_get_register(pm, reg);
    memcpy(&_reg->value, &val, size);
}

#define pm_execute_instr_binary(op) \
if (instr.dest.type == PM_OPERAND_REG){ \
    pm_operand_set_value(pm, instr.dest.value, pm_operand_get_value(pm, instr.left) op pm_operand_get_value(pm, instr.right)); \
}

char pm_execute_instruction(Pm *pm){
    uint64_t ip = pm->registers[255].value;
    if (ip > pm->instrlen) return -1;

    Pm_Instr instr = pm->instrs[ip];

    if (instr.type == PM_INSTR_MOV_IMM){
        pm_operand_set_value(pm, instr.dest.value, instr.left.value);
    }else if (instr.type == PM_INSTR_MOV_REG){
        pm_operand_set_value(pm, instr.dest.value, pm_operand_get_value(pm, instr.left));
    }else if (instr.type == PM_INSTR_HALT){
        return -1;
    }else if (instr.type == PM_INSTR_ADD){
        pm_execute_instr_binary(+);
    }else if (instr.type == PM_INSTR_SUB){
        pm_execute_instr_binary(-);
    }else if (instr.type == PM_INSTR_MUL){
        pm_execute_instr_binary(*);
    }else if (instr.type == PM_INSTR_DIV){
        pm_execute_instr_binary(/);
    }else if (instr.type == PM_INSTR_EQ){
        pm_execute_instr_binary(==);
    }else if (instr.type == PM_INSTR_LT){
        pm_execute_instr_binary(<);
    }else if (instr.type == PM_INSTR_LTE){
        pm_execute_instr_binary(<=);
    }else if (instr.type == PM_INSTR_GT){
        pm_execute_instr_binary(>);
    }else if (instr.type == PM_INSTR_GTE){
        pm_execute_instr_binary(<=);
    }else if (instr.type == PM_INSTR_STORE64){
        uint64_t *ptr1 = (uint64_t*)pm_operand_get_value(pm, instr.dest);
        ptr1 += pm_operand_get_value(pm, instr.left);
        *ptr1 = pm_operand_get_value(pm, instr.right);
    }else if (instr.type == PM_INSTR_STORE32){
        uint32_t *ptr1 = (uint32_t*)pm_operand_get_value(pm, instr.dest);
        ptr1 += pm_operand_get_value(pm, instr.left);
        *ptr1 = pm_operand_get_value(pm, instr.right);
    }else if (instr.type == PM_INSTR_STORE16){
        uint16_t *ptr1 = (uint16_t*)pm_operand_get_value(pm, instr.dest);
        ptr1 += pm_operand_get_value(pm, instr.left);
        *ptr1 = pm_operand_get_value(pm, instr.right);
    }else if (instr.type == PM_INSTR_STORE8){
        uint8_t *ptr1 = (uint8_t*)pm_operand_get_value(pm, instr.dest);
        ptr1 += pm_operand_get_value(pm, instr.left);
        *ptr1 = pm_operand_get_value(pm, instr.right);
    }else if (instr.type == PM_INSTR_LOAD8 || instr.type == PM_INSTR_LOAD16 || instr.type == PM_INSTR_LOAD32 || instr.type == PM_INSTR_LOAD64){
        uint8_t *ptr1 = (uint8_t*)pm_operand_get_value(pm, instr.left);
        ptr1 += pm_operand_get_value(pm, instr.right);
        pm_operand_set_value(pm, instr.dest.value, *ptr1);
    }else if (instr.type == PM_INSTR_LOAD_STR){
        if (instr.left.value > pm->poollen){
            fprintf(stderr, "pm load str: attempting to access out-of-bounds string\n");
            exit(-1);
        }
        pm_operand_set_value(pm, instr.dest.value, (uint64_t)pm->pool + instr.left.value);
    }else if (instr.type == PM_INSTR_JMP){
        pm->registers[255].value = pm_operand_get_value(pm, instr.dest);
        return 0;
    }else if (instr.type == PM_INSTR_JMPIF){
        if (pm_operand_get_value(pm, instr.dest)){
            pm->registers[255].value = pm_operand_get_value(pm, instr.left);
            return 0;
        }
    }else if (instr.type == PM_INSTR_JMPIFN){
        if (!pm_operand_get_value(pm, instr.dest)){
            pm->registers[255].value = pm_operand_get_value(pm, instr.left);
            return 0;
        }
    }else if (instr.type == PM_INSTR_CALL){
        pm->registers[253].value = pm->registers[255].value; // lr = ip
        pm->registers[255].value = pm_operand_get_value(pm, instr.dest);
        return 0;
    }else if (instr.type == PM_INSTR_SYSCALL){
        int num = instr.dest.value;
        int64_t reg1 = pm->registers[0].value;
        int64_t reg2 = pm->registers[1].value;
        int64_t reg3 = pm->registers[2].value;
        switch (num){
            case 0: {
                break;
            }
            default: {
                fprintf(stderr, "pm syscall: could not find syscall `%d`\n", num);
                exit(-1);
            }
        }
        printf("syscall here\n");
    }else if (instr.type == PM_INSTR_RET){
        pm->registers[255].value = pm->registers[253].value + 1; // ip = lr
        return 0;
    }else if (instr.type == PM_INSTR_NATIVE){
        char *name = (char*)&instr.dest.value;
        // free((char*)instr.dest.value);
        // Pm_Native native = pm->natives[idx];
        int idx = -1;
        for (int i=0; i<pm->nativelen; i++){
            if (pm->natives[i].name != NULL){
                if (strcmp(pm->natives[i].name, name) == 0){
                    idx = i;
                    break;
                }
            }
        }
        if (idx == -1){
            fprintf(stderr, "pm native: could not find native `%s`\n", name);
            exit(-1);
        }
        // callback((struct Pm*)pm);
        pm->natives[idx].callback((struct Pm*)pm);
    }else {
        fprintf(stderr, "pm_execute_instruction: unreachable code\n");
        exit(-1);
    };


    pm->registers[255].value++;
    return 0;
}

void pm_add_native(Pm *pm, char *name, void (*callback)(struct Pm *pm)){
    if (pm->nativelen >= pm->nativecap){
        pm->nativecap += 5;
        pm->natives = realloc(pm->natives, sizeof(Pm_Native)*pm->nativecap);
    }
    pm->natives[pm->nativelen].name = name;
    pm->natives[pm->nativelen++].callback = callback;
}

void pm_write_to_file(Pm *pm, char *file){
    FILE *f = fopen(file, "wb");
    if (f == NULL){
        fprintf(stderr, "pm_write_to_file: could not open file `%s`\n", file);
        exit(-1);
    }
    uint32_t magic = 0x9BCD;
    fwrite(&magic, 4, 1, f);

    fwrite(&pm->instrlen, 8, 1, f);
    fwrite(pm->instrs, sizeof(Pm_Instr), pm->instrlen, f);
    fwrite(&pm->poollen, 8, 1, f);
    fwrite(pm->pool, 1, pm->poollen, f);
    if (ferror(f)){
        fprintf(stderr, "pm_write_to_file: could not write to file `%s`\n", file);
        exit(-1);
    };
    fclose(f);
};
Pm *pm_read_from_file(char *file){
    FILE *f = fopen(file, "rb");
    if (f == NULL){
        fprintf(stderr, "pm_read_from_file: could not open file `%s`\n", file);
        exit(-1);
    }
    Pm *pm = pm_init();
    free(pm->instrs);

    uint32_t magic;
    fread(&magic, 4, 1, f);
    if (magic != 0x9BCD){
        fprintf(stderr, "pm_read_from_file: `%s` is not a valid binary format\n", file);
        exit(-1);
    }


    fread(&pm->instrcap, 8, 1, f);
    pm->instrs = malloc(sizeof(Pm_Instr) * pm->instrcap);
    pm->instrlen = pm->instrcap;
    fread(pm->instrs, sizeof(Pm_Instr), pm->instrlen, f);
    pm->registers[255].value = 0;
    
    fread(&pm->poolcap, 8, 1, f);
    pm->poollen = pm->poolcap;
    fread(pm->pool, 1, pm->poollen, f);

    fclose(f);
    return pm;
}


#define pm_instr_define_trinary(name, typ) \
Pm_Instr name(int64_t reg, Pm_Operand left, Pm_Operand right){ \
    return (Pm_Instr){.type=typ, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=left, .right=right}; \
};


Pm_Operand pm_operand_reg(uint64_t reg){
    return (Pm_Operand){.type=PM_OPERAND_REG, .value=reg};
}
Pm_Operand pm_operand_imm(int64_t imm){
    return (Pm_Operand){.type=PM_OPERAND_IMM, .value=imm};
}
Pm_Instr pm_instr_mov_imm(uint64_t reg, int64_t val){
    return (Pm_Instr){.type=PM_INSTR_MOV_IMM, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=val}};
};
Pm_Instr pm_instr_mov_reg(uint64_t reg, uint64_t reg1){
    return (Pm_Instr){.type=PM_INSTR_MOV_REG, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg1}};
};
pm_instr_define_trinary(pm_instr_add, PM_INSTR_ADD)
pm_instr_define_trinary(pm_instr_sub, PM_INSTR_SUB)
pm_instr_define_trinary(pm_instr_mul, PM_INSTR_MUL)
pm_instr_define_trinary(pm_instr_div, PM_INSTR_DIV)
pm_instr_define_trinary(pm_instr_eq, PM_INSTR_EQ)
pm_instr_define_trinary(pm_instr_lt, PM_INSTR_LT)
pm_instr_define_trinary(pm_instr_lte, PM_INSTR_LTE)
pm_instr_define_trinary(pm_instr_gt, PM_INSTR_GT)
pm_instr_define_trinary(pm_instr_gte, PM_INSTR_GTE)
Pm_Instr pm_instr_jmp(uint64_t location){
    return (Pm_Instr){.type=PM_INSTR_JMP, .dest=(Pm_Operand){.type=PM_OPERAND_IMM, .value=location}};
}
Pm_Instr pm_instr_call(uint64_t location){
    return (Pm_Instr){.type=PM_INSTR_CALL, .dest=(Pm_Operand){.type=PM_OPERAND_IMM, .value=location}};
}
Pm_Instr pm_instr_syscall(uint64_t num){
    return (Pm_Instr){.type=PM_INSTR_SYSCALL, .dest=(Pm_Operand){.type=PM_OPERAND_IMM, .value=num}};
}
Pm_Instr pm_instr_native(char *name){
    Pm_Instr instr = (Pm_Instr){.type=PM_INSTR_NATIVE};
    if (strlen(name) > 24){
        fprintf(stderr, "pm_instr_native: can't support strings over 24 bytes\n");
        exit(-1);
    }
    uint64_t *data = (uint64_t*)(name);
    instr.dest=(Pm_Operand){.type=PM_OPERAND_IMM, .value=*data};
    instr.left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=*(data+8)};
    instr.right=(Pm_Operand){.type=PM_OPERAND_IMM, .value=*(data+14)};
    return instr;
}
Pm_Instr pm_instr_store8(uint64_t ptr, int64_t offset, Pm_Operand value){
    return (Pm_Instr){.type=PM_INSTR_STORE8, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=offset}, .right=value};
}
Pm_Instr pm_instr_store16(uint64_t ptr, int64_t offset, Pm_Operand value){
    return (Pm_Instr){.type=PM_INSTR_STORE16, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=offset}, .right=value};
}
Pm_Instr pm_instr_store32(uint64_t ptr, int64_t offset, Pm_Operand value){
    return (Pm_Instr){.type=PM_INSTR_STORE32, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=offset}, .right=value};
}
Pm_Instr pm_instr_store64(uint64_t ptr, int64_t offset, Pm_Operand value){
    return (Pm_Instr){.type=PM_INSTR_STORE64, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=offset}, .right=value};
}



Pm_Instr pm_instr_load8(uint64_t reg, uint64_t ptr, Pm_Operand offset){
    return (Pm_Instr){.type=PM_INSTR_LOAD8, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .right=offset};
};
Pm_Instr pm_instr_load16(uint64_t reg, uint64_t ptr, Pm_Operand offset){
    return (Pm_Instr){.type=PM_INSTR_LOAD16, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .right=offset};
};
Pm_Instr pm_instr_load32(uint64_t reg, uint64_t ptr, Pm_Operand offset){
    return (Pm_Instr){.type=PM_INSTR_LOAD32, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .right=offset};
};
Pm_Instr pm_instr_load64(uint64_t reg, uint64_t ptr, Pm_Operand offset){
    return (Pm_Instr){.type=PM_INSTR_LOAD64, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=(Pm_Operand){.type=PM_OPERAND_REG, .value=ptr}, .right=offset};
};



Pm_Instr pm_instr_load_str(uint64_t reg, uint64_t addr){
    return (Pm_Instr){.type=PM_INSTR_LOAD_STR, .dest=(Pm_Operand){.type=PM_OPERAND_REG, .value=reg}, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=addr}};
};
Pm_Instr pm_instr_jmpif(Pm_Operand operand, uint64_t location){
    return (Pm_Instr){.type=PM_INSTR_JMPIF, .dest=operand, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=location}};
}
Pm_Instr pm_instr_jmpifn(Pm_Operand operand, uint64_t location){
    return (Pm_Instr){.type=PM_INSTR_JMPIFN, .dest=operand, .left=(Pm_Operand){.type=PM_OPERAND_IMM, .value=location}};
}
Pm_Instr pm_instr_halt(){
    return (Pm_Instr){.type=PM_INSTR_HALT};
};
Pm_Instr pm_instr_ret(){
    return (Pm_Instr){.type=PM_INSTR_RET};
};

#endif
