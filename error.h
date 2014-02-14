/**********************************************
* error.h - include file for error handlers
*
**********************************************/

#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>

#ifndef COMPILER_H
#include "compiler.h"
#endif

#ifndef MAX_ERR_LEN
#define MAX_ERR_LEN 100
#endif

void errExit(int pError, const char* msg, ...);

#endif
