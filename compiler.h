/*************************************************************
* compiler.h -      include file for commonly used headers
*
*************************************************************/

#ifndef COMPILER_H_
#define COMPILER_H_

#include <stdio.h>        // standard i/o functions
#include <ctype.h>
#include <stdlib.h>       // commonly used lib functions, plus
                          // EXIT_SUCCESS and EXIT_FAILURE
#include <string.h>      // string-handling
#include <sys/types.h>    // type definitions
#include <unistd.h>       // prototypes for many sys calls
#include <errno.h>        // declare variable errno
#include <fcntl.h>        // open(), O_RDONLY

#ifndef ERROR_H_
#include "error.h"
#endif

#define MAX_ID_LEN 32
#define MAX_INT_LEN 12

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n))

#endif
