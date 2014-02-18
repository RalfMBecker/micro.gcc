/****************************************************
* parser.h -          header file for parser.c 
* Language:           Micro
*
*****************************************************/

#ifndef PARSER_H_
#define PARSER_H_

#include "compiler.h"

extern int curTok;

void Statement(int fd, int readToken);
int match(int, int, token);
int getNextToken(int);

#endif
