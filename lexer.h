/*******************************************************
* lexer.h -            header file for lexer.c
* Language:            Micro
*
********************************************************/

#ifndef LEXER_H_
#define LEXER_H_

#ifndef COMPILER_H
#include "compiler.h"
#endif

// int literals and identifiers need not only a token to say what they are,
// but also a buffer to store their value/representation
char identifierStr[MAX_ID_LEN + 1]; // string value of identifier
int intVal;   // value of number, if found

typedef enum token_types{
  tok_EOF = -1, tok_BEGIN = -2 , tok_END = -3, tok_READ = -4, tok_WRITE = -5, 
	tok_ID = -6, tok_INT_LITERAL = -7, tok_LPAREN = -8, tok_RPAREN = -9,
	tok_SEMICOLON = -10, tok_COMMA = -11, tok_ASSIGN = -12, tok_OP_PLUS = -13, 
	tok_OP_MINUS = -14, tok_SCAN_EOF = -15
} token;

extern int tokenize(int);

#endif
