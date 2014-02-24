/*************************************************************
* driver.c -             driver part of compiler
* Language:              Micro
*
* Driver handles:        Looping over a series of statements
*                        In Micro, this includes processing
*                        also BEGIN, END, and EOF
*************************************************************/

#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

extern char identifierStr[];
extern int numVal;

int 
main(int argc, char* argv[]){

  int fd, openFlags, endSeen;
	endSeen = 0;

	if (argc > 1){
		openFlags = O_RDONLY;
		fd = open(argv[1], openFlags);
		if (fd == -1)
			errExit(1, " ...open()...");
	}
	else
		fd = 0;

	createSymbolTable();

	codegen_TU(fd, (argc >1)?argv[1]:"");

	// needs to be redone when doing scope
	match(1, fd, tok_BEGIN, 0);
	codegen_FUNCTION("begin");

	while ( getNextToken(fd) != EOF){
		if ( (curTok == tok_END) ) { endSeen = 1; break;}
		if ( (curTok == tok_SEMICOLON) ) continue; // allow empty statement
		     // Note: consider letting regular descent handle it - it should
		Statement(fd, 0);
	}

	if (endSeen)  // make sure we saw END before EOF
		codegen_END("begin");
	else
		errExit(0, "syntax error: program must end with token END");

	if (argc > 1)
		if (close(fd) == -1)      
			errExit(1, "...close()...");

  exit(EXIT_SUCCESS);
} 
