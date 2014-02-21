/***************************************************************
* ast.h -          AST for Micro
* Language:        Micro
*
***************************************************************/
// Note: as we can't use inheritance (->C++), we assign
//        an "exprRecord" for all types of results. We'd prefer
//        to have a hierarchy: primRec -> exprRecord, but this
//        would at the least need a (cast) and be ugly
//**************************************************************

#ifndef AST_H_
#define AST_H_

#include "hashtab.h"

typedef char stringID[MAX_ID_LEN+1];

typedef struct operator{
	enum oper { PLUS, MINUS } op;
} opRecord;

enum expr { EXPR_ID, EXPR_TMP,
						EXPR_INT_LITERAL, EXPR_LONG_LITERAL, EXPR_FLT_LITERAL 
};
enum types { INTEGER = 1, LONG = 2, FLOAT =3 
}; // numbers for table lookup
 
// note that we, for the IR, we need not worry about loss of precision
// for numerical types/values, so we just need an integer type and a 
// a floating type. Choose the largest for simplicity. 
// We do not to produce code for (a) implicit promotions/conversions 
// during evalutation of expressions, and (b) assigning an expression
// to a LHS (which could lead to loss; not handled in IR).
typedef struct expression {
	enum expr kind;
	union {
		stringID name;  // will hold its identifierStr, if any
		long val_int;      // will hold its numValue, if any
		double val_flt;
	};
	enum types type; 
} exprRecord;


#endif
