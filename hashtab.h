/*******************************************************
* hashtab.h -         header file for hashtab.c
* Language:           Micro
*
********************************************************/

#include "compiler.h"

#define HASHSIZE 101

struct nlist *hashtab[HASHSIZE];

struct nlist* lookup(char*);
struct nlist* install(char*, char*);
int undef(char*);
