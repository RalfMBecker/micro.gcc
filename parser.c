/****************************************************
* parser.c -          Recursive Descent Parser 
* Language:           Micro
*
*****************************************************/

#include "lexer.h"
#include "error.h"

int curTok;

//*****************************************************
// helper routines / interface to driver.c and lexer.c
//*****************************************************

int
getNextToken(int fd){ return (curTok = tokenize(fd)); }

// update = 0: curTok needs no updating
//        = 1: curTok needs updating
int
match(int update, int fd, token tok){

	if (update) {curTok = getNextToken(fd);} 

  if ( (tok == curTok) )
		return 0;

	errExit(0, "syntax error - invalid token, or token invalid in context");
	return -1; // to suppress gcc warning
}

//**********************************************************
// recursive descent
// Note: except for where explicitly pointed out, routines
//       need to begin by updating curTok
//**********************************************************

//void Program(int);
//void statementList(int);
void Statement(int, int);
void Expression(int, int);
void expressionList(int, int);
void idList(int, int);
void Primary(int, int);

// Not implemented in parser.c - handled mostly in logical structure
// of driver.c:
//    system goal -> program EOF
//    program -> BEGIN statement-list END
//    statement-list -> statement [statement]*


/*
int
systemGoals(int fd){

	Program(fd);
	return match(1, fd, tok_EOF);
}

void
Program(int fd){

	match(1, fd, tok_BEGIN);
	statementList(fd);
	match(0, fd, tok_END); // when we return, statement list has read one ahead
	puts("successfully processed a program");
}
*/


/*
void
statementList(int fd){

	puts("\tchecking for statement-list");

	getNextToken(fd); // validity check in Statement()
	Statement(fd);
	for(;;){
		switch( getNextToken(fd) ){
		case tok_ID:
		case tok_READ:
		case tok_WRITE:
			Statement(fd);
			break;
		default: return;
		}
	}
	puts("\tsuccessfully processed a statement-list");

}
*/


// statement -> ID := expression;
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
	case tok_ID: 
		printf("  successfully processed a LHS(statment): ID - %s\n", 
					 identifierStr);
		match(1, fd, tok_ASSIGN);
		puts("  found primary: ASSIGN");
		// getNextToken(fd);
		Expression(fd, 1); /// CONFIRM
		match(0, fd, tok_SEMICOLON); // relocate 3 instances of this check
		        // after the switch when done with debugging
		puts("  found primary: SEMICOLON");
		break;
	case tok_READ:
		puts("  successfully processed a function-statment: READ");
		match(1, fd, tok_LPAREN);
		puts("  found primary: LPAREN");
		//		match(1, fd, tok_ID);
		// printf("\tmatched one ID - %s\n", identifierStr);
		idList(fd, 0); 
		match(0, fd, tok_RPAREN);  // upon returning, idList looks ahead
		puts("  found primary: RPAREN");
		match(1, fd, tok_SEMICOLON);
		puts("  found primary: SEMICOLON");
		break;
	case tok_WRITE:
		puts("  successfully processed a function-statment: WRITE");
		match(1, fd, tok_LPAREN);
		puts("  found primary: LPAREN");
		expressionList(fd, 0); /// CONFIRM
		match(0, fd, tok_RPAREN);  // see below
		puts("  found primary: RPAREN");
		match(1, fd, tok_SEMICOLON);
		puts("  found primary: SEMICOLON");
		break;
	default: errExit(0, "illegal expression"); break;
	} // end switch

	puts("successfully matched a statement");
}

// expression -> primary [add_op primary]*
//
// Note:     fall through to get token
// Note:     upon return, curTok points 1 ahead
void
Expression(int fd, int readToken){

	puts("    checking for expression");

	Primary(fd, readToken);
	while ( (curTok == tok_OP_PLUS)  || (curTok == tok_OP_MINUS) ){
		printf("      successfully processed an ADD_OP: %s\n", 
				 (curTok == tok_OP_PLUS)?"OP_PLUS":"OP_MINUS");
		//getNextToken(fd);
		Primary(fd, 1); // CONFIRM
	}
	// at this point, curTok points ahead (e.g., to a ';')

	puts("    successfully matched an expression");
}

// NOTE: as currently drafter, we do not need OP_ADD/OP_MIN
//       routines: they are folded into 'Expression'
//       (not ideal if more operators; ok for 2)

//

// primary -> ( expression )
//
//            ID
//            INT_LITERAL
//            OP_PLUS
//            OP_MINUS
// Note: fct returns with curTok pointing 1 ahead
void
Primary(int fd, int readToken){

	puts("        checking for primary");

	if (readToken) getNextToken(fd);

	switch(curTok){
	case tok_LPAREN:
		puts("          found primary: LPAREN");
		//getNextToken(fd);
		Expression(fd, 1); /// CONFIRM
		match(0, fd, tok_RPAREN); // Expression() reads ahead
		getNextToken(fd);
		break;
	case tok_ID: 
		printf("          found primary: ID - %s\n", identifierStr);
		getNextToken(fd);
		break;
	case tok_INT_LITERAL: 
		printf("          found primary: INT_LITERAL - %d\n", intVal);
		getNextToken(fd);
		break;
	default: errExit(0, "invalid primary"); break;
	}

	puts("        successfully processed a primary");
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

	match(1, fd, tok_ID);
	printf("      matched one ID - %s\n", identifierStr);

	while ( (tok_COMMA == getNextToken(fd)) ){
		match(1, fd, tok_ID);
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



