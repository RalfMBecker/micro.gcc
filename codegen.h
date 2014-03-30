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
struct nlist* writeSymbolTable(int exprType, char* name, int type, char* scope);
struct nlist* readSymbolTable(const char* name);

opRecord makeOpRec(token tok);
exprRecord makeIDRec(const char* name);
exprRecord makeLiteralRec(token tok);
exprRecord generateInfix(const exprRecord LHS, 
			 const opRecord op, const exprRecord RHS);

int checkCast(const exprRecord LHS, const exprRecord RHS);
exprRecord castRecord(const exprRecord rec, int to);

void codegen_DECLARE(const exprRecord);
// kind: 0 - assignment (name == storage); 1 - copy assignment
void codegen_ASSIGN(const exprRecord LHS, const exprRecord RHS, int kind);
void codegen_FUNCTION(const char* name);
void codegen_END(const char*);
void codegen_TU(int fd, const char*);

#endif
