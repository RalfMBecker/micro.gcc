/*******************************************************
* codegen.h -            header file for codegen.c
* Language:              Micro
*
********************************************************/

#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "hashtab.h"
#include "ast.h"
#include "lexer.h"

#define NUMREGS 12 // relocate to interpreter

extern struct nlist* symbolTable[HASHSIZE];

void createSymbolTable(void);
struct nlist* writeSymbolTable(int exprType, char* name, char* type);
struct nlist* readSymbolTable(const char* name);

opRecord makeOpRec(token tok);
exprRecord makeIDRec(token tok);
exprRecord makeLiteralRec(token tok);

void codegen_DECLARE(char* ID, char* storage);
void codegen_ASSIGN(char* dest, char* src);
void codegen_BEGIN(int fd, const char* name);
void codegen_END(void);

#endif
