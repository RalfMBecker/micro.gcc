/*******************************************************
* codegen.h -            header file for codegen.c
* Language:              Micro
*
********************************************************/

#include "hashtab.h"

#define NUMREGS 12

extern struct nlist* symbolTable[HASHSIZE];

void createSymbolTable(void);

void codegen_BEGIN(int, char*);
void codegen_END(void);
