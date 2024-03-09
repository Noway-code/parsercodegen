#!/bin/bash

# Specify the directory containing the test case files
testCaseDir="./testCases/errors"

# Iterate over all files in the directory
for file in "$testCaseDir"/*; do
    # Use basename to extract the filename from the path
    filename=$(basename "$file")

    # Replace spaces with underscores and remove commas in the filename
    newFilename=$(echo "$filename" | tr ' ' '_' | tr -d ',')

    # Only rename the file if the new name is different
    if [ "$filename" != "$newFilename" ]; then
        # Construct the full path for the new file name
        newPath="$testCaseDir/$newFilename"

        # Rename the file
        mv "$file" "$newPath"
        echo "Renamed '$file' to '$newPath'"
    fi
done
