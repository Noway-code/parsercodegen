// Jacob Dioso
// Camilo Alvarez

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define constants for maximum lengths and numbers of various elements
#define identMax 11
#define numMax 5
#define numResWords 14
#define numSpecSymbols 13
#define numExtraSymbols 4
#define extraSymbolsLength 3
#define MAX_STRING 512
#define MAX_SYMBOL_TABLE_SIZE 500

// Define enumeration for token types
typedef enum {
	oddsym = 1, identsym, numbersym, plussym, minussym,
	multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
	gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
	periodsym, becomessym, beginsym, endsym, ifsym, thensym,
	whilesym, dosym, constsym, varsym, writesym,
	readsym, procsym, callsym
} token_type;

// Define arrays for special symbols, extra symbols, and reserved words
char specialSymbols[numSpecSymbols] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};
char extraSymbols[numExtraSymbols][extraSymbolsLength] = {":=", "<>", "<=", ">="};
char reservedWords[numResWords][identMax] = {"const", "var", "begin", "end", "if", "fi", "then",
											"while", "do", "read", "write", "odd", "procedure", "call"};

// Function Prototypes for HW2
void printSource(FILE *fileptr);
void createToken(FILE *fileptr);
char peek(FILE *fileptr);
int isReserved(char *lexeme);
int isSymbol(char reader);
int symbolCheck(char *lexeme, int lexCount);
void assignReserved(char *lexeme, int token, int lexCount);
void assignNumber(char *lexeme, int lexCount);
void assignSymbol(char *lexeme, int token, int lexCount);
void printTokenList();

// Function Prototypes for HW3
void emit(char *op, int l, int m);
void ADD_SYMBOLTABLE(char *name, int kind, int val, int level, int addr, int mark);
void PRINT_SYMBOLTABLE();
int SYMBOLTABLECHECK(char *string);
void PROGRAM();
void BLOCK();
void CONST_DECLARATION();
int VAR_DECLARATION();
void PROCEDURE_DECLARATION();
void ADD_PROCEDURETABLE(char *name, int kind, int level, int addr);
void STATEMENT();
void CONDITION();
void EXPRESSION();
void TERM();
void FACTOR();
void PRINT_ASSEMBLY_TO_FILE(int i, FILE *fp);
int PROCTABLECHECK(char *name);

// Define a struct for tokens
typedef struct Tokens {
	int token;
	char number[numMax];
	char identifier[identMax];
} Tokens;
Tokens tokenList[MAX_STRING];
int tokenCount;

// Define a struct for symbols
typedef struct Symbol {
	char name[10]; // name up to 11 chars
	int kind; // const = 1, var = 2, proc = 3
	int val; // number (ASCII value)
	int level; // L level
	int addr; // M address
	int mark; // to indicate unavailable or deleted
} Symbol;
Symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbolIndex = 0;

// Define a struct for instructions
typedef struct Instruction {
	char op[11]; // Operation code
	int l;  // Lexicographical level
	int m;  // Modifier
} Instruction;
Instruction assembly[MAX_SYMBOL_TABLE_SIZE]; // Arbitrary size for the moment
int assemblyIndex = 0;

typedef struct Procedure {
	char name[10]; // name up to 11 chars
	int kind; // const = 1, var = 2, proc = 3
	int level; // L level
	int addr; // M address
} Procedure;
Procedure procedure_table[MAX_SYMBOL_TABLE_SIZE]; // Also arbitrary size for the moment
int procedureIndex = 0;
int procedureBaseIndex = 0;

int level;

// Main function
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("No file included\n");
		exit(1);
	}
	FILE *fileptr = fopen(argv[1], "r");
	printSource(fileptr);
	createToken(fileptr);
	fclose(fileptr);
	PROGRAM();
	return 0;
}

// Function to print out the source file, and rewind the fileptr
void printSource(FILE *fileptr) {
	int reader; // int since EOF is represented as a negative number
	// Print each character in the file until EOF is reached
	while ((reader = getc(fileptr)) != EOF)
		putchar(reader);
	rewind(fileptr);
	return;
}

void createToken(FILE *fileptr) {
	char reader;
	char lexeme[MAX_STRING];
	int token;
	int lexCount = 0;
	int comment = 0;
	while ((reader = fgetc(fileptr)) != EOF) {
		lexeme[0] = '\0'; // resets the lexeme after every loop
		lexCount = 0; // resets the count after every loop
		if (isalpha(reader) && comment == 0) { // Check for keywords and identifiers
			lexeme[lexCount++] = reader;

			while (isalnum(peek(fileptr))) {
				reader = fgetc(fileptr);
				lexeme[lexCount++] = reader;
			}
			lexeme[lexCount] = '\0';

			token = isReserved(lexeme);
			assignReserved(lexeme, token, lexCount);
		}
		else if (isdigit(reader) && comment == 0) { // Check for numbers
			lexeme[lexCount++] = reader;

			while (isdigit(peek(fileptr))) {
				reader = fgetc(fileptr);
				lexeme[lexCount++] = reader;
			}
			lexeme[lexCount] = '\0';

			assignNumber(lexeme, lexCount);
		}
		else if (reader == ' ' || reader == 13 || reader == 10 || reader == 9) { // Ignores spaces and new lines
			continue;
		}
		else if ((isSymbol(reader)) > -1 || comment == 1) { // Check for symbols
			lexeme[lexCount++] = reader;

			while (isSymbol(peek(fileptr)) > -1) {
				reader = fgetc(fileptr);
				lexeme[lexCount++] = reader;
			}
			lexeme[lexCount] = '\0';

			// Logic to ignore comments
			if (comment == 1) {
				if (strcmp(lexeme, "*/") == 0)
					comment = 0;
			}
			else if (strcmp(lexeme, "/*") == 0) {
				comment = 1;
			}
			else if ((strcmp(lexeme, ":") == 0)) { // ":" alone is considered an invalid symbol
				printf("Error: Not a valid symbol\n");
				exit(1);
			}
			else {
				token = symbolCheck(lexeme, lexCount);
				assignSymbol(lexeme, token, lexCount);
			}
		}
		else {
			printf("Error: Not a valid symbol\n");
			exit(1);
		}
	}
	// printTokenList();
}

// Peeks at the character in front of our current reader
char peek(FILE *fileptr) {
	char next_char;

	next_char = fgetc(fileptr);
	ungetc(next_char, fileptr);

	return next_char;
}

// Checks to see if the lexeme matches a reserved word
int isReserved(char *lexeme) {
	int match;

	for (match = 0; match < numResWords; match++) {
		if (strcmp(lexeme, reservedWords[match]) == 0)
			return match;
	}

	return -1;
}

// Checks to see if the lexeme matches the existing symbols
int isSymbol(char reader) {

	// Compares the reader to all the special symbols for a match
	for (int search = 0; search < numSpecSymbols; search++) {
		if (reader == specialSymbols[search])
			return search;
	}

	return -1;
}

// Finds the token of the symbol lexeme
int symbolCheck(char *lexeme, int lexCount) {
	int match;

	// Checks to see if the symbol has 2 characters
	if (lexCount == 2) {
		for (match = 0; match < numExtraSymbols; match++) {
			if (strcmp(lexeme, extraSymbols[match]) == 0)
				return match;
		}
	}

	for (match = 0; match < numSpecSymbols; match++) {
		if (lexeme[0] == specialSymbols[match])
			return match;
	}
}

// Prints the reserved word, and its corresponding token
void assignReserved(char *lexeme, int token, int lexCount) {
	// Checks to make sure the ident isn't too long
	if (lexCount > 11) {
		printf("Error: Name too long\n");
		exit(1);
		return;
	}

	// Checks which reserved word to print out
	switch (token) {
		case -1: // Prints out the identifier
			tokenList[tokenCount].token = identsym;
			strcpy(tokenList[tokenCount].identifier, lexeme);
			tokenCount++;
			break;

		case 0: // const
			tokenList[tokenCount++].token = constsym;
			break;

		case 1: // var
			tokenList[tokenCount++].token = varsym;
			break;

			// Removed case 2: procsym
			// Removed case 3: callsym

		case 2: // begin (previously case 4)
			tokenList[tokenCount++].token = beginsym;
			break;

		case 3: // end
			tokenList[tokenCount++].token = endsym;
			break;

		case 4: // if
			tokenList[tokenCount++].token = ifsym;
			break;

		case 5: // fi
			tokenList[tokenCount++].token = fisym;
			break;

		case 6: // then
			tokenList[tokenCount++].token = thensym;
			break;

			// Removed case 9: elsesym

		case 7: // while
			tokenList[tokenCount++].token = whilesym;
			break;

		case 8: // do
			tokenList[tokenCount++].token = dosym;
			break;

		case 9: // read
			tokenList[tokenCount++].token = readsym;
			break;

		case 10: // write
			tokenList[tokenCount++].token = writesym;
			break;

		case 11: // odd
			tokenList[tokenCount++].token = oddsym;
			break;

		case 12: // procedure
			tokenList[tokenCount++].token = procsym;
			break;

		case 13: // call
			tokenList[tokenCount++].token = callsym;
			break;

		default:
			printf("Something went wrong printing reserved\n");
			exit(1);
			break;
	}
}

// Prints the number and numbersym
void assignNumber(char *lexeme, int lexCount) {
	if (lexCount > numMax) {
		printf("Error: Number too long\n");
		exit(1);
		return;
	}
	tokenList[tokenCount].token = numbersym;
	strcpy(tokenList[tokenCount].number, lexeme);
	tokenCount++;
}

// Prints the symbol and the corresponding token
void assignSymbol(char *lexeme, int token, int lexCount) {
	if (lexCount == 2) {
		switch (token) {
			// :=
			case 0:
				tokenList[tokenCount++].token = becomessym;
				break;

				// <>
			case 1:
				tokenList[tokenCount++].token = neqsym;
				break;

				// <=
			case 2:
				tokenList[tokenCount++].token = leqsym;
				break;

				// >=
			case 3:
				tokenList[tokenCount++].token = geqsym;
				break;

			default:
				printf("Error: Not a valid symbol\n");
				exit(1);
				break;
		}
	}
	else {
		switch (token) {
			// +
			case 0:
				tokenList[tokenCount++].token = plussym;
				break;

				// -
			case 1:
				tokenList[tokenCount++].token = minussym;
				break;

				// *
			case 2:
				tokenList[tokenCount++].token = multsym;
				break;

				// /
			case 3:
				tokenList[tokenCount++].token = slashsym;
				break;

				// (
			case 4:
				tokenList[tokenCount++].token = lparentsym;
				break;

				// )
			case 5:
				tokenList[tokenCount++].token = rparentsym;
				break;

				// =
			case 6:
				tokenList[tokenCount++].token = eqsym;
				break;

				// ,
			case 7:
				tokenList[tokenCount++].token = commasym;
				break;

				// .
			case 8:
				tokenList[tokenCount++].token = periodsym;
				break;

				// <
			case 9:
				tokenList[tokenCount++].token = lessym;
				break;

				// >
			case 10:
				tokenList[tokenCount++].token = gtrsym;
				break;

				// ;
			case 11:
				tokenList[tokenCount++].token = semicolonsym;
				break;

			default:
				printf("Error: Not a valid symbol\n");
				exit(1);
				break;
		}
	}
}

// Prints out the entire token list
void printTokenList() {
	printf("\n\nToken List:\n");
	for (int printer = 0; printer < tokenCount; printer++) {
		if (tokenList[printer].token == identsym) { // Prints out the identifier and the token
			printf("%d ", tokenList[printer].token);
			printf("%s ", tokenList[printer].identifier);
		}
		else if (tokenList[printer].token == numbersym) { // Prints out the number and the token
			printf("%d ", tokenList[printer].token);
			printf("%s ", tokenList[printer].number);
		}
		else
			printf("%d ", tokenList[printer].token);
	}
	printf("\n");

}


// -----------------------------------------------HW3+HW4 Functions-----------------------------------------------
void emit(char *op, int l, int m) {
//    printf("Current token: %d | %d\n", checkCount++, tokenList[tokenCount].token);
	if (assemblyIndex >= MAX_SYMBOL_TABLE_SIZE) { //also arbitrary size
		printf("Error: Code array overflow\n");
		exit(1);
	}

	Instruction inst;
	strncpy(inst.op, op, sizeof(inst.op) - 1);
	inst.op[sizeof(inst.op) - 1] = '\0';
	inst.l = l;  // Set lexicographical level
	inst.m = m;  // Set modifier

	assembly[assemblyIndex++] = inst; // Store instruction and increment index
}

// val is ascii value of the constant
void ADD_SYMBOLTABLE(char *name, int kind, int val, int level, int addr, int mark) {
	if (symbolIndex >= MAX_SYMBOL_TABLE_SIZE) {
		printf("Error: Symbol table overflow\n");
		exit(1);
	}

	Symbol sym;
	strcpy(sym.name, name);
	sym.kind = kind;
	sym.val = val;
	sym.level = level;
	sym.addr = addr;
	sym.mark = mark;

	symbol_table[symbolIndex++] = sym;
}


void PRINT_ASSEMBLY() {
	FILE *fp = fopen("ELF.txt", "w"); // Open file for writing
	if (fp == NULL) {
		printf("Error opening file!\n");
		return;
	}
	printf("Assembly Table:\n");
	printf("%-5s %-10s %-5s %-5s\n", "Line", "OP", "L", "M");

	for (int i = 0; i < assemblyIndex; ++i) {
		printf("%-5d %-10s %-5d %-5d\n", i, assembly[i].op, assembly[i].l, assembly[i].m);
		PRINT_ASSEMBLY_TO_FILE(i, fp);
	}
	fclose(fp); // Close the file
}

void PRINT_ASSEMBLY_TO_FILE(int i, FILE *fp) {
	int op = 0;
	char *str = assembly[i].op;
	if (strcmp(str, "LIT") == 0) op = 1;
	else if (strcmp(str, "OPR") == 0) op = 2;
	else if (strcmp(str, "LOD") == 0) op = 3;
	else if (strcmp(str, "STO") == 0) op = 4;
	else if (strcmp(str, "CAL") == 0) op = 5;
	else if (strcmp(str, "INC") == 0) op = 6;
	else if (strcmp(str, "JMP") == 0) op = 7;
	else if (strcmp(str, "JPC") == 0) op = 8;
	else if (strcmp(str, "SYS") == 0) op = 9;
	fprintf(fp, "%d %d %d\n", op, assembly[i].l, assembly[i].m); // Write loop data to file
}


void PRINT_SYMBOLTABLE() {
	PRINT_ASSEMBLY();

	printf("Symbol Table:\n");
	printf("%-5s %-10s %-5s %-5s %-5s %-5s\n", "Kind", "Name", "Val", "Level", "Addr", "Mark");

	for (int i = 0; i < symbolIndex; i++) {
		symbol_table[i].mark = 1;
		printf("%-5d %-10s %-5d %-5d %-5d %-5d\n",
		       symbol_table[i].kind,
		       symbol_table[i].name,
		       symbol_table[i].val,
		       symbol_table[i].level,
		       symbol_table[i].addr,
		       symbol_table[i].mark);
	}
}


//linear search through symbol table looking at name
//return index if found, -1 if not
int SYMBOLTABLECHECK(char *string) {
	for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
		if (strcmp(symbol_table[i].name, string) == 0) {
			return i;
		}
	}
	return -1;
}

// if we don't end the block with a period, error. after the block, and period, we emit a halt.
void PROGRAM() {
	level = -1;
	tokenCount = 0; // Resets the tokenCount, so it reads from the start of the Token list
	BLOCK();
	if (tokenList[tokenCount].token != periodsym) {
		printf("Error: Program must end with period\n");
		exit(1);
	}
	emit("SYS", 0, 3); // Halt
	PRINT_SYMBOLTABLE();
}

void BLOCK() {
	level++;
	int prevSymbolIndex = symbolIndex;
	int jmpIdx = assemblyIndex;
	emit("JMP", 0, 0); // Placeholder for jump address

	CONST_DECLARATION();
	int numVars = VAR_DECLARATION();
	PROCEDURE_DECLARATION();

	assembly[jmpIdx].m = assemblyIndex * 3;
	emit("INC", 0, 3 + numVars);
	STATEMENT();
	if(level >= 0)
		emit("OPR", 0, 0); // Returns from a procedure
//	symbolIndex = prevSymbolIndex;
	level--;
}

int PROCTABLECHECK(char *name) {
	for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
		if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].kind == 3 && symbol_table[i].mark == 0 ) {
			return i;
		}
	}
	return -1;
}

void PROCEDURE_DECLARATION() {
	while (tokenList[tokenCount].token == procsym) {
		tokenCount++;
		if (tokenList[tokenCount].token != identsym) {
			printf("Error: Procedure keywords must be followed by identifiers\n");
			exit(1);
		}

		if (PROCTABLECHECK(tokenList[tokenCount].identifier) != -1) {
			printf("Error: Procedure name has already been declared\n");
			exit(1);
		}
// Saves the name of the identifier to be used for later
		char identName[identMax];
		strcpy(identName, tokenList[tokenCount].identifier);
		tokenCount++;

		ADD_SYMBOLTABLE(identName, 3, tokenList[tokenCount].token, level, tokenCount, 0); //TODO: Check mark
		if (tokenList[tokenCount].token != semicolonsym) {
			printf("Error: Procedure declarations must be followed by a semicolon\n");
			exit(1);
		}
		tokenCount++;
		BLOCK();
		if (tokenList[tokenCount].token != semicolonsym) {
			printf("Error: Procedure declarations must be followed by a semicolon\n");
			exit(1);
		}
		tokenCount++;
	}
}


void CONST_DECLARATION() {
	if (tokenList[tokenCount].token == constsym) {
		do {
			tokenCount++;
			if (tokenList[tokenCount].token != identsym) {
				printf("Error: Const keywords must be followed by identifiers\n");
				exit(1);
			}

			if (SYMBOLTABLECHECK(tokenList[tokenCount].identifier) != -1) {
				printf("Error: Symbol name has already been declared\n");
				exit(1);
			}

			// Saves the name of the identifier to be used for later
			char identName[identMax];
			strcpy(identName, tokenList[tokenCount].identifier);
			tokenCount++;

			if (tokenList[tokenCount].token != eqsym) {
				printf("Error: Constants must be assigned with equals\n");
				exit(1);
			}

			tokenCount++;

			if (tokenList[tokenCount].token != numbersym) {
				printf("Error: Constants must be assigned an integer value\n");
				exit(1);
			}
			ADD_SYMBOLTABLE(identName, 1, atoi(tokenList[tokenCount].number), 0, 0, 0);
			tokenCount++;
		} while (tokenList[tokenCount].token == commasym);
		if (tokenList[tokenCount].token != semicolonsym) {
			printf("Error: Constant declarations must be followed by a semicolon\n");
			exit(1);
		}
		tokenCount++;
	}
}

int VAR_DECLARATION() {
	int numVars = 0;
	if (tokenList[tokenCount].token == varsym) {
		do {
			numVars++;
			tokenCount++;
			if (tokenList[tokenCount].token != identsym) {
				printf("Error: Var keywords must be followed by identifiers\n");
				exit(1);
			}

			if (SYMBOLTABLECHECK(tokenList[tokenCount].identifier) != -1) {
				printf("Error: Symbol name has already been declared\n");
				exit(1);
			}
			ADD_SYMBOLTABLE(tokenList[tokenCount].identifier, 2, 0, level, 2 + numVars, 0);
			tokenCount++;
		} while (tokenList[tokenCount].token == commasym);
		if (tokenList[tokenCount].token != semicolonsym) {
			printf("Error: Variable declarations must be followed by a semicolon\n");
			exit(1);
		}
		tokenCount++;
	}
	return numVars;
}




void STATEMENT() {
	if (tokenList[tokenCount].token == identsym) {
		int symIdx = SYMBOLTABLECHECK(tokenList[tokenCount].identifier);
		if (symIdx == -1) {
			printf("Error: Undeclared identifier\n");
			exit(1);
		}
		if (symbol_table[symIdx].kind != 2) { // Ensure it's a variable
			printf("Error: Only variable values may be altered\n");
			exit(1);
		}

		tokenCount++;

		if (tokenList[tokenCount].token != becomessym) {
			printf("Error: Assignment statements must use assignment operator\n");
			exit(1);
		}
		tokenCount++;
		EXPRESSION();
		emit("STO", 0, symbol_table[symIdx].addr); // Assuming addr is the address to store the value

	}
	else if (tokenList[tokenCount].token == callsym) {
		tokenCount++;
		if (tokenList[tokenCount].token != identsym) {
			printf("Error: Call keywords must be followed by identifiers\n");
			exit(1);
		}
		char identName[identMax];
		strcpy(identName, tokenList[tokenCount].identifier);

		int symIdx = SYMBOLTABLECHECK(identName);
		//Check Symbol Table

		if (symIdx == -1) {
			printf("Error: Call to undeclared procedure\n");
			exit(1);
		}
//		if (symbol_table[symIdx].kind != 3) {
//			printf("Error: Call to non-procedure\n");
//			exit(1);
//		}
		tokenCount++;
		emit("CAL", 0, (symbol_table[symIdx].addr/3)-1);

		tokenCount++;
	}
	else if (tokenList[tokenCount].token == beginsym) {
		do {
			tokenCount++;
			STATEMENT();
		} while (tokenList[tokenCount].token == semicolonsym);

		if (tokenList[tokenCount].token != endsym) {
			printf("Error: begin must be followed by end\n");
			exit(1);
		}
		if (level >= 1) {
//			emit("OPR", 0, 0); // Returns from a procedure
			level--;
		}
		tokenCount++;

	}
	else if (tokenList[tokenCount].token == ifsym) {
		tokenCount++;
		CONDITION();
		int jpcIdx = assemblyIndex; // Save current index for the jump
		emit("JPC", 0, jpcIdx); // Placeholder for jump address
		if (tokenList[tokenCount].token != thensym) {
			printf("Error: if must be followed by then\n");
			exit(1);
		}
		tokenCount++;
		STATEMENT();
		assembly[jpcIdx].m = assemblyIndex * 3; // Update jump address to next instruction
		if (tokenList[tokenCount].token != fisym) {
			printf("Error: then must be followed by fi\n");
			exit(1);
		}
		tokenCount++;

	}
	else if (tokenList[tokenCount].token == whilesym) {
		int loopStartIdx = assemblyIndex;
		tokenCount++;
		CONDITION();
		if (tokenList[tokenCount++].token != dosym) {
			printf("Error: while must be followed by do\n");
			exit(1);
		}
		int jpcIdx = assemblyIndex; // Save current index for conditional jump
		emit("JPC", 0, 0); // Placeholder for jump out of loop
		STATEMENT();
		emit("JMP", 0, loopStartIdx * 3); // Jump back to start of loop
		assembly[jpcIdx].m = assemblyIndex * 3; // Update the jump-out address

	}
	else if (tokenList[tokenCount].token == readsym) {
		tokenCount++;
		if (tokenList[tokenCount].token != identsym) {
			printf("Error: read keywords must be followed by identifier\n");
			exit(1);
		}
		int symIdx = SYMBOLTABLECHECK(tokenList[tokenCount].identifier);
		if (symIdx == -1) {
			printf("Error: Read into non-variable\n");
			exit(1);
		}
		if (symbol_table[symIdx].kind != 2) {
			printf("Error: Read into undeclared variable\n");
			exit(1);
		}
		tokenCount++;
		emit("SYS", 0, 2);
		emit("STO", level, symbol_table[symIdx].addr);

	}
	else if (tokenList[tokenCount].token == writesym) {
		tokenCount++;
		EXPRESSION();
		emit("SYS", 0, 1);
	}
}

// emits all have placeholders for the moment
void CONDITION() {
	// ODD
	if (tokenList[tokenCount].token == oddsym) { // ODD
		tokenCount++;
		EXPRESSION();
		emit("OPR", 0, 11);
	}
	else {
		EXPRESSION();
		if (tokenList[tokenCount].token == eqsym) // EQL
		{
			tokenCount++;
			EXPRESSION();
			emit("OPR", 0, 5);
		}
		else if (tokenList[tokenCount].token == neqsym) // NEQ
		{
			tokenCount++;
			EXPRESSION();
			emit("OPR", 0, 6);
		}
		else if (tokenList[tokenCount].token == lessym) // LSS
		{
			tokenCount++;
			EXPRESSION();
			emit("OPR", 0, 7);
		}
		else if (tokenList[tokenCount].token == leqsym) // LEQ
		{
			tokenCount++;
			EXPRESSION();
			emit("OPR", 0, 8);
		}
		else if (tokenList[tokenCount].token == gtrsym) // GTR
		{
			tokenCount++;
			EXPRESSION();
			emit("OPR", 0, 9);
		}
		else if (tokenList[tokenCount].token == geqsym) // GEQ
		{
			tokenCount++;
			EXPRESSION();
			emit("OPR", 0, 10);
		}
		else {
			printf("Error: Condition must contain a comparison operator\n");
			exit(1);
		}
	}
}

// we can have a term, or a term followed by a plus or minus, or a term followed by a plus or minus followed by another term.
void EXPRESSION() {
	// First term in the expression
	TERM();

	// Handle subsequent + or - operations followed by terms
	while (tokenList[tokenCount].token == plussym || tokenList[tokenCount].token == minussym) {
		if (tokenList[tokenCount].token == plussym) {
			tokenCount++;
			TERM();
			emit("OPR", 0, 1); // ADD
		}
		else if (tokenList[tokenCount].token == minussym) {
			tokenCount++;
			TERM();
			emit("OPR", 0, 2); // SUB
		}
	}
}

void TERM() {
	FACTOR();
	while (tokenList[tokenCount].token == multsym || tokenList[tokenCount].token == slashsym) {
		if (tokenList[tokenCount].token == multsym) // MUL
		{
			tokenCount++;
			FACTOR();
			emit("OPR", 0, 3);
		}
		else if (tokenList[tokenCount].token == slashsym) // DIV
		{
			tokenCount++;
			FACTOR();
			emit("OPR", 0, 4);
		}
	}
}

// factor can be an identifier, a number, a left parenthesis followed by an expression followed by a right parenthesis, or an error.
void FACTOR() {
	if (tokenList[tokenCount].token == identsym) {
		// Check if identifier exists
		int symIdx = SYMBOLTABLECHECK(tokenList[tokenCount].identifier);
		if (symIdx == -1) {
			printf("Error: Identifier undeclared\n");
			exit(1);
		}
		if (symbol_table[symIdx].kind == 1)
			emit("LIT", 0, symbol_table[symIdx].val);
		else
			emit("LOD", level, symbol_table[symIdx].addr);
		tokenCount++;
	}
	else if (tokenList[tokenCount].token == numbersym) {
		emit("LIT", 0, atoi(tokenList[tokenCount].number));
		tokenCount++;
	}
	else if (tokenList[tokenCount].token == lparentsym) {
		tokenCount++;
		EXPRESSION();
		if (tokenList[tokenCount].token != rparentsym) {
			printf("Error: Right parenthesis must follow left parenthesis\n");
			exit(1);
		}
		tokenCount++;
	}
	else {
		printf("Error: Arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
		exit(1);
	}
}
