/*******************************************************
* codegen.h -            header file for codegen.c
* Language:              Micro
*
********************************************************/

#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "hashtab.h"
#include "ast.h"

#define NUMREGS 12 // relocate to interpreter

extern struct nlist* symbolTable[HASHSIZE];

void createSymbolTable(void);
struct nlist* rwSymbolTable(char* name, char* type);
int checkID_Defined(const char* name);

void codegen_DECLARE(char* ID, char* storage);
void codegen_ASSIGN(char* dest, char* src);
void codegen_BEGIN(int fd, const char* name);
void codegen_END(void);

#endif
