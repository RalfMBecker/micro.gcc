/***************************************************************
* ast.h -          AST for Micro
* Language:        Micro
*
****************************************************************
* Note:  as we can't use inheritance (->C++), we assign
*        an "exprRecord" for all types of results, with the usual
*        work-around of having a 'kind' enumeration.
***************************************************************/

#ifndef AST_H_
#define AST_H_

#include "hashtab.h"
#include "compiler.h"

typedef char stringID[MAX_ID_LEN+1];

typedef struct operator{
    enum oper { PLUS, MINUS, MUL, DIV } op;
} opRecord;

enum expr { EXPR_ID, EXPR_TMP, EXPR_INT_LITERAL, EXPR_LONG_LITERAL, 
	    EXPR_FLT_LITERAL};

enum types{ INVALID = 0, INTEGER = 1, LONG = 2, FLOAT =3, FCT_DECL = 4, 
	    FCT_IMPL=5 }; // numbers for table lookup
 
// Note that, for the IR, we need not worry about loss of precision
// for numerical types/values, so we need just one integer type and
// one floating type that is large enough for all such types.
typedef struct expression {
    enum expr kind;
    union {
	stringID name;  // will hold its identifierStr, if any
	long val_int;      // will hold its numValue, if any
	double val_flt;    // will hold its fltVal, if any
    };
    enum types type; 
} exprRecord;

#endif
