/*******************************************************
* lexer.h -            header file for lexer.c
* Language:            Micro
*
********************************************************/

#ifndef LEXER_H_
#define LEXER_H_

#include "compiler.h"

// int literals and identifiers need not only a token to say what they are,
// but also a buffer to store their value/representation
char identifierStr[MAX_ID_LEN + 1]; // string value of identifier
long intVal;   // value of number, if found
double fltVal;

typedef enum token_types{
    tok_EOF = -1, tok_BEGIN=-2 , tok_END = -3, tok_READ = -4, tok_WRITE = -5, 
    tok_ID = -6, tok_INT_LITERAL = -7, tok_FLT_LITERAL= -8, tok_ASSIGN = -9, 
    tok_DEC_INT = -10, tok_DEC_LONG = -11, tok_DEC_FLT = -12,
    tok_OP_PLUS = '+', tok_OP_MINUS = '-', tok_OP_MUL = '*', tok_OP_DIV = '/',
    tok_LPAREN = '(', tok_RPAREN = ')', tok_COMMA = ',', tok_SEMICOLON = ';',
} token;

extern int tokenize(int);

#endif
