/*******************************************************
* lexer.h -            header file for lexer.c
* Language:            Micro
*
********************************************************/

#ifndef LEXER_H_
#define LEXER_H_

#include <ctype.h>
#include <fcntl.h>        // open(), O_RDONLY

#ifndef COMPILER_H
#include "compiler.h"
#endif

// int literals and identifiers need not only a token to say what they are,
// but also a buffer to store their value/representation
char identifierStr[MAX_ID_LEN + 1]; // string value of identifier
int intVal;   // value of number, if found

typedef enum token_types{
  tok_EOF = -1, tok_BEGIN , tok_END, tok_READ, tok_WRITE, tok_ID, 
  tok_INT_LITERAL, tok_LPAREN, tok_RPAREN, tok_SEMICOLON, tok_COMMA, 
  tok_ASSIGN, tok_OP_PLUS, tok_OP_MINUS, tok_SCAN_EOF
} token;
// note: with changed logic, probably will remove tok_SCAN_EOF

extern token tokenize(int);

#endif
