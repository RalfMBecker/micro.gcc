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

#include "ast.h"

#define HASHSIZE 101

// type is actually 'enum types'. To avoid 'incomplete type' error,
// would need to put 'enum types' definition in joint header file.
// preferred to keep in ast.h for easy access
struct nlist{
    struct nlist* next;
    char* name;
    int type;
    char* scope;
    char* storage; 
};

struct nlist* lookup(struct nlist**, const char*);
struct nlist* install(struct nlist**, char* name, int type, 
		      char* scope, char* storage);
int undef(struct nlist**, const char*);
void printHashTable(struct nlist**);

#endif
