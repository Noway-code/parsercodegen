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



// Function Prototypes
void printSource(FILE* fileptr);
void createToken(FILE* fileptr);
char peek(FILE *fileptr);
int isReserved(char* lexeme);
int isSymbol(char reader);
int symbolCheck(char* lexeme, int lexCount);
void printReserved(char* lexeme, int token, int lexCount);
void printNumber(char* lexeme, int lexCount);
void printSymbol(char* lexeme, int token, int lexCount);
void printTokenList();

typedef struct Tokens {
    int token;
    char number[numMax];
    char identifier[identMax];
} Tokens;

Tokens tokenList[MAX_STRING];
int tokenCount;

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
            printReserved(lexeme, token, lexCount);
        }
        else if (isdigit(reader) && comment == 0) { // Check for numbers
            lexeme[lexCount++] = reader;

            while (isdigit(peek(fileptr))) {
                reader = fgetc(fileptr);
                lexeme[lexCount++] = reader;
            }
            lexeme[lexCount] = '\0';

            printNumber(lexeme, lexCount);
        }
        else if (reader == ' ' || reader == 13 || reader == 10) { // Ignores spaces and new lines
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
                printSymbol(lexeme, token, lexCount);
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
void printReserved(char* lexeme, int token, int lexCount) {
    // Checks to make sure the ident isn't too long
    if (lexCount > 11) {
        printf("Error:\t\t\tName too long\n");
        return;
    }

    // Checks which reserved word to print out
    switch (token) {
    case -1: // Prints out the identifier
        printf("%-11s\t\t%d\n", lexeme, identsym);
        tokenList[tokenCount].token = identsym;
        strcpy(tokenList[tokenCount].identifier, lexeme);
        tokenCount++;
        break;

    case 0: // const
        printf("%-11s\t\t%d\n", lexeme, constsym);
        tokenList[tokenCount++].token = constsym;
        break;

    case 1: // var
        printf("%-11s\t\t%d\n", lexeme, varsym);
        tokenList[tokenCount++].token = varsym;
        break;

    case 2: // procedure
        printf("%-11s\t\t%d\n", lexeme, procsym);
        tokenList[tokenCount++].token = procsym;
        break;

    case 3: // call
        printf("%-11s\t\t%d\n", lexeme, callsym);
        tokenList[tokenCount++].token = callsym;
        break;

    case 4: // begin
        printf("%-11s\t\t%d\n", lexeme, beginsym);
        tokenList[tokenCount++].token = beginsym;
        break;

    case 5: // end
        printf("%-11s\t\t%d\n", lexeme, endsym);
        tokenList[tokenCount++].token = endsym;
        break;

    case 6: // if
        printf("%-11s\t\t%d\n", lexeme, ifsym);
        tokenList[tokenCount++].token = ifsym;
        break;

    case 7: // fi
        printf("%-11s\t\t%d\n", lexeme, fisym);
        tokenList[tokenCount++].token = fisym;
        break;

    case 8: // then
        printf("%-11s\t\t%d\n", lexeme, thensym);
        tokenList[tokenCount++].token = thensym;
        break;

    case 9: // else
        printf("%-11s\t\t%d\n", lexeme, elsesym);
        tokenList[tokenCount++].token = elsesym;
        break;

    case 10: // while
        printf("%-11s\t\t%d\n", lexeme, whilesym);
        tokenList[tokenCount++].token = whilesym;
        break;

    case 11: // do
        printf("%-11s\t\t%d\n", lexeme, dosym);
        tokenList[tokenCount++].token = dosym;
        break;
    
    case 12: // read
        printf("%-11s\t\t%d\n", lexeme, readsym);
        tokenList[tokenCount++].token = readsym;
        break;

    case 13: // write
        printf("%-11s\t\t%d\n", lexeme, writesym);
        tokenList[tokenCount++].token = writesym;
        break;
    
    default:
        printf("Something went wrong printing reserved\n");
        break;
    }

}

// Prints the number and numbersym
void printNumber(char* lexeme, int lexCount) {
    if (lexCount > numMax) {
        printf("Error:\t\t\tNumber too long\n");
        return;
    }
    printf("%-11s\t\t%d\n", lexeme, numbersym);
    tokenList[tokenCount].token = numbersym;
    strcpy(tokenList[tokenCount].number, lexeme);
    tokenCount++;
}

// Prints the symbol and the corresponding token
void printSymbol(char* lexeme, int token, int lexCount) {
    if (lexCount == 2) {
        switch (token)
        {
        // :=
        case 0:
            printf("%-11s\t\t%d\n", lexeme, becomessym);
            tokenList[tokenCount++].token = becomessym;
            break;

        // <>
        case 1:
            printf("%-11s\t\t%d\n", lexeme, neqsym);
            tokenList[tokenCount++].token = neqsym;
            break;

        // <=
        case 2:
            printf("%-11s\t\t%d\n", lexeme, leqsym);
            tokenList[tokenCount++].token = leqsym;
            break;

        // >=
        case 3:
            printf("%-11s\t\t%d\n", lexeme, geqsym);
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
            printf("%-11s\t\t%d\n", lexeme, plussym);
            tokenList[tokenCount++].token = plussym;
            break;

        // -
        case 1:
            printf("%-11s\t\t%d\n", lexeme, minussym);
            tokenList[tokenCount++].token = minussym;
            break;

        // *
        case 2:
            printf("%-11s\t\t%d\n", lexeme, multsym);
            tokenList[tokenCount++].token = multsym;
            break;

        // /
        case 3:
            printf("%-11s\t\t%d\n", lexeme, slashsym);
            tokenList[tokenCount++].token = slashsym;
            break;

        // (
        case 4:
            printf("%-11s\t\t%d\n", lexeme, lparentsym);
            tokenList[tokenCount++].token = lparentsym;
            break;

        // )
        case 5:
            printf("%-11s\t\t%d\n", lexeme, rparentsym);
            tokenList[tokenCount++].token = rparentsym;
            break;

        // =
        case 6:
            printf("%-11s\t\t%d\n", lexeme, eqsym);
            tokenList[tokenCount++].token = eqsym;
            break;

        // ,
        case 7:
            printf("%-11s\t\t%d\n", lexeme, commasym);
            tokenList[tokenCount++].token = commasym;
            break;

        // .
        case 8:
            printf("%-11s\t\t%d\n", lexeme, periodsym);
            tokenList[tokenCount++].token = periodsym;
            break;

        // <
        case 9:
            printf("%-11s\t\t%d\n", lexeme, lessym);
            tokenList[tokenCount++].token = lessym;
            break;

        // >
        case 10:
            printf("%-11s\t\t%d\n", lexeme, gtrsym);
            tokenList[tokenCount++].token = gtrsym;
            break;

        // ;
        case 11:
            printf("%-11s\t\t%d\n", lexeme, semicolonsym);
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