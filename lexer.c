/****************************************************************
* lexer.c -            lexer part of compiler
* Language:            Micro
*
* Note:                - for Micro, 1 char look-ahead is enough
*                      - Micro code ungetc(); we look ahead
*                        and store our look-ahead in last_char
*                      - max size identifiers: 32 bytes
*                      - max digits int: 12 (I imposed)
****************************************************************/

#include "compiler.h"
#include "lexer.h"

static void
next_char(int fd, int* last_char){

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
static token check_reserved(char* word){

  if ( (strcmp(word, "BEGIN") == 0) )
    return tok_BEGIN;
  if ( (strcmp(word, "END") == 0) )
    return tok_END;
  if ( (strcmp(word, "READ") == 0) )
    return tok_READ;
  if ( (strcmp(word, "WRITE") == 0) )
    return tok_WRITE;

  return tok_ID; // not a reserved keyword (tok_xxx is numbered 1 and higher)
}

// note how last_char look-ahead invariant is preserved by each possible
// sub case (where it is not explicitly invoked, a comment explains why)
token tokenize(int fd){

  static int last_char = ' ';
  int i;
  char numStr[MAX_INT_LEN+1];

  while (isspace(last_char))
    next_char(fd, &last_char);

  // case identifier ([a-zA-z][a-zA-z0-9_]*)
  // returns tok_BEGIN, tok_END, tok_READ, tok_WRITE, tok_ID, respectively
  i = 0;
  if ( isalpha(last_char) ){
    while ( isalnum(last_char) || ('_' == last_char) ){
      if ( (MAX_ID_LEN == i) ){
	identifierStr[0] = '\0'; // keep in clean slate
	errExit(0, "...invalid lenght of identifier: %d (%d allowed)...", 
		i, MAX_ID_LEN);
      }
      identifierStr[i++] = last_char;
      next_char(fd, &last_char);
    }
    identifierStr[i] = '\0'; // note: last_char already looks ahead as we
                             //       read one char ahead
    //puts("...processing an identifier/reserved key word...");

    //    next_char(fd, &last_char);
    return check_reserved(identifierStr);
  }

  // integer literal (recall: only valid numerical type)
  i = 0;
  if ( isdigit(last_char) ){
    while ( isdigit(last_char) ){
      if ( (MAX_INT_LEN == i) ){
	numStr[0] = '\0'; // clean up
	errExit(0, "...invalid number of digits of int type: %d (%d allowed)",
		i, MAX_INT_LEN);
      }
      numStr[i++] = last_char;
      next_char(fd, &last_char);
    }
    numStr[i] = '\0';
      
    errno = 0;   // as 0 can be returned legitimetely
    intVal = atoi(numStr);
    if (errno != 0) // overflow? 
      errExit(1, "...atoi(%s)...",  numStr);
    //puts("...processing an integer literal...");

    next_char(fd, &last_char);
    return tok_INT_LITERAL;
  }

  // assignment
  if ( (':' == last_char) ){
    next_char(fd, &last_char);
    if ( ('=' == last_char) ){
      //puts("...processing an assignment...");
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
  
  }

  // case comment and op_minus
  if (last_char == '-'){
    next_char(fd, &last_char);
    if (last_char == '-'){ // the look-ahead check already re-fille last_char
      puts("...processing a comment...");
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
