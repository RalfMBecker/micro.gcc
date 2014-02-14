/*************************************************************
* lexer.c -            lexer part of compiler
* Language:            Micro
*
* Note:                for Micro, 1 char look-ahead is enough
*************************************************************/

#include "compiler.h"
#include "lexer.h"

static void
next_char(int fd, int* last_char){

  ssize_t numRead;

  numRead = read(fd, last_char, 1);
  if (numRead == -1)
    errExit(1, "...int read()...");
  else if (numRead == 0)
    *last_char = EOF;
}

token tokenize(void){

  return tok_EOF;;
}
