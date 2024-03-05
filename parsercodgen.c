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
    skipsym = 1, identsym, numbersym, plussym, minussym,
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
// Insert here

// Token sturct for HW2
typedef struct Tokens {
    int token;
    char number[numMax];
    char identifier[identMax];
} Tokens;
Tokens tokenList[MAX_STRING];
int tokenCount;

// Symbol struct for HW3
typedef struct Symbol
{
    int kind; // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark // to indicate unavailable or deleted
} Symbol;
symbol_table[MAX_SYMBOL_TABLE_SIZE];

int main(int argc, char **argv) {
    if (argc < 2)
    {
        printf("No file included\n");
        exit(1);
    }
    FILE* fileptr = fopen(argv[1], "r");
    printSource(fileptr);
    createToken(fileptr);
    fclose(fileptr);
    return 0;
}

// Prints out the source file, and rewinds the fileptr
void printSource(FILE* fileptr) {
    printf("Source Program:\n");

    int reader; // int since EOF is represented as a negative numbeer
    while ((reader = getc(fileptr)) != EOF)
        putchar(reader);
    rewind(fileptr);
    printf("\n\n\n");
    return;
}

void createToken(FILE* fileptr) {
    printf("Lexeme Table:\n"
           "lexeme\t\t\ttoken type\n");

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
    printf("\nToken List:\n");
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

/*
SYMBOLTABLECHECK (string)
    linear search through symbol table looking at name
    return index if found, -1 if not

PROGRAM
    BLOCK
    if token != periodsym
        error
    emit HALT

BLOCK
    CONST-DECLARATION
    numVars = VAR-DECLARATION
    emit INC (M = 3 + numVars)
    STATEMENT

CONST-DECLARATION
    if token == const
        do
            get next token
            if token != identsym
                error
            if SYMBOLTABLECHECK (token) != -1
                error
            save ident name
            get next token
            if token != eqlsym
                error
            get next token
            if token != numbersym
                error
            add to symbol table (kind 1, saved name, number, 0, 0)
            get next token
        while token == commasym
        if token != semicolonsym
            error
        get next token


VAR-DECLARATION – returns number of variables
    numVars = 0
    if token == varsym
        do
            numVars++
            get next token
            if token != identsym
                error
            if SYMBOLTABLECHECK (token) != -1
                error
            add to symbol table (kind 2, ident, 0, 0, var# + 2)
            get next token
        while token == commasym
        if token != semicolonsym
            error
        get next token
    return numVars


STATEMENT
    if token == identsym
        symIdx = SYMBOLTABLECHECK (token)
        if symIdx == -1
            error
        if table[symIdx].kind != 2 (not a var)
            error
        get next token
        if token != becomessym
            error
        get next token
        EXPRESSION
        emit STO (M = table[symIdx].addr)
        return
    if token == beginsym
        do
            get next token
            STATEMENT
        while token == semicolonsym
        if token != endsym
            error
        get next token
        return
    if token == ifsym
        get next token
        CONDITION
        jpcIdx = current code index
        emit JPC
        if token != thensym
        error
        get next token
        STATEMENT
        code[jpcIdx].M = current code index
        return
    if token == whilesym
        get next token
        loopIdx = current code index
        CONDITION
        if token != dosym
            error
        get next token
        jpcIdx = current code index
        emit JPC
        STATEMENT
        emit JMP (M = loopIdx)
        code[jpcIdx].M = current code index
        return
    if token == readsym
        get next token
        if token != identsym
            error
        symIdx = SYMBOLTABLECHECK (token)
        if symIdx == -1
            error
        if table[symIdx].kind != 2 (not a var)
            error
        get next token
        emit READ
        emit STO (M = table[symIdx].addr)
        return
    if token == writesym
        get next token
        EXPRESSION
        emit WRITE
        return

        
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
error*/