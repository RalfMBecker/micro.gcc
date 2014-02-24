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
* int a; long b; (assign values); float c = a + b;
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

// associative array <name> <-> <type> <scope> <storage> 
struct nlist* symbolTable[HASHSIZE];

// promotion and conversion priority
static int promotionPriority[MAX_TYPES][2];

// also intiializes the table holding promotion priority
void 
createSymbolTable(void){

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

// static allocation of storage so caller can safely access
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
// Error:   returns NULL (attempt to redefine variable/function)
struct nlist*
writeSymbolTable(int exprType, char* name, int type, char* scope){

	char storage[MAGIC];

	if ( !( NULL == lookup(symbolTable, name)) ) // can't redefine 
		return NULL;
	strcpy(	storage, assignNewTemp());

	return install(symbolTable, name, type, NULL, storage);
}

// Returns: pointer to node if already in symbol table
// Error:   returns NULL
// Comment: mere wrapper
//          need separately as 'write' version needs type,
//          which for mere read might be unknown
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
		errExit(0, "invalid token in binary expression");
		break;
	}

	return res;
}

// used in declarations
exprRecord 
makeIDRec(const char* name){

	exprRecord res;
	struct nlist* pList;

	if ( (NULL == (pList = readSymbolTable(name)) ) )
		errExit(0, "attempting to access undeclared ID (%s)", identifierStr);

	res.kind = EXPR_ID;
	strcpy(res.name, pList->name);
	res.type = pList->type;

	return res;
}

// assumes call when intVal/fltVal contains the literal
exprRecord 
makeLiteralRec(token tok){

	exprRecord res;

	switch(tok){
	case tok_INT_LITERAL:
		res.kind = EXPR_INT_LITERAL;
		res.val_int = intVal;
		res.type = INTEGER; // need to pick a default: if we see an int type,
		break;              // consider it to be an int (not a long, say)
	case tok_FLT_LITERAL:
		res.kind = EXPR_FLT_LITERAL;
		res.val_flt = fltVal;
		res.type = FLOAT; // same about defaults as above, if also double exists
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
codegen_DECLARE(const exprRecord rec){

	char typeStr[MAX_TOK_LEN], commandStr[MAGIC];
  struct nlist* recNL;
	int t;

	if ( (NULL == (recNL = readSymbolTable(rec.name)) ) )
		errExit(0, "cannot find name in Symbol Table (%s)", rec.name);

	if ( (INTEGER == (t = recNL->type)) )
		strcpy(typeStr, "int");
	else if ( (LONG == t) )
		strcpy(typeStr, "long");
	else if ( (FLOAT == t) )
		strcpy(typeStr, "float");
	else
		errExit(0, "in ST, invalid type entry (%d) for ID (%s)", t, rec.name);  

	sprintf(commandStr, "%s", "Declare:");
	printf("%-8s %s, %s, %s\n", commandStr, rec.name, recNL->storage, typeStr);
}


static char*
storageFromName(const char* name){

	struct nlist* pNL;

	if ( (NULL == (pNL = readSymbolTable(name)) ) )
		errExit(0, "unable to find (%s) in symbol table", name);

	return pNL->storage;
}

// int kind: 0 - assignment; 1 - copy assignment
// LHS should be be a fake tmpExpr (0) (name == storage), or EXPR_ID (1) 
// RHS could be anything
void
codegen_ASSIGN(const exprRecord LHS, const exprRecord RHS, int kind){

	char strL[MAGIC], strR[MAGIC], commandStr[MAGIC];

	if ( (0 == kind) )
		strcpy(strL, LHS.name);
	else if ( (1 == kind) )
		strcpy(strL, storageFromName(LHS.name));
	else
		errExit(0, "invalid call of codegen_Assign (type = %d)", kind);

	if ( (EXPR_INT_LITERAL == RHS.kind) || (EXPR_LONG_LITERAL == RHS.kind) )
		sprintf(strR, "%ld", RHS.val_int);
	else if ( (EXPR_FLT_LITERAL == RHS.kind) )
		sprintf(strR, "%g", RHS.val_flt);
	else if ( (EXPR_ID == RHS.kind) )
		sprintf(strR, "%s", storageFromName(RHS.name));
	else if ( (EXPR_TMP == RHS.kind) ) // name == storage location
		sprintf(strR, "%s", RHS.name);
	else
		errExit(0, "invalid assignment");

	sprintf(commandStr, "%s", "Assign:");
	printf("%-8s %s, %s\n", commandStr, strL, strR);
}

// res will be EXPR_TMP (name == storage); LHS/RHS could be anything
static void
codegen_INFIX(const exprRecord res, const exprRecord LHS, 
							const opRecord op, const exprRecord RHS){

	// should be max MAX_LIT_LEN and MAX_ID_LEN
	char opStr[MAGIC], strL[MAGIC], strR[MAGIC];
	int tL, tR;

	if ( (PLUS == op.op) ) 
		strcpy(opStr, "Add:");
	else if ( (MINUS == op.op) )
		strcpy(opStr, "Sub:");
	else
		errExit(0, "illegal operation in infix expression");

	// prepare what to print depending on expr type of LHS and RHS
	if ( (EXPR_ID == (tL = LHS.kind)) )
		strcpy(strL, storageFromName(LHS.name));
	else if ( (EXPR_TMP == tL) )
		strcpy(strL, LHS.name);  // name == storage
	else if ( (EXPR_INT_LITERAL == tL) || (EXPR_LONG_LITERAL == tL) )
		sprintf(strL, "%ld", LHS.val_int);
	else if ( (EXPR_FLT_LITERAL == tL) )
		sprintf(strL, "%g", LHS.val_flt);
	else
		errExit(0, "invalid type (%d)", tL);

	if ( (EXPR_ID == (tR = RHS.kind)) )
		strcpy(strR, storageFromName(RHS.name));
	else if ( (EXPR_TMP == tR) )
		strcpy(strR, RHS.name);  // name == storage
	else if ( (EXPR_INT_LITERAL == tR) || (EXPR_LONG_LITERAL == tR) )
		sprintf(strR, "%ld", RHS.val_int);
	else if ( (EXPR_FLT_LITERAL == tR) )
		sprintf(strR, "%g", RHS.val_flt);
	else
		errExit(0, "invalid type (%d)", tR);

	// for res, name == storage
	printf("%-8s %s, %s, %s\n", opStr, res.name, strL, strR);		
}

static char*
typeToStr(int type){

	static char typeStr[MAX_TOK_LEN + 1];

	switch(type){
	case INTEGER: strcpy(typeStr, "int"); break;
	case LONG: strcpy(typeStr, "long"); break;
	case FLOAT: strcpy(typeStr, "float"); break;
	default: errExit(0, "invalid type %d", type);
	}

	return typeStr;
}

// at call, dest should be an EXPR_TMP; from could be any type of expr
static void
codegen_CONVERT(const exprRecord dest, const exprRecord from, int to){

	char convType[MAGIC], typeStr[MAX_TOK_LEN + 1], fromStr[MAGIC];
	int t;

	if ( (LONG == to) && (INTEGER == from.type) )
		strcpy(convType, "Promote:");
	else
		strcpy(convType, "Convert:");

	strcpy(typeStr, typeToStr(to));

	if ( (EXPR_ID == (t = from.kind) ) )
		sprintf(fromStr, "%s", storageFromName(from.name));
	else if ( (EXPR_TMP == t) )
		sprintf(fromStr, "%s", from.name); // name == storage for tmp
	else if ( (EXPR_INT_LITERAL == t) || (EXPR_LONG_LITERAL == t) )
		sprintf(fromStr, "%ld", from.val_int);
	else if ( (EXPR_FLT_LITERAL == t) )
		sprintf(fromStr, "%g", from.val_flt);
	else
		errExit(0, "in conversion, invalid expression type (%d)", from.kind);

	printf("%-8s %s, %s, %s\n", convType, dest.name, fromStr, typeStr);
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
checkCast(const exprRecord LHS, const exprRecord RHS){

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
castRecord(const exprRecord old, int newType){

	exprRecord res;

	res.kind = EXPR_TMP;
	res.type = newType;
	strcpy(res.name, assignNewTemp());

	codegen_CONVERT(res, old, newType);

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

	exprRecord res;
	int t;

	// cast if needed
	if ( (1 == (t = checkCast(LHS, RHS)) ) ){
		LHS = castRecord(LHS, RHS.type);
		res.type = RHS.type;
	}
	else if ( (2 == t) ){
		RHS = castRecord(RHS, LHS.type);
		res.type = LHS.type;
	}

	res.kind = EXPR_TMP;
	strcpy(res.name, assignNewTemp());

	codegen_INFIX(res, LHS, op, RHS);

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
