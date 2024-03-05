// Jacob Dioso
// Camilo Alvarez

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Define constants for maximum lengths and numbers of different types of symbols
#define identMax 11
#define numMax 5
#define numResWords 14
#define numSpecSymbols 13
#define numExtraSymbols 4
#define extraSymbolsLength 3
#define MAX_STRING 512

// Define an enumeration for different types of tokens
typedef enum {
	skipsym = 1, identsym, numbersym, plussym, minussym,
	multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
	gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
	periodsym, becomessym, beginsym, endsym, ifsym, thensym,
	whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
	readsym , elsesym} token_type;

// Define arrays for special symbols, extra symbols, and reserved words
char specialSymbols[numSpecSymbols] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};
char extraSymbols[numExtraSymbols][extraSymbolsLength] = {":=", "<>", "<=", ">="};
char reservedWords[numResWords][identMax] = {"const", "var", "procedure", "call", "begin", "end", "if",
                                             "fi", "then", "else", "while", "do", "read", "write"};
// Function Prototypes
void printSource(FILE* fileptr); // Prints out the source file
void createToken(FILE* fileptr); // Creates tokens from the source file
char peek(FILE *fileptr); // Peeks at the next character in the file
int isReserved(char* lexeme); // Checks if a lexeme is a reserved word
int isSymbol(char reader); // Checks if a character is a symbol
int symbolCheck(char* lexeme, int lexCount); // Checks if a lexeme is a symbol
void printReserved(char* lexeme, int token, int lexCount); // Prints a reserved word and its corresponding token
void printNumber(char* lexeme, int lexCount); // Prints a number and its corresponding token
void printSymbol(char* lexeme, int token, int lexCount); // Prints a symbol and its corresponding token
void printTokenList(); // Prints the entire token list


// Define a struct for tokens
typedef struct Tokens {
	int token;
	char number[numMax];
	char identifier[identMax];
} Tokens;

// Define an array for the token list and a counter for the number of tokens
Tokens tokenList[MAX_STRING];
int tokenCount;


int main(int argc, char **argv) {
	// Check if a file was included in the command line arguments
	if (argc < 2)
	{
		printf("No file included\n");
		exit(1);
	}
	// Open the file, print the source, create tokens, and close the file
	FILE* fileptr = fopen(argv[1], "r");
	printSource(fileptr);
	createToken(fileptr);
	fclose(fileptr);
	return 0;
}

void printSource(FILE* fileptr) {
	printf("Source Program:\n");

	int reader; // int since EOF is represented as a negative number
	// Print each character in the file until EOF is reached
	while ((reader = getc(fileptr)) != EOF)
		putchar(reader);
	// Rewind the file pointer to the beginning of the file
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
	// Read each character in the file until EOF is reached
	while ((reader = fgetc(fileptr)) != EOF) {
		lexeme[0] = '\0'; // resets the lexeme after every loop
		lexCount = 0; // resets the count after every loop
		// Check for keywords and identifiers
		if (isalpha(reader) && comment == 0) {
			lexeme[lexCount++] = reader;

			// Continue reading while the next character is alphanumeric
			while (isalnum(peek(fileptr))) {
				reader = fgetc(fileptr);
				lexeme[lexCount++] = reader;
			}
			lexeme[lexCount] = '\0';

			token = isReserved(lexeme);
			printReserved(lexeme, token, lexCount);
		}
			// Check for numbers
		else if (isdigit(reader) && comment == 0) {
			lexeme[lexCount++] = reader;

			// Continue reading while the next character is a digit
			while (isdigit(peek(fileptr))) {
				reader = fgetc(fileptr);
				lexeme[lexCount++] = reader;
			}
			lexeme[lexCount] = '\0';

			printNumber(lexeme, lexCount);
		}
			// Ignore spaces and new lines
		else if (reader == ' ' || reader == 13 || reader == 10 || reader == 9) {
			continue;
		}
			// Check for symbols
		else if ((isSymbol(reader)) > -1 || comment == 1) {
			lexeme[lexCount++] = reader;

			// Continue reading while the next character is a symbol
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

char peek(FILE *fileptr) {
	char next_char;

	// Get the next character in the file and then unget it to leave the file pointer unchanged
	next_char = fgetc(fileptr);
	ungetc(next_char, fileptr);

	return next_char;
}

int isReserved(char* lexeme) {
	int match;

	// Compare the lexeme to each reserved word
	for (match = 0; match < numResWords; match++) {
		if (strcmp(lexeme, reservedWords[match]) == 0)
			return match;
	}

	return -1;
}

int isSymbol(char reader) {

	// Compare the reader to each special symbol
	for (int search = 0; search < numSpecSymbols; search++) {
		if (reader == specialSymbols[search])
			return search;
	}

	return -1;
}

int symbolCheck(char* lexeme, int lexCount) {
	int match;

	// Check if the symbol has 2 characters
	if (lexCount == 2) {
		for (match = 0; match < numExtraSymbols; match++) {
			if (strcmp(lexeme, extraSymbols[match]) == 0)
				return match;
		}
	}

	// Compare the lexeme to each special symbol
	for (match = 0; match < numSpecSymbols; match++) {
		if (lexeme[0] == specialSymbols[match])
			return match;
	}
}

void printReserved(char* lexeme, int token, int lexCount) {
	// Check if the identifier is too long
	if (lexCount > 11) {
		printf("Error:\t\t\tName too long\n");
		return;
	}

	// Print the reserved word and its corresponding token
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

void printNumber(char* lexeme, int lexCount) {
	// Check if the number is too long
	if (lexCount > numMax) {
		printf("Error:\t\t\tNumber too long\n");
		return;
	}
	// Print the number and its corresponding token
	printf("%-11s\t\t%d\n", lexeme, numbersym);
	tokenList[tokenCount].token = numbersym;
	strcpy(tokenList[tokenCount].number, lexeme);
	tokenCount++;
}

void printSymbol(char* lexeme, int token, int lexCount) {
	// Check if the symbol has 2 characters
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
		// Print the symbol and its corresponding token
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

				// Case for '=' symbol
			case 6:
				printf("%-11s\t\t%d\n", lexeme, eqsym);
				tokenList[tokenCount++].token = eqsym;
				break;

				// Case for ',' symbol
			case 7:
				printf("%-11s\t\t%d\n", lexeme, commasym);
				tokenList[tokenCount++].token = commasym;
				break;

				// Case for '.' symbol
			case 8:
				printf("%-11s\t\t%d\n", lexeme, periodsym);
				tokenList[tokenCount++].token = periodsym;
				break;

				// Case for '<' symbol
			case 9:
				printf("%-11s\t\t%d\n", lexeme, lessym);
				tokenList[tokenCount++].token = lessym;
				break;

				// Case for '>' symbol
			case 10:
				printf("%-11s\t\t%d\n", lexeme, gtrsym);
				tokenList[tokenCount++].token = gtrsym;
				break;

				// Case for ';' symbol
			case 11:
				printf("%-11s\t\t%d\n", lexeme, semicolonsym);
				tokenList[tokenCount++].token = semicolonsym;
				break;

				// Default case for any other symbol
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
		// Case for identifier token
		if (tokenList[printer].token == identsym) {
			printf("%d ", tokenList[printer].token);
			printf("%s ", tokenList[printer].identifier);
		}
			// Case for number token
		else if (tokenList[printer].token == numbersym) {
			printf("%d ", tokenList[printer].token);
			printf("%s ", tokenList[printer].number);
		}
			// Case for any other token
		else
			printf("%d ", tokenList[printer].token);
	}
	printf("\n");
}
