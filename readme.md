# Tiny PL/0 Compiler

---

This project is an implementation of a Tiny PL/0 compiler, completed as part of the System Software course (COP 3402) at the University of Central Florida, Spring 2024. It encompasses a Recursive Descent Parser and an Intermediate Code Generator that processes the PL/0 programming language, producing assembly code for a virtual machine as output.

## Getting Started

### Compilation

This project requires a Unix-like environment for compilation and execution.
To compile the project, navigate to the project directory and run the following command:

```sh
gcc -o parsercodegen parsercodegen.c
```

### Running the Compiler

The compiler can be executed with input files through command-line arguments. Here is the basic usage:

```sh
./parsercodegen <input_file>
```

Replace `<input_file>` with the test case of your choice.

## Input Files

There are 30 test cases are available at `testCases/` directory, `testCases/errors` contains a demonstration of all 15 errors possible during parsing. `testCases/resultTrue` contains 15 different scenarios which do not contain errors and should compile properly. 

Test Scripts are available:

1.
```
chmod -x <fileName>.sh
```
2.
```
./<fileName>.sh
```
fileName may be `runErrorTests` or `runTruthCheck`

## Output Files

The `outputs` directory includes at least 15 output files corresponding to the compiler's output for each input file. These demonstrate the error handling and successful compilation where applicable.

## Features

- **Recursive Descent Parsing:** Implements a parser that checks PL/0 source code against the language's grammar.
- **Intermediate Code Generation:** Generates assembly code for the virtual machine if the source code is syntactically correct.
- **Symbol Table Management:** Manages a symbol table containing variables and constants declared in the PL/0 program.
- **Error Reporting:** Identifies and reports specific errors in the source code, adhering to predefined error messages.

## Contributors

- Jacob Dioso
- Camilo Alvarez-Velez
