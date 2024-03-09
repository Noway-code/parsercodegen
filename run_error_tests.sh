#!/bin/bash

# Directory containing the error test cases
testCaseDir="/home/noway/CLionProjects/parsercodegen/testCases/errors"

# Check if parsercodegen executable exists
if [ ! -f "./parsercodegen" ]; then
    echo "parsercodegen executable not found."
    exit 1
fi

# Function to simplify strings for comparison
simplify_string() {
    echo "$1" | tr '[:upper:]' '[:lower:]' | sed -e 's/[^a-z0-9]/ /g' -e 's/\b\(and\|or\|the\|must\|be\|with\|by\|a\|an\)\b//g' | tr -s ' '
}

# Initialize arrays to track test results
declare -a passedTests
declare -a failedTests

# Iterate over all files in the test case directory
for testFile in "$testCaseDir"/*; do
    # Extract the name of the file for printing
    fileName=$(basename "$testFile")
    simplifiedFileName=$(simplify_string "$fileName")

    echo "Running test case: $fileName"

    # Run parsercodegen with the test file and capture the output
    output=$(./parsercodegen "$testFile" 2>&1)
    simplifiedOutput=$(simplify_string "$output")

    # Print the output
    echo "Output of the test:"
    echo "$output"

    # Check if the file name appears in the output
    if [[ "$simplifiedOutput" == *"$simplifiedFileName"* ]]; then
        echo "CHECK RESULT: PASS"
        passedTests+=("$fileName")
    else
        echo "CHECK RESULT: !=== FAIL ===!"
        failedTests+=("$fileName")
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
