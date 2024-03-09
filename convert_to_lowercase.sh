#!/bin/bash

# Directory containing the test cases
testCaseDir="./testCases/errors"

# Iterate over all files in the test case directory
for testFile in "$testCaseDir"/*; do
    # Check if file is a regular file (not a directory or a link, etc.)
    if [ -f "$testFile" ]; then
        # Convert all uppercase characters to lowercase in-place
        tr '[:upper:]' '[:lower:]' < "$testFile" > "$testFile.tmp" && mv "$testFile.tmp" "$testFile"
    fi
done

echo "Conversion to lowercase completed."
