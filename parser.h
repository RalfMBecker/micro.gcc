/****************************************************
* parser.h -          header file for parser.c 
* Language:           Micro
*
*****************************************************/

#ifndef PARSER_H_
#define PARSER_H_

extern int curTok;

void Statement(int fd, int readToken);
int match(int update, int fd, token, int readAhead);
int getNextToken(int);

#endif
