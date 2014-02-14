/******************************************************
* driver.c -             driver part of compiler
* Language:              Micro
*
******************************************************/

#include "compiler.h"
#include "lexer.h"

int main(int argc, char* argv[]){

  int fd, openFlags;
  int token;

  // *** to do: error checking of argc/argv ***
  openFlags = O_RDONLY;
  fd = open(argv[1], openFlags);
  if (fd == -1)
    errExit(1, " ...open()...");

  while ( (token = tokenize()) ){
      switch(token){
      case tok_EOF:
	puts("token = tok_EOF");;
	break;
      
      default:
	fprintf(stderr, 
		"token = ascii; value = %c\n", isprint(token)?token:'?');
	break;
      } // end loop to debug print tokens found
    }
  if (close(fd) == -1)      
    errExit(1, "...close()...");

  exit(EXIT_SUCCESS);
} 
