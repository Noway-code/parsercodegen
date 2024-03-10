# Tiny PL/0 Compiler

## Compilation

gcc -o parsercodegen parsercodegen.c
./parsercodegen <input_file>

	Replace `<input_file>` with the test case of your choice.

## Input Files

There are 30 test cases available at `testCases/` directory, `testCases/errors` contains a demonstration of all 15 errors
possible during parsing. `testCases/resultTrue` contains 15 different scenarios which do not contain errors and should compile properly.

### Test Scripts are available:

1.
chmod -x <fileName>.sh

2.
./<fileName>.sh

fileName may be `runErrorTests` or `runTruthCheck`

## Output Files

The `outputs` directory includes 30 output files corresponding to the compiler's output for each input file.

## Contributors

- Jacob Dioso
- Camilo Alvarez-Velez
