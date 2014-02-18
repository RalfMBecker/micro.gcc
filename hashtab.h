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

#include "compiler.h"

#define HASHSIZE 101

struct nlist{
  struct nlist* next;
  char* name;
  char* defn;
};


struct nlist* lookup(struct nlist**, char*);
struct nlist* install(struct nlist**, char*, char*);
int undef(struct nlist**, char*);
void printHashTable(struct nlist**);

