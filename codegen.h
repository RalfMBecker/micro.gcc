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
//exprRecord readIDRec(token tok);
exprRecord makeLiteralRec(token tok);
exprRecord generateInfix(const exprRecord LHS, 
												 const opRecord op, const exprRecord RHS);

int checkCast(const exprRecord LHS, const exprRecord RHS);
exprRecord castRecord(const exprRecord rec, int to);

void codegen_DECLARE(const exprRecord);
void codegen_ASSIGN(const exprRecord LHS, const exprRecord RHS);
void codegen_FUNCTION(const char* name);
void codegen_END(const char*);
void codegen_TU(int fd, const char*);

#endif
