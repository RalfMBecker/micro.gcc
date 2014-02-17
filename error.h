/**********************************************
* error.h - include file for error handlers
*
**********************************************/

#ifndef ERROR_H_
#define ERROR_H_

#include <stdarg.h>

#ifndef COMPILER_H_
#include "compiler.h"
#endif

#ifndef MAX_ERR_LEN
#define MAX_ERR_LEN 100
#endif

void errExit(int pError, const char* msg, ...);

#endif
