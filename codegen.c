/*******************************************************
* codegen.c -          generate IR
* Language:            Micro
*
********************************************************/

#include "compiler.h"
#include "codegen.h"

// 2D array: 1st col - all registers; 2nd - free currently?
static int regFree[NUMREGS+1][2]; // to make it 1-based

// associative array <id> <-> register allocated
struct nlist* symbolTable[HASHSIZE];

// start out by making them all available (except 0 == doesn't exist)
static void
regFreeInit(void){

	int i;
	regFree[0][0] = regFree[0][1] = 0;
		for (i = 1; i < NUMREGS+1; i++){
			regFree[i][0] = i; regFree[i][1] = 1;
	}
}

// extension: if more than 12 regs requested, call 'allocateMem()'
// allocation using a static char array: note it will be overwritten
static void
allocateRegister(char* name){

	static char reg[5];

	int i;
	for (i = 1; i < NUMREGS+1; i++){
		if ( (regFree[i][1] == 1) ){
			sprintf(reg, "$%d", i);
			if ( (NULL == install(symbolTable, name, reg)) )
				errExit(0, "installing %s into symbol table", name); 
			regFree[i][1] = 0;
			return;
		}
	}
	// extension: if all registers are used -> call mem allocation fct
	errExit(0, "out of available registers");
}

static void
deAllocateRegister(char* name){

	struct nlist* p;
	int tmpNumber;
	char tmpChar[5], errMsg[100];
	if ( ( NULL != (p = lookup(symbolTable, name)) ) ){
		strcpy(tmpChar, &((p->defn)[1]) );
		tmpNumber = atoi(tmpChar);  
		regFree[tmpNumber][1] = 1;
	}

	if ( (-1 == undef(symbolTable, name)) ){
		sprintf(errMsg, "%s", "trying to de-allocate non-existing identifier"); 
		strcat(errMsg, " from symbol table");  
		errExit(0, errMsg);
	}
}

void 
createSymbolTable(void){

	regFreeInit();

	int i;
	for (i = 0; i < HASHSIZE; i++) 
		symbolTable[i] = NULL;

}

void 
codegen_BEGIN(int fd, char* name){

	printf("\ncode generated for %s\n", (fd)?name:"stdin");
	puts("------------------------");
}

void codegen_END(){	puts("Halt"); }
