/*******************************************************
* codegen.c -          generate IR/Assembly
* Language:            Micro
*
* SSA Philosophy: whenever a new value is created, 
* assign to a new tmp variable: 
*    - in intermediate steps when evaluating an expression 
*      on the RHS [of an assignment] because the operand
*      types differ (the expression can be a literal), or 
*    - when assigning LHS = RHS, converting RHS as needed
*      (this covers the case of function template imposed
*       conversions for the return value: int f(int);)  
* Example: 
* short a; long b; (assign values); float c = a + b;
* dec a, tmp%1
* dec b, tmp%2 (followed by assignment steps)
* dec c, tmp%3
* promote tmp%4, a, long
* add tmp%5, tmp%4, tmp%2
* promote tmp%6, tmp%5, float
* assign tmp%3, tmp%6
* TO DO: scope
*        store as struct {char* TU, char* withinTU} scope
*        if global, scope.TU = "all", scope.withinTU = "na"
*        globals: GLOBALS [functDec | varDec]* END_GLOBALS
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

	// define "the usual conventions"
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
// Error:   returns NULL
struct nlist*
writeSymbolTable(int exprType, char* name, int type){

	char storage[10];

	if ( !( NULL == lookup(symbolTable, name)) )// found it
		return NULL;
	strcpy(	storage, assignNewTemp());

	return install(symbolTable, name, type, NULL, storage);

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

// used in declarations
exprRecord 
makeIDRec(token tok){

	exprRecord res;
	struct nlist* pList;

	// test should never fail; for safety/style
	if ( (NULL != (pList = readSymbolTable(identifierStr)) ) )
		errExit(0, "attempting to redefine ID (%s)", identifierStr);
	
	res.kind = EXPR_ID;
	strcpy(res.name, pList->name);
	res.type = pList->type;

	return res;
}

// used in expressions: say, int A was stored in tmp%4.
// make a tmp record tmp%4 with type int, storage tmp%4
exprRecord 
readIDRec(token tok){

	exprRecord res;
	struct nlist* pList;

	// test should never fail; for safety/style
	if ( (NULL == (pList = readSymbolTable(identifierStr)) ) )
		errExit(0, "attempting to access undefined ID (%s)", identifierStr);
	
	res.kind = EXPR_TMP;
	strcpy(res.name, pList->storage);
	res.type = pList->type;

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
* Code generation wrappers
*
****************************************************/

void
codegen_DECLARE(const char* ID, int type, const char* storage){

	char chType[MAX_ID_LEN + 1];

	switch(type){
	case INTEGER: strcpy(chType, "int"); break;
	case LONG: strcpy(chType, "long"); break;
	case FLOAT: strcpy(chType, "float"); break;
	default: errExit(0, "illegal type in declaration"); break;
	}

	printf("Declare: %s %s, %s\n", chType, ID, storage);
}

// *************CHANGE: hand on object; extract storage/type
// *************PRINT AS: Assign: A, tmp%1 (int)
// ********************************************************
void
codegen_ASSIGN(const char* dest, const char* src){

	printf("Assign: %s, %s\n", dest, src);
}

static void
codegen_CONVERT(exprRecord rec, int to){

	char convType[15];

	if ( (LONG == to) && (INTEGER == rec.type) )
		strcpy(convType, "Promote");
	else
		strcpy(convType, "Convert");

	printf("%s: %s, %s, %d\n", convType, assignNewTemp(), rec.name, to);
}

// adjust once we process args
void 
codegen_FUNCTION(const char* name){
	
	printf("Function: %s\n", name);
	puts("----------------------------------------------");
}

void 
codegen_END(const char* name){	

	puts("----------------------------------------------");
	printf("End function: %s\n\n", name);
}

void
codegen_TU(int fd, const char* name){

	puts("----------------------------------------------");
	printf("code generated for %s\n", (fd)?name:"stdin");
	puts("----------------------------------------------\n");
}

/***************************************************
* End code generation wrappers
*
****************************************************/

/***************************************************
* Conversion and promotion for binary expressions
* (infixes), assignments, and return values
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

	codegen_CONVERT(old, newType);

	res.kind = EXPR_TMP;
	res.type = newType;
	strcpy(res.name, assignNewTemp());

	return res;
}

/***************************************************
* End of conversion and promotion
*
****************************************************/

/***************************************************
* Infix, Prefix, and Postfix operations
*
****************************************************/

exprRecord
generateInfix(exprRecord LHS, opRecord op, exprRecord RHS){

	exprRecord res, LHS_adj, RHS_adj;
	int t;

	// casting, if needed
	// castRecord also does IR generation, so must do first
	if ( ( 0 == (t = checkCast(LHS, RHS)) ) )
		res.type = LHS.type; // equal type case
	else{
		if ( (1 == t) ){ // convert LHS
			RHS_adj = RHS;
			LHS_adj = castRecord(LHS, RHS.type);
			res.type = RHS.type;
		}
		else{ // t = 2
			LHS_adj = LHS;
			RHS_adj = castRecord(RHS, LHS.type);
			res.type = LHS.type;
		}
	}

	res.kind = EXPR_TMP;
	strcpy(res.name, assignNewTemp());

	// CALL CODEGENERATION codeGen("Assign", LHS_adj, op, RHS_adj);
	return res;
}

/***************************************************
* End Infix, Prefix, and Postfix operations
*
****************************************************/

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
