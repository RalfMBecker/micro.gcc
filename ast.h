/*************************************************************
* ast.h -          AST for Micro
* Language:        Micro
*
*************************************************************/

#ifndef AST_H_
#define AST_H_

#include "hashtab.h"

typedef char stringID[MAX_ID_LEN+1];

typedef struct operator{
	enum op { PLUS, MINUS } oper;
} opRecord;

enum expr { EXPR_ID, EXPR_INT_LITERAL, EXPR_TEMP };
enum type { INTEGER, FLOAT };
 
typedef struct expression {
	enum expr kind;
	union {
		stringID name;  // will hold its identifierStr, if any
		int val;      // will hold its numValue, if any
	};
	enum type whattype; 
} exprRecord;

#endif
