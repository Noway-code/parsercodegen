#!/bin/bash

# Directory containing the test cases that should pass without errors
testCaseDir="./testCases/resultTrue"

# Check if parsercodegen executable exists
if [ ! -f "./parsercodegen" ]; then
    echo "parsercodegen executable not found."
    exit 1
fi

# Initialize arrays to track test results
declare -a passedTests
declare -a failedTests

# Iterate over all files in the test case directory
for testFile in "$testCaseDir"/*; do
    # Extract the name of the file for printing
    fileName=$(basename "$testFile")

    echo "Running test case: $fileName"

    # Run parsercodegen with the test file and capture the output
    output=$(./parsercodegen "$testFile" 2>&1)

    # Print the output
    echo "Output of the test:"
    echo "$output"

    # Check if "Error:" appears in the output
    if echo "$output" | grep -q "Error:"; then
        echo "CHECK RESULT: !=== FAIL ===!"
        failedTests+=("$fileName")
    else
        echo "CHECK RESULT: PASS"
        passedTests+=("$fileName")
    fi

    echo "----------------------------------------------------"
done

# Print summary table
echo "Summary of Test Results"
echo "----------------------------------------"
echo "PASSED TESTS"
for testName in "${passedTests[@]}"; do
    echo "- $testName"
done

echo "----------------------------------------"
echo "FAILED TESTS"
for testName in "${failedTests[@]}"; do
    echo "- $testName"
done
