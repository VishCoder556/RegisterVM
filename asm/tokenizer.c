#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PASM_TOKEN_ID,
    PASM_TOKEN_EQ,
    PASM_TOKEN_INT,
    PASM_TOKEN_FLOAT,
    PASM_TOKEN_LB,
    PASM_TOKEN_RB,
    PASM_TOKEN_LP,
    PASM_TOKEN_RP,
    PASM_TOKEN_STRING,
    PASM_TOKEN_COMMA,
    PASM_TOKEN_COLON,
    PASM_TOKEN_PERCENT,
    PASM_TOKEN_AMP,
    PASM_TOKEN_MAX, // Marker to know how many tokens we have
    // Keep adding on eventually
}Pasm_TokenType;

typedef struct {
    Pasm_TokenType type;
    char *value;
}Pasm_Token;

typedef struct {
    char *input_file;
    char *buffer;
    int bufferlen;


    Pasm_Token *tokens;
    int tokencap;
    int tokenlen;

    int cur;
    int col;
    int row;
}Pasm_Tokenizer;


Pasm_Tokenizer *pasm_tokenizer_init(char *input_file, char *buffer);
char pasm_tokenizer_token(Pasm_Tokenizer *tokenizer);


Pasm_Tokenizer *pasm_tokenizer_init(char *input_file, char *buffer){
    Pasm_Tokenizer *tokenizer = malloc(sizeof(Pasm_Tokenizer));
    tokenizer->input_file = input_file;
    tokenizer->buffer = buffer;
    tokenizer->bufferlen = strlen(buffer);
    tokenizer->cur = 0;
    tokenizer->col = 0;
    tokenizer->row = 0;
    tokenizer->tokencap = 10; // For now
    tokenizer->tokenlen = 0;
    tokenizer->tokens = malloc(sizeof(Pasm_Token) * tokenizer->tokencap);
    return tokenizer;
}

void pasm_tokenizer_append(Pasm_Tokenizer *tokenizer, Pasm_TokenType type, char *value){
    if (tokenizer->tokenlen >= tokenizer->tokencap){
        tokenizer->tokencap += 10;
        tokenizer->tokens = realloc(tokenizer->tokens, sizeof(Pasm_Token) * tokenizer->tokencap);
    }
    tokenizer->tokens[tokenizer->tokenlen++] = (Pasm_Token){type, value};
}

char pasm_tokenizer_peek(Pasm_Tokenizer *tokenizer){
    if (tokenizer->cur <= tokenizer->bufferlen){
        char c = tokenizer->buffer[tokenizer->cur];
        tokenizer->cur++;
        tokenizer->col++;
        return c;
    }
    return '\0';
}

char *char_to_string(char c){
    char *string = malloc(2);
    string[0] = c;
    string[1] = '\0';
    return string;
}

#define pasm_tokenizer_advance(tokenizer)  char_to_string(pasm_tokenizer_peek(tokenizer))
// Get the current character as a string and advance

char pasm_tokenizer_token(Pasm_Tokenizer *tokenizer){
    char c = tokenizer->buffer[tokenizer->cur];

    assert(PASM_TOKEN_MAX == 13 && "Exhaustive handling of tokens -- please implement token here");
    // Assertion style copied from Tsoding Daily in his programming language Porth (P.S. you should check it out);

    switch (c){
        case '/': {
            pasm_tokenizer_peek(tokenizer);
            char c = tokenizer->buffer[tokenizer->cur];
            if (c == '/'){
                while(c != '\n'){
                    pasm_tokenizer_peek(tokenizer);
                    c = tokenizer->buffer[tokenizer->cur];
                };
                break;
            };
        }
        case '&': pasm_tokenizer_append(tokenizer, PASM_TOKEN_AMP, pasm_tokenizer_advance(tokenizer)); break;
        case '%': pasm_tokenizer_append(tokenizer, PASM_TOKEN_PERCENT, pasm_tokenizer_advance(tokenizer)); break;
        case ':': pasm_tokenizer_append(tokenizer, PASM_TOKEN_COLON, pasm_tokenizer_advance(tokenizer)); break;
        case ',': pasm_tokenizer_append(tokenizer, PASM_TOKEN_COMMA, pasm_tokenizer_advance(tokenizer)); break;
        case '=': pasm_tokenizer_append(tokenizer, PASM_TOKEN_EQ, pasm_tokenizer_advance(tokenizer)); break;
        case '{': pasm_tokenizer_append(tokenizer, PASM_TOKEN_LB, pasm_tokenizer_advance(tokenizer)); break;
        case '}': pasm_tokenizer_append(tokenizer, PASM_TOKEN_RB, pasm_tokenizer_advance(tokenizer)); break;
        case '(': pasm_tokenizer_append(tokenizer, PASM_TOKEN_LP, pasm_tokenizer_advance(tokenizer)); break;
        case ')': pasm_tokenizer_append(tokenizer, PASM_TOKEN_RP, pasm_tokenizer_advance(tokenizer)); break;
        case '\n': tokenizer->row++; tokenizer->col = 0; tokenizer->cur++; break;
        case '\t': pasm_tokenizer_peek(tokenizer); break;
        case ' ': pasm_tokenizer_peek(tokenizer); break;
        case '\'':{
            pasm_tokenizer_peek(tokenizer);
            char a = tokenizer->buffer[tokenizer->cur];
            if (a == '\\'){
                pasm_tokenizer_peek(tokenizer);
                a = tokenizer->buffer[tokenizer->cur];
                switch (a){
                    case '0': a = '\0'; break;
                    case 'n': a = '\n'; break;
                    case 'r': a = '\r'; break;
                    case '\\': a = '\\'; break;
                    default: assert(0 && "Error: found unknown escape statement");
                }
            }
            pasm_tokenizer_peek(tokenizer);
            pasm_tokenizer_peek(tokenizer);
            

            char *buffer = malloc(10);
            sprintf(buffer, "%d", a);
            pasm_tokenizer_append(tokenizer, PASM_TOKEN_INT, buffer);
            break;
          }
        case '\"':
            pasm_tokenizer_peek(tokenizer);
            int valuecap = 100;
            char *value = malloc(valuecap);
            int len = 0;
            c = tokenizer->buffer[tokenizer->cur];
            while (c != '\"'){
                if (len >= valuecap){
                    valuecap += 5;
                    value = realloc(value, valuecap);
                }
                if (c == '\\'){
                    pasm_tokenizer_peek(tokenizer);
                    c = tokenizer->buffer[tokenizer->cur];
                    switch (c){
                        case '0': c = '\0'; break;
                        case 'n': c = '\n'; break;
                        case 'r': c = '\r'; break;
                        case '\\': c = '\\'; break;
                    }
                }
                value[len++] = c;
                pasm_tokenizer_peek(tokenizer);
                c = tokenizer->buffer[tokenizer->cur];
            }
            if (pasm_tokenizer_peek(tokenizer) != '\"'){
                assert(0 && "Unreachable code");
            }
            pasm_tokenizer_append(tokenizer, PASM_TOKEN_STRING, value);
            break;
        case '\0': return -1;
        default:
            if (isalpha(c) || c=='.'||c=='_'){
                int valuecap = 100;
                char *value = malloc(valuecap);
                int len = 0;
                // . and _ are allowed because they aren't used as tokens elsewhere
                while (isalpha(c) || c == '.' || c == '_' || isnumber(c)){
                    if (len >= valuecap){
                        valuecap += 5;
                        value = realloc(value, valuecap);
                    }
                    value[len++] = c;
                    pasm_tokenizer_peek(tokenizer);
                    c = tokenizer->buffer[tokenizer->cur];
                }
                pasm_tokenizer_append(tokenizer, PASM_TOKEN_ID, value);
                break;
            }else if (isnumber(c) || c == '-'){
                int valuecap = 100;
                char *value = malloc(valuecap);
                int len = 0;
                while (isnumber(c) || c == '-'){
                    if (len >= valuecap){
                        valuecap += 5;
                        value = realloc(value, valuecap);
                    }
                    value[len++] = c;
                    pasm_tokenizer_peek(tokenizer);
                    c = tokenizer->buffer[tokenizer->cur];
                }
                if (c == '.'){
                    pasm_tokenizer_peek(tokenizer);
                    value[len++] = '.';
                    c = tokenizer->buffer[tokenizer->cur];
                    while (isnumber(c)){
                        if (len >= valuecap){
                            valuecap += 5;
                            value = realloc(value, valuecap);
                        }
                        value[len++] = c;
                        pasm_tokenizer_peek(tokenizer);
                        c = tokenizer->buffer[tokenizer->cur];
                    }
                    pasm_tokenizer_append(tokenizer, PASM_TOKEN_FLOAT, value);
                    break;
                };
                pasm_tokenizer_append(tokenizer, PASM_TOKEN_INT, value);
                break;
            }
    }
skip_increment:
    return 0;
}
