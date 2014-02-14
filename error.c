/**********************************************************
* error.c -    prototypes for error handling functions
*
**********************************************************/

#include <stdarg.h>
#include "compiler.h"
#include "ename.c.inc"

#ifdef __GNUC__
__attribute__ ((__noreturn__)) // in case of being called from
#endif                        // non-void function
void
errExit(int pError, const char* format, ...){

  va_list arglist;
  char usrMsg[MAX_ERR_LEN+1], errMsg[MAX_ERR_LEN+1], str[MAX_ERR_LEN+1];
  char err[MAX_ERR_LEN];

  va_start(arglist, format);
  vsnprintf(usrMsg, MAX_ERR_LEN, format, arglist);
  va_end(arglist);

  if (pError){
    if ( (errno > 0) && (errno < MAX_ENAME) )
      strcpy(err, ename[errno]);
    else
      strcpy(err, "???");  
    snprintf(errMsg, MAX_ERR_LEN, "%s %s", err, strerror(errno));
  }
  else
    strcpy(errMsg, " ");

  // could be too long for str; ignored
  snprintf(str, MAX_ERR_LEN, "ERROR: %s %s\n", usrMsg, errMsg); 

  fflush(stdout);
  fputs(str, stderr);
  fflush(stderr);

  exit(EXIT_FAILURE);
}
