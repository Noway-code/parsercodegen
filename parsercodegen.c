// Jacob Dioso
// Camilo Alvarez

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define identMax 11
#define numMax 5
#define numResWords 14
#define numSpecSymbols 13
#define numExtraSymbols 4
#define extraSymbolsLength 3
#define MAX_STRING 512
#define MAX_SYMBOL_TABLE_SIZE 500

typedef enum {
    oddsym = 1, identsym, numbersym, plussym, minussym,
    multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym} token_type;

char specialSymbols[numSpecSymbols] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};

char extraSymbols[numExtraSymbols][extraSymbolsLength] = {":=", "<>", "<=", ">="};

char reservedWords[numResWords][identMax] = {"const", "var", "procedure", "call", "begin", "end", "if",
                                             "fi", "then", "else", "while", "do", "read", "write"};
// Function Prototypes (HW2)
void printSource(FILE* fileptr);
void createToken(FILE* fileptr);
char peek(FILE *fileptr);
int isReserved(char* lexeme);
int isSymbol(char reader);

int symbolCheck(char* lexeme, int lexCount);
void assignReserved(char* lexeme, int token, int lexCount);
void assignNumber(char* lexeme, int lexCount);
void assignSymbol(char* lexeme, int token, int lexCount);
void printTokenList();

// Function Prototypes (HW3)
void emit(char* op, int l, int m);
void ADD_SYMBOLTABLE(char* name, int kind, int val, int level, int addr, int mark);
void PRINT_SYMBOLTABLE();
int SYMBOLTABLECHECK (char* string);
void PROGRAM();
void BLOCK();
void CONST_DECLARATION();
int VAR_DECLARATION();

// Define a struct for tokens
typedef struct Tokens {
    int token;
    char number[numMax];
    char identifier[identMax];
} Tokens;
Tokens tokenList[MAX_STRING];
int tokenCount;

// Symbol struct for HW3.
typedef struct Symbol
{
	char name[10]; // name up to 11 chars
    int kind; // const = 1, var = 2, proc = 3
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} Symbol;
Symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbolIndex = 0;


typedef struct Instruction {
	char* op; // Operation code
	int l;  // Lexicographical level
	int m;  // Modifier
} Instruction;
Instruction assembly[MAX_SYMBOL_TABLE_SIZE]; // Arbitrary size for the moment
int assIndex = 0;

int main(int argc, char **argv) {
    if (argc < 2)
    {
        printf("No file included\n");
        exit(1);
    }
    FILE* fileptr = fopen(argv[1], "r");
//    printSource(fileptr);
    createToken(fileptr);
    fclose(fileptr);
    return 0;
}

// -----------------------------------------------HW2 Functions-----------------------------------------------
// Prints out the source file, and rewinds the fileptr
void printSource(FILE* fileptr) {
	int reader; // int since EOF is represented as a negative number
	// Print each character in the file until EOF is reached
    while ((reader = getc(fileptr)) != EOF)
        putchar(reader);
    rewind(fileptr);
    return;
}

void createToken(FILE* fileptr) {
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
                printf("Error:\t\t\tNot a valid symbol\n");
            }
            else {
                token = symbolCheck(lexeme, lexCount);
                assignSymbol(lexeme, token, lexCount);
            }
        }
        else
        {
            printf("Error:\t\t\tNot a valid symbol\n");
        }
    }
	printTokenList();
}

// Peeks at the character in front of our current reader
char peek(FILE *fileptr) {
    char next_char;

    next_char = fgetc(fileptr);
    ungetc(next_char, fileptr);

    return next_char;
}

// Checks to see if the lexeme matches a reserved word
int isReserved(char* lexeme) {
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
int symbolCheck(char* lexeme, int lexCount) {
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
void assignReserved(char* lexeme, int token, int lexCount) {
    // Checks to make sure the ident isn't too long
    if (lexCount > 11) {
        printf("Error:\t\t\tName too long\n");
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

    case 2: // procedure
        tokenList[tokenCount++].token = procsym;
        break;

    case 3: // call
        tokenList[tokenCount++].token = callsym;
        break;

    case 4: // begin
        tokenList[tokenCount++].token = beginsym;
        break;

    case 5: // end
        tokenList[tokenCount++].token = endsym;
        break;

    case 6: // if
        tokenList[tokenCount++].token = ifsym;
        break;

    case 7: // fi
        tokenList[tokenCount++].token = fisym;
        break;

    case 8: // then
        tokenList[tokenCount++].token = thensym;
        break;

    case 9: // else
        tokenList[tokenCount++].token = elsesym;
        break;

    case 10: // while
        tokenList[tokenCount++].token = whilesym;
        break;

    case 11: // do
        tokenList[tokenCount++].token = dosym;
        break;

    case 12: // read
        tokenList[tokenCount++].token = readsym;
        break;

    case 13: // write
        tokenList[tokenCount++].token = writesym;
        break;

    default:
        printf("Something went wrong printing reserved\n");
        break;
    }

}

// Prints the number and numbersym
void assignNumber(char* lexeme, int lexCount) {
    if (lexCount > numMax) {
        printf("Error:\t\t\tNumber too long\n");
        return;
    }
    tokenList[tokenCount].token = numbersym;
    strcpy(tokenList[tokenCount].number, lexeme);
    tokenCount++;
}

// Prints the symbol and the corresponding token
void assignSymbol(char* lexeme, int token, int lexCount) {
    if (lexCount == 2) {
        switch (token)
        {
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
            printf("Something went wrong while printing the extra symbols\n");
            break;
        }
    }
    else {
        switch (token)
        {
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
            printf("Something went wrong while printing the symbols\n");
            break;
        }
    }
}

// Prints out the entire token list
void printTokenList() {
    printf("Token List: ");
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

}


// -----------------------------------------------HW3 Functions-----------------------------------------------
// Function to emit instructions to the assembly array. Not sure if this is correct, but it's a start.
void emit(char* op, int l, int m) {
	if (assIndex >= MAX_SYMBOL_TABLE_SIZE) { //also arbitrary size
		printf("Error: Code array overflow\n");
		exit(1);
	}

	Instruction inst;
	strcpy(inst.op, op); // Set operation code
	inst.l = l;  // Set lexicographical level
	inst.m = m;  // Set modifier

	assembly[assIndex++] = inst; // Store instruction and increment index
}
// val is ascii value of the constant
void ADD_SYMBOLTABLE(char* name, int kind, int val, int level, int addr, int mark) {
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

void PRINT_SYMBOLTABLE() {
	printf("Symbol Table:\n");
	printf("Kind\tName\tVal\tLevel\tAddr\tMark\n");
	for (int i = 0; i < symbolIndex; i++) {
		if(symbol_table[i].kind == 2)
			printf("%d\t%s\t%d\t%d\t%d\t%d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val, symbol_table[i].level, symbol_table[i].addr, symbol_table[i].mark);
	}
}

//linear search through symbol table looking at name
//return index if found, -1 if not
int SYMBOLTABLECHECK (char* string){
	for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++){
		if (strcmp(symbol_table[i].name, string) == 0){
			return i;
		}
	}
	return -1;
}

// if we don't end the block with a period, error. after the block, and period, we emit a halt.
void PROGRAM() {
    tokenCount = 0; // Resets the tokenCount so it reads from the start of the Token list
    BLOCK();
    if (tokenList[tokenCount++].token != periodsym) {
        printf("Error: Program must end with period\n");
    }
	emit("SYS", 0, 3); // Halt
}

// we can have a const declaration or a var declaration, or both, or neither.
// if we have a var declaration, we emit an INCREASE instruction with the number of variables.
void BLOCK(){
    CONST_DECLARATION();
    int numVars = VAR_DECLARATION();
    emit("INC", 0, 3 + numVars);
//    STATEMENT();
}


// we can have multiple const declarations, each separated by a comma, and ending with a semicolon.
void CONST_DECLARATION() {
    if (tokenList[tokenCount++].token == constsym)
        do {
            if (tokenList[tokenCount++].token != identsym) {
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

            if (tokenList[tokenCount++].token != eqsym) {
                printf("Error: Constants must be assigned with =\n");
                exit(1);
            }

            if (tokenList[tokenCount++].token != numbersym) {
                printf("Error: Constants must be assigned an integer value\n");
                exit(1);
            }
			// Assignment asks for ascii but idk why, that only works for single digit numbers.
	        ADD_SYMBOLTABLE(identName, 1, atoi(tokenList[tokenCount].number), 0, 0, 0);
			tokenCount++;
        } while (tokenList[tokenCount++].token == commasym);
        if (tokenList[tokenCount++].token != semicolonsym) {
            printf("Error: Constant declarations must be followed by a semicolon\n");
            exit(1);
        }
}



// we can have multiple var declarations, each separated by a comma, and ending with a semicolon. returns number of variables
// im sussed out by all the tokenList[tokenCount++]. i think we are producing off by one errors.
// symbol table is likely not being updated with the correct values.
int VAR_DECLARATION() {
    int numVars = 0;
    if (tokenList[tokenCount++].token == varsym)
        do {
            numVars++;
            if (tokenList[tokenCount++].token != identsym){
                printf("Error: Const keywords must be followed by identifiers\n");
                exit(1);
            }

            if (SYMBOLTABLECHECK(tokenList[tokenCount].identifier) != -1) {
                printf("Error: Symbol name has already been declared\n");
                exit(1);
            }
//			?
	        ADD_SYMBOLTABLE(tokenList[tokenCount].identifier, 2, 0, 0, 2 + numVars, 0);
        } while (tokenList[tokenCount++].token == commasym);
        if (tokenList[tokenCount++].token != semicolonsym) {
            printf("Error: Variable declarations must be followed by a semicolon\n");
            exit(1);
        }
    return numVars;
}


// we can have a statement, or a begin statement, or an if statement, or a while statement, or a read statement, or a write statement.
//void STATEMENT() {
//	if (tokenList[tokenCount].token == identsym) {
//		int symIdx = SYMBOLTABLECHECK(tokenList[tokenCount++].identifier);
//		if (symIdx == -1) {
//			printf("Error: Identifier not found\n");
//			exit(1);
//		}
//		if (symbol_table[symIdx].kind != 2) { // Ensure it's a variable
//			printf("Error: Assignment to non-variable\n");
//			exit(1);
//		}
//		if (tokenList[tokenCount++].token != becomessym) {
//			printf("Error: Expected := for assignment\n");
//			exit(1);
//		}
//		EXPRESSION();
//		emit("STO", 0, symbol_table[symIdx].addr); // Assuming addr is the address to store the value
//	} else if (tokenList[tokenCount].token == beginsym) {
//		tokenCount++;
//		STATEMENT();
//		while (tokenList[tokenCount].token == semicolonsym) {
//			tokenCount++;
//			STATEMENT();
//		}
//		if (tokenList[tokenCount++].token != endsym) {
//			printf("Error: End of begin block expected\n");
//			exit(1);
//		}
//	} else if (tokenList[tokenCount].token == ifsym) {
//		tokenCount++;
//		CONDITION();
//		int jpcIdx = assIndex; // Save current index for the jump
//		emit("JPC", 0, 0); // Placeholder for jump address
//		if (tokenList[tokenCount++].token != thensym) {
//			printf("Error: then expected\n");
//			exit(1);
//		}
//		STATEMENT();
//		assembly[jpcIdx].m = assIndex; // Update jump address to next instruction
//	} else if (tokenList[tokenCount].token == whilesym) {
//		int loopStartIdx = assIndex;
//		tokenCount++;
//		CONDITION();
//		int jpcIdx = assIndex; // Save current index for conditional jump
//		emit("JPC", 0, 0); // Placeholder for jump out of loop
//		if (tokenList[tokenCount++].token != dosym) {
//			printf("Error: do expected\n");
//			exit(1);
//		}
//		STATEMENT();
//		emit("JMP", 0, loopStartIdx); // Jump back to start of loop
//		assembly[jpcIdx].m = assIndex; // Update the jump-out address
//	} else if (tokenList[tokenCount].token == readsym) {
//		tokenCount++;
//		if (tokenList[tokenCount].token != identsym) {
//			printf("Error: Identifier expected after read\n");
//			exit(1);
//		}
//		int symIdx = SYMBOLTABLECHECK(tokenList[tokenCount++].identifier);
//		if (symIdx == -1 || symbol_table[symIdx].kind != 2) {
//			printf("Error: Read into non-variable or undeclared variable\n");
//			exit(1);
//		}
//		emit("READ", 0, symbol_table[symIdx].addr);
//	} else if (tokenList[tokenCount].token == writesym) {
//		tokenCount++;
//		EXPRESSION();
//		emit("WRITE", 0, 0);
//	}
//}
/*
// we can have an odd condition, or an expression, or an expression followed by an equal, not equal, less than, less than or equal, greater than, or greater than or equal condition.
CONDITION
    if token == oddsym
        get next token
        EXPRESSION
        emit ODD
    else
        EXPRESSION
        if token == eqlsym
            get next token
            EXPRESSION
            emit EQL
        else if token == neqsym
            get next token
            EXPRESSION
            emit NEQ
        else if token == lessym
            get next token
            EXPRESSION
            emit LSS
        else if token == leqsym
            get next token
            EXPRESSION
            emit LEQ
        else if token == gtrsym
            get next token
            EXPRESSION
            emit GTR
        else if token == geqsym
            get next token
            EXPRESSION
            emit GEQ
        else
            error
*/

/*
// we can have a term, or a term followed by a plus or minus, or a term followed by a plus or minus followed by another term.
EXPRESSION (HINT: modify it to match the grammar)
    if token == minussym
        get next token
        TERM
        emit NEG
        while token == plussym || token == minussym
            if token == plussym
                get next token
                TERM
                emit ADD
            else
                get next token
                TERM
                emit SUB
    else
        if token == plussym
            get next token
            TERM
            while token == plussym || token == minussym
                if token == plussym
                    get next token
                    TERM
                    emit ADD
                else
                    get next token
                    TERM
                    emit SUB
*/

// term can be a factor, or a factor followed by a multiply, divide, or mod, or a factor followed by a multiply, divide, or mod followed by another factor.
/*
TERM
    FACTOR
    while token == multsym || token == slashsym || token == modsym
        if token == multsym
            get next token
            FACTOR
            emit MUL
        else if token == slashsym
            get next token
            FACTOR
            emit DIV
        else
            get next token
            FACTOR
            emit MOD
*/

/*
// factor can be an identifier, a number, a left parenthesis followed by an expression followed by a right parenthesis, or an error.
FACTOR
    if token == identsym
        symIdx = SYMBOLTABLECHECK (token)
        if symIdx == -1
            error
        if table[symIdx].kind == 1 (const)
            emit LIT (M = table[symIdx].Value)
        else (var)
            emit LOD (M = table[symIdx].addr)
        get next token
    else if token == numbersym
        emit LIT
        get next token
    else if token == lparentsym
        get next token
        EXPRESSION
        if token != rparentsym
            error
        get next token
    else
        error
*/
