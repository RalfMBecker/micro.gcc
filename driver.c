/******************************************************
* driver.c -             driver part of compiler
* Language:              Micro
*
******************************************************/

#include "compiler.h"
#include "lexer.h"
#include "parser.h"

extern char identifierStr[];
extern int numVal;

int 
main(int argc, char* argv[]){

  int fd, openFlags;
	//  int token;

  // *** to do: error checking of argc/argv ***
  openFlags = O_RDONLY;
  fd = open(argv[1], openFlags);
  if (fd == -1)
    errExit(1, " ...open()...");

	systemGoals(fd);


	/*
  while ( ( (token = tokenize(fd)) != -1) ){
      switch(token){
      case tok_EOF:
	puts("token = tok_EOF");
	break;
      case tok_BEGIN:
	puts("token = tok_BEGIN");
	break;
      case tok_END:
	puts("token = tok_END");
	break;
      case tok_READ:
	puts("token = tok_READ");
	break;
      case tok_ID:
	printf("token = tok_ID - variable name: %s\n", identifierStr);
	break;
      case tok_INT_LITERAL:
	printf("token = tok_INT_LITERAL - integer value: %d\n", intVal);
	break;
      case tok_ASSIGN:
	puts("token = tok_ASSIGN");
	break;
      case tok_LPAREN:
	puts("token = tok_LPAREN");
	break;
      case tok_RPAREN:
	puts("token = tok_RPAREN");
	break;
      case tok_SEMICOLON:
	puts("token = tok_SEMICOLON");
	break;
      case tok_COMMA:
	puts("token = tok_COMMA");
	break;
      case tok_OP_PLUS:
	puts("token = tok_OP_PLUS");
	break;
      case tok_OP_MINUS:
	puts("token = tok_OP_MINUS");
	break;

      default:
	fprintf(stderr, 
		"token = ascii; value = %c\n", isprint(token)?token:'?');
	break;
      } // end loop to debug print tokens found
    }

  puts("token = tok_EOF");
  puts("\t\tWe reached end of file; exiting.");
	*/


  if (close(fd) == -1)      
    errExit(1, "...close()...");


  exit(EXIT_SUCCESS);
} 
