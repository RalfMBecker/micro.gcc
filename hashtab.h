/*************************************************************
* hashtab.h -         header file for hashtab.c
* Language:           Micro
*
**************************************************************
* struct nlist{ 
*     struct nlist* next;
*     char* name;
*     char* defn; };
**************************************************************
* Usage: 
*         install(<hashtab>, "test", "1");
*         struct nlist* p; p = lookup(<hashtab>, "name");
*                          p= undef(<hashtab>, "name");
*         don't forget to initialize table:
*         for (i = 0; i < HASHSIZE; i++) (hashtab[i] = NULL);
*************************************************************/

#ifndef HASHTAB_H_
#define HASHTAB_H_

#include "compiler.h"

#define HASHSIZE 101

struct nlist{
  struct nlist* next;
  char* name;
  char* type;
	char* scope;
	char* storage;
};

struct nlist* lookup(struct nlist**, const char*);
struct nlist* install(struct nlist**, char* name, char* type, 
											char* scope, char* storage); 
       // send NULL if no type/scope/storage to be stored
      // placeholder if any of type, scope, storage undefined: "placeholder"
int undef(struct nlist**, const char*);
void printHashTable(struct nlist**);

#endif
