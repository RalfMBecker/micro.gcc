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

typedef enum token_types{
  tok_BEGIN, tok_END, tok_READ, tok_WRITE, to_ID, to_INT_LITERAL, tok_LPAREN, 
  tok_RPAREN, tok_SEMICOLON, tok_COMMA, tok_OP_ASSIGN, tok_OP_PLUS, 
  tok_OP_MINUS, tok_SCAN_EOF, tok_EOF
} token;
// note: with changed logic, probably will remove tok_SCAN_EOF and tok_EOF
//       out logic is likely to check for 0-value, which applies if we refer
//       to EOF, but not if we do to tok_EOF etc.

extern token tokenize(void);

#endif
