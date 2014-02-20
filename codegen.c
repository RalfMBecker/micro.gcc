/*******************************************************
* codegen.c -          generate IR/Assembly
* Language:            Micro
*
********************************************************/

#include "compiler.h"
#include "codegen.h"

// associative array <id> <-> storage location
struct nlist* symbolTable[HASHSIZE];

void 
createSymbolTable(void){

	// regFreeInit(); // goes into interpreter (?)
	int i;
	for (i = 0; i < HASHSIZE; i++) 
		symbolTable[i] = NULL;
}

// Returns: value (statically allocated; caller to save)
//          NULL for error (handled by caller)
// Operates like C++ lookup of associative arrays
struct nlist*
rwSymbolTable(char* name, char* type){

	static char storage[10];
	static int i = 0;
	struct nlist* p;

	if ( !( NULL == (p = lookup(symbolTable, name)) ) ) // found it
		return p;

	// if not, enter it
	i++;
	sprintf(storage, "temp&%d", i);
	if ( (NULL == (p = install(symbolTable, name, type, NULL, storage)) ) )
		return NULL; 

	return p;
}

// Wrapper around lookup(symbolTable, name)
// Need this separately to be able to both check if present (here),
// and to read out/insert value (rwSymbolTable)
int
checkID_Defined(const char* name){

	if ( (NULL == lookup(symbolTable, name)) )
		return 0;

	return 1;
}

void

void
codegen_DECLARE(char* ID, char* storage){

	printf("Declare: %s, %s\n", ID, storage);
}

void
codegen_ASSIGN(char* dest, char* src){

	printf("Assign: %s, %s\n", dest, src);
}

void 
codegen_BEGIN(int fd, const char* name){

	printf("\ncode generated for %s\n", (fd)?name:"stdin");
	puts("------------------------");
}

void codegen_END(void){	puts("Halt"); }


/***************************************************
* re-locate what follows into interpreter
* translating IR -> assembly
*
****************************************************/

/*

// 2D array: 1st col - all registers; 2nd - free currently?

static int regFree[NUMREGS+1][2]; // to make it 1-based

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
static char*
allocateRegister(char* name){

	// adjust this to handle mem/reg allocation
	static char reg[5];

	int i;
	for (i = 1; i < NUMREGS+1; i++){
		if ( (regFree[i][1] == 1) ){
			sprintf(reg, "$%d", i);
			if ( (NULL == install(symbolTable, name, reg)) )
				errExit(0, "installing %s into symbol table", name); 
			regFree[i][1] = 0;
			return reg;
		}
	}
	// extension: if all registers are used -> call mem allocation fct
	errExit(0, "out of available registers");
	return NULL; // to suppress gcc warning
}

static void
deAllocateRegister(char* name){

	struct nlist* p;
	int tmpNumber;
	char tmpChar[5], errMsg[100];
	if ( ( NULL != (p = lookup(symbolTable, name)) ) ){
		strcpy(tmpChar, &((p->storage)[1]) ); // assumes reg. adjust later
		tmpNumber = atoi(tmpChar);  // we won't free memory within a fct.
		regFree[tmpNumber][1] = 1;  // so probably "if reg" type test enough
	}

	if ( (-1 == undef(symbolTable, name)) ){
		sprintf(errMsg, "%s", "trying to de-allocate non-existing identifier"); 
		strcat(errMsg, " from symbol table");  
		errExit(0, errMsg);
	}
}

*/

/***************************************************
* End: re-location to interpreter
*
****************************************************/
