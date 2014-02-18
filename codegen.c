/*******************************************************
* codegen.c -          generate IR
* Language:            Micro
*
********************************************************/

#include "compiler.h"

void codegen_BEGIN(int fd, char* name){

	printf("\ncode generated for %s\n", (fd)?name:"stdin");
	puts("------------------------");
} 

void codegen_END(){	puts("Halt"); }
