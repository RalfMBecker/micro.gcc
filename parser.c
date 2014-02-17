/****************************************************
* parser.c -          Recursive Descent Parser 
* Language:           Micro
*
*****************************************************/

#include "lexer.h"
#include "error.h"

int curTok;

//****************************************
// helper routines
//****************************************

static int
getNextToken(int fd){ return (curTok = tokenize(fd)); }

// update = 0: curTok needs no updating
//        = 1: curTok needs updating
static int
match(int update, int fd, token tok){

	if (update) {curTok = getNextToken(fd);} 

  if ( (tok == curTok) );
		return 0;

	errExit(0, "syntax error - invalid token");
	return -1; // to suppress gcc warning
}

//**********************************************************
// recursive descent
// Note: except for where explicitly pointed out, routines
//       need to begin by updating curTok
//**********************************************************

void Program(int);
void statementList(int);
void Statement(int);
void Expression(int);
void expressionList(int);
void idList(int);
void Primary(int);

// system goal -> program EOF
int
systemGoals(int fd){

	Program(fd);
	return match(1, fd, tok_EOF);
}

// program -> BEGIN statement-list END
void
Program(int fd){

	match(1, fd, tok_BEGIN);
	statementList(fd);
	match(0, fd, tok_END); // when we return, statement list has read one ahead
	puts("successfully processed a program");
}

// statement-list -> statement [statement]*
// Note:    leaves with curTok 1 ahead
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

// statement -> ID := expression;
//              read( id-list);
//              write( expr-list);
// note that curToken points to one of the above (or an illegal
// token) upon entering this routine
// Note:   function leaves 'clean', pointing to last processed token
void
Statement(int fd){

	puts("\tchecking for statement");

	switch(curTok){
	case tok_ID: 
		printf("\t\tsuccessfully processed a LHS(statment): ID - %s\n", 
					 identifierStr);
		match(1, fd, tok_ASSIGN);
		puts("\tfound primary: ASSIGN");
		getNextToken(fd);
		Expression(fd);
		match(0, fd, tok_SEMICOLON);
		puts("\tfound primary: SEMICOLON");
		break;
	case tok_READ:
		puts("\t\tsuccessfully processed a LHS(statment): READ");
		match(1, fd, tok_LPAREN);
		puts("\tfound primary: LPAREN");
		match(1, fd, tok_ID);
		printf("\tmatched one ID - %s\n", identifierStr);
		idList(fd);
		match(0, fd, tok_RPAREN);  // see below
		puts("\tfound primary: RPAREN");
		match(1, fd, tok_SEMICOLON);
		puts("\tfound primary: SEMICOLON");
		break;
	case tok_WRITE:
		puts("\t\tsuccessfully processed a LHS(statment): WRITE");
		match(1, fd, tok_LPAREN);
		puts("\tfound primary: LPAREN");
		expressionList(fd);
		match(0, fd, tok_RPAREN);  // see below
		puts("\tfound primary: RPAREN");
		match(1, fd, tok_SEMICOLON);
		puts("\tfound primary: SEMICOLON");
		break;
	default: errExit(0, "illegal expression"); break;
	} // end switch

	puts("\t\tsuccessfully processed a statement");
}

// id-list -> id [,id]*
// Note: we only enter having already found an initial ID, 
//       so need to only check the [,fd]* part
// Note 2: when done, curTok points ahead
void
idList(int fd){

	puts("\tchecking for id-list");

	while ( (getNextToken(fd) == tok_COMMA) ){
		match(1, fd, tok_ID);
		printf("\tmatched one ID - %s\n", identifierStr);
	}
		//		match(1, fd, tok_ID);

	puts("\t\t\tsuccessfully matched an id-list");
}

// *****CHECK ****
// expression-list -> expression [, expression]*
// Note:     we enter having not yet confirmed any expression
// Note 2:   when done, curTok points ahead
void expressionList(int fd){

	puts("\tchecking for expression-list");

	getNextToken(fd);
	Expression(fd);
	while ( (tok_COMMA == getNextToken(fd)) ){
		getNextToken(fd);
		Expression(fd);
	}

	puts("\t\t\tsuccessfully matched an expression-list");
}


// expression -> primary [add_op primary]*
// Note:     called with curTok pointing 1 ahead
// Note:     upon return, curTok points 1 ahead
void
Expression(int fd){

	puts("\tchecking for expression");

	Primary(fd);
	while ( ( getNextToken(fd) == tok_OP_PLUS ) || 
					(curTok == tok_OP_MINUS) ){
		printf("\tsuccessfully process an ADD_OP: %s\n", 
				 (curTok == tok_OP_PLUS)?"OP_PLUS":"OP_MINUS");
		getNextToken(fd);
		Primary(fd);
	}

	puts("\t\t\t\tsuccessfully matched an expression");
}

// NOTE: as currently drafter, we do not need OP_ADD/OP_MIN
//       routines: they are folded into 'Expression'
//       (not ideal if more operators; ok for 2)

//

// primary -> ( expression )
//            ID
//            INT_LITERAL
//            OP_PLUS
//            OP_MINUS
void
Primary(int fd){

	puts("\tchecking for primary");

	switch(curTok){
	case tok_LPAREN:
		puts("\tfound primary: LPAREN");
		getNextToken(fd);
		Expression(fd);
		match(0, fd, tok_RPAREN);
		break;
	case tok_ID: 
		printf("\tfound primary: ID - %s\n", identifierStr);
		break;
	case tok_INT_LITERAL: 
		printf("\tfound primary: INT_LITERAL - %d\n", intVal);
		break;
	default: errExit(0, "invalid primary"); break;
	}

	puts("\t\t\t\t\tsuccessfully process a primary");
}


