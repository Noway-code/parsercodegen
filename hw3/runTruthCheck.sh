#!/bin/bash

gcc -o parsercodegen parsercodegen.c

# Directory containing the test cases that should pass without errors
testCaseDir="./testCases/resultTrue"
# Output directory for the results
outputDir="./output/resultTrue"

# Check if parsercodegen executable exists
if [ ! -f "./parsercodegen" ]; then
    echo "parsercodegen executable not found."
    exit 1
fi

# Create the output directory if it doesn't already exist
mkdir -p "$outputDir"

# Initialize arrays to track test results
declare -a passedTests
declare -a failedTests

# Iterate over all files in the test case directory
for testFile in "$testCaseDir"/*; do
    # Extract the name of the file for printing
    fileName=$(basename "$testFile")
    # Create output file name in the desired format
    outputFile="${fileName}Output"

    echo "Running test case: $fileName"

    # Run parsercodegen with the test file and capture the output
    output=$(./parsercodegen "$testFile" 2>&1)
    # Write the output to the corresponding file
    echo "$output" > "$outputDir/$outputFile"

    # Print the output to the console
    echo "Output of the test saved to $outputDir/$outputFile"

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
