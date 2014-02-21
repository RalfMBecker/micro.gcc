/****************************************************
* parser.c -          Recursive Descent Parser 
* Language:           Micro
*
*****************************************************/

#include "lexer.h"
#include "error.h"
#include "ast.h"
#include "codegen.h"

int curTok;

//*****************************************************
// helper routines / interface to driver.c and lexer.c
//*****************************************************

int
getNextToken(int fd){ return (curTok = tokenize(fd)); }

// update = 0: curTok needs no updating before processing
//        = 1: curTok needs updating
// readAhead = 0: after the above, do not further forward curTok
//           = 1:      "         , do getNextToken() again
int
match(int update, int fd, token tok, int readAhead){

	if (update) getNextToken(fd); 

  if ( (tok == curTok) ) {
		if (readAhead) getNextToken(fd);

		return 0;
	}

	errExit(0, "syntax error - invalid token, or token invalid in context");
	return -1; // to suppress gcc warning
}

//**********************************************************
// recursive descent
// Note: except for where explicitly pointed out, routines
//       need to begin by updating curTok
// Note2: as to return type, see comment in ast.h
//**********************************************************

void Statement(int, int);
exprRecord Declaration(int, int);
exprRecord Expression(int, int);
exprRecord Primary(int, int);
void expressionList(int, int);
void idList(int, int);

// Not implemented in parser.c - handled mostly in logical structure
// of driver.c:
//    system goal -> program EOF
//    program -> BEGIN statement-list END
//    statement-list -> statement [statement]*


// statement -> declaration
//              ID := expession;  // ID must be first declared
//              read( id-list);
//              write( expr-list);
//
// Upon starting the descent from driver.c, getNextToken() has been
// called already; so curTok points to the right token.
// Note:   function leaves 'clean', pointing to last processed token
void
Statement(int fd, int readToken){

	puts("checking for statement");

	switch(curTok){

	case tok_DEC_INT:
		Declaration(fd, INTEGER);
		break;
	case tok_DEC_LONG:
		Declaration(fd, LONG);
		break;
	case tok_DEC_FLT:
		Declaration(fd, FLOAT);
		break;

	case tok_ID: 
		if ( (NULL == lookup(symbolTable, identifierStr) ) )
			errExit(0, "cannot assign to undeclared identifier (%s)", identifierStr);
		printf("  successfully processed a LHS(statment): ID - %s\n", 
					 identifierStr);
		match(1, fd, tok_ASSIGN, 0);
		puts("  found primary: ASSIGN");
		Expression(fd, 1); /// CONFIRM
		match(0, fd, tok_SEMICOLON, 0); // relocate 3 instances of this check
		        // after the switch when done with debugging
		puts("  found primary: SEMICOLON");
		break;

	case tok_READ:
		puts("  successfully processed a function-statment: READ");
		match(1, fd, tok_LPAREN, 0);
		puts("  found primary: LPAREN");
		idList(fd, 0); 
		match(0, fd, tok_RPAREN, 0);  // upon returning, idList looks ahead
		puts("  found primary: RPAREN");
		match(1, fd, tok_SEMICOLON, 0);
		puts("  found primary: SEMICOLON");
		break;

	case tok_WRITE:
		puts("  successfully processed a function-statment: WRITE");
		match(1, fd, tok_LPAREN, 0);
		puts("  found primary: LPAREN");
		expressionList(fd, 0); /// CONFIRM
		match(0, fd, tok_RPAREN, 0);  // see below
		puts("  found primary: RPAREN");
		match(1, fd, tok_SEMICOLON, 0);
		puts("  found primary: SEMICOLON");
		break;

	default: errExit(0, "illegal expression"); break;
	} // end switch

	puts("successfully matched a statement");
}

// declaration -> type id;
//                type id = expr;
//     (type in {int, float})
// Note: when arriving here, type has already been found
exprRecord
Declaration(int fd, int type){

	exprRecord eRec;
	strcpy(eRec.name, "test"); eRec.type = INTEGER; eRec.kind = EXPR_ID;

	struct nlist* LHS_S;
	//	exprRecord LHS, RHS;    

	match(1, fd, tok_ID, 1);

	// if the ID is followed (illegally) by a token that overwrites
	// identifierStr used for the ID, we store a wrong value.
	// however, only ';' and '=' are legal, so compilation stops anyway then
	if ( (NULL != readSymbolTable(identifierStr)) )
		errExit(0, "attempting to re-declare identifier (%s)", identifierStr);

	// recall that we read one token ahead
	if ( (tok_SEMICOLON == curTok) || (tok_ASSIGN == curTok) )
		LHS_S = writeSymbolTable(1, identifierStr, type);
	if ( (NULL == LHS_S) )
			errExit(0, "error inserting identifier %s into symbol table", 
							identifierStr);

	switch (curTok){

	case tok_SEMICOLON:  // declaration case 
		codegen_DECLARE(identifierStr, type, LHS_S->storage);
		break;

	case tok_ASSIGN:  // copy assignment case
		//		RHS = Expression(fd, 1); // TO DO: fct sig
		codegen_DECLARE(identifierStr, type, LHS_S->storage);
		Expression(fd, 1);
		codegen_ASSIGN(LHS_S->storage, "RES OF EXPRESSION");

		match(0, fd, tok_SEMICOLON, 0);
		// TO DO: perfrom binary operation; into AST to hand on as return
		break;

	default: errExit(0, "illegal syntax in declaration"); break;
	}

	return eRec;
}

// expression -> primary [add_op primary]*
//
// Note:     fall through to get token
// Note:     upon return, curTok points 1 ahead
exprRecord
Expression(int fd, int readToken){

	exprRecord eRec;
	strcpy(eRec.name, "test"); eRec.type = INTEGER; eRec.kind = EXPR_ID;

	puts("    checking for expression");

	Primary(fd, readToken);
	while ( (curTok == tok_OP_PLUS)  || (curTok == tok_OP_MINUS) ){
		printf("      successfully processed an ADD_OP: %s\n", 
				 (curTok == tok_OP_PLUS)?"OP_PLUS":"OP_MINUS");
		Primary(fd, 1); // CONFIRM
	}
	// at this point, curTok points ahead (e.g., to a ';')

	puts("    successfully matched an expression");

	return eRec;
}

// NOTE: as currently drafter, we do not need OP_ADD/OP_MIN
//       routines: they are folded into 'Expression'
//       (not ideal if more operators; ok for 2)

//

// primary -> ( expression )
//            -[INT_LITERAL|LONG_LITERAL|FLT_LITERAL] (TO DO)
//            ID
//            INT_LITERAL | LONG_LITERAL | FLT_LITERAL
//            OP_PLUS
//            OP_MINUS
// Note: fct returns with curTok pointing 1 ahead
exprRecord
Primary(int fd, int readToken){

	exprRecord eRec;
	strcpy(eRec.name, "test"); eRec.type = INTEGER; eRec.kind = EXPR_ID;

	puts("        checking for primary");

	if (readToken) getNextToken(fd);

	switch(curTok){
	case tok_LPAREN:
		puts("          found primary: LPAREN");
		Expression(fd, 1); 
		match(0, fd, tok_RPAREN, 1); // Expression() reads ahead
		break;
	case tok_ID: 
		// we cannot declare when we come here - done before
		if ( (NULL == readSymbolTable(identifierStr)) )
			errExit(0, "illegal use of undeclared identifier (%s)", identifierStr);
		printf("          found primary: ID - %s\n", identifierStr);
		getNextToken(fd);
		break;
	case tok_INT_LITERAL: 
		printf("          found primary: INT_LITERAL - %ld\n", intVal);
		getNextToken(fd);
		break;
	case tok_FLT_LITERAL: 
		printf("          found primary: FLT_LITERAL - %g\n", fltVal);
		getNextToken(fd);
		break;
		/*	case tok_OP_MINUS:
		puts("          found primary: unary MINUS");
		getNextToken(fd);
		break;
		*/
	default: errExit(0, "invalid primary"); break;
	}

	puts("        successfully processed a primary");

	return eRec;
}

//*************************************************************
// arg cases, etc.
//
// Note: Token that 'glue' (a|y a, y - etc) seem fine to handle
//       directly using getNextToken(), instead of relying on
//       an artifical descent (alternatively, have a non-killing
//       function like match() (confirm(), say) which wraps
//       around the call to getNextToken()
//*************************************************************

// id-list -> id [,id]*
//
// Note: we enter having not yet confirmed any id
// Note2: curTok should not point ahead upon entry
// Note3: when done, curTok points ahead
void
idList(int fd, int readToken){

	puts("    checking for id-list");

	match(1, fd, tok_ID, 0);
	printf("      matched one ID - %s\n", identifierStr);

	while ( (tok_COMMA == getNextToken(fd)) ){
		match(1, fd, tok_ID, 0);
		printf("      matched one ID - %s\n", identifierStr);
	}

	puts("    successfully matched an id-list");
}

// expression-list -> expression [, expression]*
//
// Note:     we enter having not yet confirmed any expression
// Note 2:   when done, curTok points ahead
void expressionList(int fd, int readToken){

	puts("  checking for expression-list");

	Expression(fd, 1);  // recall: we point ahead after
	while ( (tok_COMMA == curTok) )
		Expression(fd, 1);  /// again, we'll point ahead 

	puts("  successfully matched an expression-list");
}



