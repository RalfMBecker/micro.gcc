/*************************************************************
* compiler.h -      include file for commonly used headers
*
*************************************************************/

#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>        // standard i/o functions
#include <sys/types.h>    // type definitions
#include <stdlib.h>       // commonly used lib functions, plus
                          // EXIT_SUCCESS and EXIT_FAILURE
#include <string.h>      // string-handling
#include <unistd.h>       // prototypes for many sys calls
#include <errno.h>        // declare variable errno

#ifndef ERROR_H
#include "error.h"
#endif

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n))

#endif
