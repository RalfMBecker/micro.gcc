/****************************************************************
* lexer.c -            lexer part of compiler
* Language:            Micro
*
* Note:                - so far, we are LL(1), and curTok will do
*                      - max size identifiers: MAX_ID_LEN (32)
*                      - max digits literals: MAX_LIT_LEN (20)
****************************************************************/

#include "compiler.h"
#include "lexer.h"

static void
next_char(int fd, int* last_char)
{
    ssize_t numRead;

    numRead = read(fd, last_char, 1);
    if (numRead == -1)
	errExit(1, "...int read()...");
    else if (numRead == 0)
	*last_char = EOF;
}

// validity check of possible identifier
// return value:    -1: unspecified error
//                   tok_ID: not a reserved keyword
//                   tok_xxx: keyword xxx, as indexed by tok_xxx
static token 
check_reserved(char* word)
{
    if ( (0 == strcmp(word, "begin")) )
	return tok_BEGIN;
    if ( (0 == strcmp(word, "end")) )
	return tok_END;
    if ( (0 == strcmp(word, "read")) )
	return tok_READ;
    if ( (0 == strcmp(word, "write")) )
	return tok_WRITE;
    if ( (0 == strcmp(word, "int")) )
	return tok_DEC_INT;
    if ( (0 == strcmp(word, "long")) )
	return tok_DEC_LONG;
    if ( (0 == strcmp(word, "float")) )
	return tok_DEC_FLT;

    return tok_ID; // not a reserved keyword (tok_xxx is numbered 1 and higher)
}

// note how last_char look-ahead invariant is preserved by each possible
// sub case (where it is not explicitly invoked, a comment explains why)
int 
tokenize(int fd)
{
    static int last_char = ' ';
    int i;
    char numStr[MAX_LIT_LEN+1];

    while (isspace(last_char))
	next_char(fd, &last_char);

    // case identifier ([a-zA-z][a-zA-z0-9_]*)
    // returns tok_BEGIN, tok_END, tok_READ, tok_WRITE, tok_ID, respectively
    i = 0;
    if ( isalpha(last_char) ){
	while ( isalnum(last_char) || ('_' == last_char) ){
	    if ( (MAX_ID_LEN == i) ){
		identifierStr[0] = '\0'; // keep in clean slate
		errExit(0, "...invalid lenght of identifier: %d (%d allowed)...", i, MAX_ID_LEN);
	    }
	    identifierStr[i++] = last_char;
	    next_char(fd, &last_char);
	}
	identifierStr[i] = '\0'; // note: last_char already looks ahead as we
	                         //       read one char ahead

	return check_reserved(identifierStr);
    }

    // numeric literal
    i = 0;
    if ( isdigit(last_char) ){
	while ( isdigit(last_char) ){
	    if ( (MAX_LIT_LEN == i) ){
		numStr[0] = '\0'; // clean up
		errExit(0, "...invalid number of digits of int type: %d (%d allowed)", i, MAX_LIT_LEN);
	    }
	    numStr[i++] = last_char;
	    next_char(fd, &last_char);
	}

	// case: int or long. default to int; handle promotion elsewhere
	if ( '.' != last_char){
	    numStr[i] = '\0';
      
	    errno = 0;   // as 0 can be returned legitimetely
	    intVal = atol(numStr);
	    if (errno != 0) // overflow? 
		errExit(1, "...atoi(%s)...",  numStr);

	    return tok_INT_LITERAL;
	}

	// case: float
	numStr[i++] = last_char;
	next_char(fd, &last_char);

	while ( isdigit(last_char) ){
	    if ( (MAX_LIT_LEN == i) ){
		numStr[0] = '\0'; // clean up
		errExit(0, "...invalid number of digits of int type: %d (%d allowed)", i, MAX_LIT_LEN);
	    }
	    numStr[i++] = last_char;
	    next_char(fd, &last_char);
	}

	numStr[i] = '\0';
	errno = 0;   // as 0 can be returned legitimetely
	fltVal = atof(numStr);
	if (errno != 0) // overflow? 
	    errExit(1, "...atoi(%s)...",  numStr);

	return tok_FLT_LITERAL;
    } //end case numeric literal

    // assignment
    if ( (':' == last_char) ){
	next_char(fd, &last_char);
	if ( ('=' == last_char) ){
	    next_char(fd, &last_char);
	    return tok_ASSIGN;
	}
	else
	    errExit(0, "...invalid syntax: : not followed by =");
    }

    // single token literals following (also EOF)
    switch(last_char){
    case '(': next_char(fd, &last_char); return tok_LPAREN; break;
    case ')': next_char(fd, &last_char); return tok_RPAREN; break;
    case ';': next_char(fd, &last_char); return tok_SEMICOLON; break;
    case ',': next_char(fd, &last_char); return tok_COMMA; break;
    case '+': next_char(fd, &last_char); return tok_OP_PLUS; break;
    case '*': next_char(fd, &last_char); return tok_OP_MUL; break;
    case '/': next_char(fd, &last_char); return tok_OP_DIV; break;	
    default: break;
    }

    // case comment and op_minus
    if (last_char == '-'){
	next_char(fd, &last_char);
	if (last_char == '-'){ // the lookahead check already re-fille last_char
	    while ( (last_char != '\n') && (last_char != EOF) )
		next_char(fd, &last_char);
	    if ( (last_char == '\n') )
		return tokenize(fd);
	}
	else // see above comment: look-ahead invariant in last_char already ok
	    return tok_OP_MINUS;
    }

    // case EOF
    if ( (tok_EOF == last_char) )
	return tok_EOF;

    // if we come here, we fell through: illegal terminal/token
    errExit(0, "...illegal token %c", last_char);

    return -1; // to suppress gcc no return value warning
}
