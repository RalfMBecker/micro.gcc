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
//
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
//    statement-list -> statement [statement]* (add back for functions)


// type:  0 - assign; 1 - copy assignment
static void
castAndAssign(const exprRecord LHS, const exprRecord RHS, int type){

	exprRecord tmpRecord;

	if ( (0 != checkCast(LHS, RHS)) ){  // cast RHS to type assigned to
		tmpRecord = castRecord(RHS, LHS.type);
		codegen_ASSIGN(LHS, tmpRecord, type);
	}
	else// LHS.type = RHS.type
		codegen_ASSIGN(LHS, RHS, type);
}

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

	exprRecord LHS, RHS;
	struct nlist* pNL;

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

		// UNDO CHANGE ++++++++ MAKE FAKE TMP 
	case tok_ID: // note: ID found has already been entered into the ast
		           // and ST with a call to makeIDRec when first encountered
		if ( (NULL == (pNL = lookup(symbolTable, identifierStr)) ) )
			errExit(0, "cannot assign to undeclared identifier (%s)", identifierStr);

		// create a fake TMP object to handle processing more elegantly
		strcpy(LHS.name, pNL->storage); 
		LHS.kind = EXPR_TMP;
		LHS.type = pNL->type;

		match(1, fd, tok_ASSIGN, 0);
		RHS = Expression(fd, 1); 
		castAndAssign(LHS, RHS, 0);
		match(0, fd, tok_SEMICOLON, 0);
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

}

// declaration -> type id;
//                type id = expr;
//     (type in {int, long, float})
// Note: when arriving here, type has already been found
exprRecord
Declaration(int fd, int type){

	exprRecord LHS, RHS;
	struct nlist* LHS_S;
	char tmpScope[15];
	strcpy(tmpScope, "placeholder");

	match(1, fd, tok_ID, 1);

	if ( (NULL != readSymbolTable(identifierStr) ) )
		errExit(0, "attempting to re-declare identifier (%s)", identifierStr);

	// recall that we read one token ahead
	if ( !( (tok_SEMICOLON == curTok) || (tok_ASSIGN == curTok) ) )
		errExit(0, "invalid symbol after declaration (%d)", curTok);

	LHS_S = writeSymbolTable(EXPR_ID, identifierStr, type, tmpScope);
	if ( (NULL == LHS_S) )
		errExit(0, "error inserting %s into symbol table", identifierStr);

	LHS = makeIDRec(identifierStr);
	codegen_DECLARE(LHS);

	switch (curTok){
	case tok_SEMICOLON:  // declaration case 
		break;
	case tok_ASSIGN:  // copy assignment case
		RHS = Expression(fd, 1);
		castAndAssign(LHS, RHS, 1);
		match(0, fd, tok_SEMICOLON, 0);
		break;
	default: errExit(0, "illegal syntax in declaration"); break;
	}

	return LHS;
}

// expression -> primary [add_op primary]*
//
// Note:     fall through to get token
// Note:     upon return, curTok points 1 ahead
exprRecord
Expression(int fd, int readToken){

	exprRecord LHS, RHS;
	opRecord opRec;

	LHS = Primary(fd, readToken);
	while ( (curTok == tok_OP_PLUS)  || (curTok == tok_OP_MINUS) ){
		opRec = makeOpRec(curTok);
		RHS = Primary(fd, 1);
		LHS = generateInfix(LHS, opRec, RHS);
	}
	// at this point, curTok points ahead (e.g., to a ';')

	return LHS;
}

// primary -> ( expression )
//            -[INT_LITERAL|LONG_LITERAL|FLT_LITERAL] (TO DO)
//            ID
//            INT_LITERAL | LONG_LITERAL | FLT_LITERAL
//            OP_PLUS
//            OP_MINUS
// Note: fct returns with curTok pointing 1 ahead
exprRecord
Primary(int fd, int readToken){

	exprRecord ret;

	if (readToken) getNextToken(fd);

	switch(curTok){
	case tok_LPAREN:
		ret = Expression(fd, 1); 
		match(0, fd, tok_RPAREN, 1); // Expression() reads ahead
		break;
	case tok_ID: 
		// we cannot declare when we come here - done before
		if ( (NULL == readSymbolTable(identifierStr)) )
			errExit(0, "illegal use of undeclared identifier (%s)", identifierStr);
		ret = makeIDRec(identifierStr);
		getNextToken(fd);
		break;
	case tok_INT_LITERAL: 
	case tok_FLT_LITERAL:
		ret = makeLiteralRec(curTok);
		getNextToken(fd);
		break;
		/*	case tok_OP_MINUS:
		puts("          found primary: unary MINUS");
		getNextToken(fd);
		break;
		*/
	default: errExit(0, "invalid primary"); break;
	}

	return ret;
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



