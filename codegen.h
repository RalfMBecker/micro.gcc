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
struct nlist* writeSymbolTable(int exprType, char* name, int type);
struct nlist* readSymbolTable(const char* name);

opRecord makeOpRec(token tok);
exprRecord makeIDRec(token tok);
exprRecord readIDRec(token tok);
exprRecord makeLiteralRec(token tok);

void codegen_DECLARE(const char* ID, int type, const char* storage);
void codegen_ASSIGN(const char* dest, const char* src);
void codegen_FUNCTION(const char* name);
void codegen_END(const char*);
void codegen_TU(int fd, const char*);

#endif
