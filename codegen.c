/*******************************************************
* codegen.c -          generate IR/Assembly
* Language:            Micro
*
* SSA Philosophy: whenever a new value is created, 
* assign to a new tmp variable - eg, in intermediate 
* steps when evaluating an expression on the RHS of an 
* assignment. The assigment proper can go directly 
* to the target variable (recipient). 
* Example (one way): 
* short a; long b; (assign values); float c = a + b;
* dec a, tmp%1
* dec b, tmp%2 (followed by assignment steps)
* dec c, tmp%3
* promote tmp%4, a, long
* add tmp%5, tmp%4, tmp%2
* assign tmp%3, tmp%5
********************************************************/

#include "compiler.h"
#include "codegen.h"

/***************************************************
* Symbol Table management
*
****************************************************/

// associative array <id> <-> storage location
struct nlist* symbolTable[HASHSIZE];

// promotiona and conversion priority
static int promotionPriority[MAX_TYPES][2];

// also intiializes the table holding promotion priority
void 
createSymbolTable(void){

	// regFreeInit(); // goes into interpreter (?)
	int i;
	for (i = 0; i < HASHSIZE; i++) 
		symbolTable[i] = NULL;

	promotionPriority[1][0] = INTEGER;
	promotionPriority[1][1] = 10;

	promotionPriority[2][0] = LONG;
	promotionPriority[2][1] = 100;

 	promotionPriority[3][0] = FLOAT;
 	promotionPriority[3][1] = 1000;

}

// static allocation of storage so can be safely accessed from caller
static char*
assignNewTemp(){

	static char storage[10];
	static int i = 0;

	i++;
	sprintf(storage, "temp&%d", i);

	return storage;
}

// Returns: pointer to node inserted 
//          (statically allocated; caller to save)
//          if already in table, just return pointer to existing node
// type:    0: called by temp variable - possibly adjust exprType; no
//             new entry
//          1: called for ID for first time. Enter into symbol table
// Error:   returns NULL
struct nlist*
writeSymbolTable(int exprType, char* name, char* type){

	char storage[10];
	struct nlist* p;

	if ( (1 == exprType) ){
		if ( !( NULL == (p = lookup(symbolTable, name)) ) ) // found it
			return p;
		strcpy(	storage, assignNewTemp());
	}

	if ( (NULL == (p = install(symbolTable, name, type, NULL, storage)) ) )
		return NULL;

	return p;
}

// Returns: pointer to node if already in symbol table
// Error:   returns NULL
// Comment: mere wrapper
//          need separately as 'write' version needs type,
//          which for mere read is unknown
struct nlist*
readSymbolTable(const char* name){

	return lookup(symbolTable, name);
}

/***************************************************
* AST object creation helpers
*
****************************************************/

opRecord 
makeOpRec(token tok){

	opRecord res;

	switch(tok){
	case tok_OP_PLUS:
		res.op = PLUS;
		break;
	case tok_OP_MINUS:
		res.op = MINUS;
		break;
	default: 
		errExit(0, "invalid token handed on for processing as operation");
		break;
	}

	return res;
}

exprRecord 
makeIDRec(token tok){

	exprRecord res;
	struct nlist* pList;

	// test should never fail; for safety/style
	if ( (NULL == (pList = readSymbolTable(identifierStr)) ) )
		errExit(0, "attempting to access undefined ID (%s)", identifierStr);
	
	res.kind = EXPR_ID;
	strcpy(res.name, pList->name);
	if ( strcmp(pList->type, "int") ) res.type = INTEGER;
	else if ( strcmp(pList->type, "long") ) res.type = LONG;
	else if ( strcmp(pList->type, "float") ) res.type = FLOAT;
	else errExit(0, "corrupted Symbol Table");	

	return res;
}

exprRecord 
makeLiteralRec(token tok){

	exprRecord res;

	switch(tok){
	case tok_INT_LITERAL:
		res.kind = EXPR_INT_LITERAL;
		res.val_int = intVal;
		res.type = INTEGER;
		break;
	case tok_FLT_LITERAL:
		res.kind = EXPR_FLT_LITERAL;
		res.val_int = fltVal;
		res.type = FLOAT;
		break;
	default:
		errExit(0, "invalid token handed on for processing as literal expression");
		break;
	}

	return res;
}

/***************************************************
* Conversion and promotion for binary expressions
*
****************************************************/

// which, if any, of LHS and RHS needs to be cast?
int
checkCast(exprRecord LHS, exprRecord RHS){

	if ( (LHS.type == RHS.type) )
		return 0;

	if ( promotionPriority[LHS.type] < promotionPriority[RHS.type] )
		return 1;

	return 2;
}

// cast: - always to a temporary in program logic
//       - source might have been literal, but temporary after,
//         so its value no longer matters
exprRecord
castRecord(exprRecord old, int newType){

	exprRecord res;

	res.kind = EXPR_ID;
	res.type = newType;


	// RETHING LOGIC: SHOULD CREATE NEW TEMP OBJECT IN ANY CASE
	// TO DO TOMORROW

	return res;
}



/*
// Goal:      cast 'rec' to type 'target'
// Return:    tmp var of target type
// we need to specify 'source' type so we can refer to the
// proper object in the union member of exprRecord
// No distinction made between promotion and conversion
exprRecord
castRecordType(exprRecord rec, int source, int target){

	if ( (target == rec.type) )
		return rec;

	exprRecord tmp;

	tmp.type = target;
	if ( (EXPR_ID == rec.kind) ){ // no other changes needed
		tmp.kind = rec.kind;
		strcpy(tmp.name, rec.name);
		return tmp;
	}
	if ( (INTEGER == source) && (LONG == target) ){
		tmp.val_long = (long) rec.val_int; return tmp; }
	else if ( (INTEGER == source) && (FLOAT == target) ){
		tmp.val_flt = (float) rec.val_int; return tmp; }
	else if ( (LONG == source) && (FLOAT == target) ){
		tmp.val_flt = (float) rec.val_long; return tmp; }
	else
		errExit(0, "invalid (implicit) cast requested");

	return tmp; // to suppress gcc warning
}
*/


/***************************************************
* End of conversion and promotion
*
****************************************************/

/***************************************************
* Code generation wrappers
*
****************************************************/

void
codegen_DECLARE(char* ID, char* storage){

	printf("Declare: %s, %s\n", ID, storage);
}
// *************CHANGE: hand on object; extract storage/type
// *************PRINT AS: Assign: A, tmp%1 (int)

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
