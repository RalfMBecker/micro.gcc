/*************************************************************
* hashtab.c -          hash table for use in symbol table
* Language:            Micro
*
**************************************************************
* Usage: 
*         install(test, "1");
*         struct nlist* p; p = lookup("name");
*                          p= undef("name");
*         don't forget to initialize table:
*         for (i = 0; i < HASHSIZE; i++) (hashtab[i] = NULL);
*************************************************************/

#include "hashtab.h"

struct nlist{
  struct nlist* next;
  char* name;
  char* defn;
};

unsigned hash(char*);
struct nlist* findprior(char*);
char* mystrdup(char*);

struct nlist* findprior(char* s){

  struct nlist* np;
  struct nlist* np_prior;

  for (np_prior = np = hashtab[hash(s)]; np != NULL; np = np->next){
    if (strcmp(s, np->name) == 0)
      return np_prior;
    np_prior = np;
  }

  return np_prior;
}

struct nlist* lookup(char* s){

  struct nlist* np;

  for (np = hashtab[hash(s)]; np != NULL; np = np->next)
    if (strcmp(s, np->name) == 0)
      return np;

  return NULL;
}

// in linked list rooted at hash(name), find nlist* of same hash value, 
// if any, preceding the name to be undefined; then re-link properly
int undef(char* name){

  struct nlist* p;
  struct nlist* p_prior;

  p = lookup(name);
  if (p == NULL)
    return -1;
  p_prior = findprior(name);

  if (!(p_prior == p))
    p_prior->next = p->next;
  else
    hashtab[hash(name)] = NULL;
  free ( (void*) p->name);
  free ( (void*) p->defn);
  free( (void*) p); // free only frees the space, doesn't undefine

  return 0;
}

struct nlist* install(char* name, char* defn){

  struct nlist* np;
  unsigned hashval;

  if ( (np = lookup(name)) == NULL){
    np = (struct nlist*) malloc(sizeof(struct nlist));
    if (np == NULL || (np->name = mystrdup(name)) == NULL)
      return NULL;
    hashval = hash(name);
    np->next = hashtab[hashval];
    hashtab[hashval] = np;
  }
  else
    free( (void*) np->defn);
  if( (np->defn = mystrdup(defn) ) == NULL)
      return NULL;
  else
    return np;
}

unsigned hash(char* s){

  unsigned hashval;

  for (hashval = 0; *s != '\0'; s++)
    hashval = *s + 31*hashval;

  return hashval%HASHSIZE;
}

char* mystrdup(char* s){

  char* p;

  p = (char*) malloc(strlen(s)+1);
  if (p != NULL)
    strcpy(p, s);
  return p;
}
