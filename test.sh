#!/bin/bash

# Generate test files
echo "Hello, World!" > file1.bin
echo "Goodbye, World!" > file2.bin

# Run the program
./server client file1.bin file2.bin

# Check the result
if cmp -s result.txt expected_result.bin; then
   echo "Test passed"
else
   echo "Test failed"
fi