# Tiny PL/0 Compiler

## Input Files

There are 30 test cases available at `testCases/` directory, `testCases/errors` contains tests for all 15 errors named
according to the error they represent. `testCases/resultTrue` contains 15 different scenarios which do not contain errors and should compile properly.

## Output Files

The `outputs` directory includes 30 output files corresponding to the compiler's output for each input file.
They take the form of `<input_file>Output.txt`.

## Compilation

gcc -o parsercodegen parsercodegen.c
./parsercodegen testCases/<test_type>/<input_file>

- Replace `<test_type>` with `errors` or `resultTrue` depending on the test case you want to run.
- Replace `<input_file>` with the test case of your choice.


### Test Scripts are available (may not work on Windows devices unfortunately):

1.
chmod -x <fileName>.sh

2.
./<fileName>.sh

fileName may be `runErrorTests` or `runTruthCheck`


## Contributors

- Jacob Dioso
- Camilo Alvarez-Velez
