#!/bin/bash
# testing.sh - Testing script for CS50 TSE Indexer
# Author: Sasha Ries
# Date: 2/17/25

# Define directory paths and programs
INDEXER=./indexer
INDEXTEST=./indextest
INVALID_DIR=invalid
CRAWLER_DIR=~/cs50-dev/shared/tse/output/crawler/pages-letters-depth-1
INDEX_DIR=index_dir

# Status tracking
TEST_COUNT=0
TEST_PASS=0

# Function to run a test case and track its success/failure
run_test() {
    TEST_COUNT=$((TEST_COUNT + 1))
    echo "Test $TEST_COUNT: $1"
    if eval "$2"; then         # Changed this line to use eval with quotes
        echo "✓ Passed"
        TEST_PASS=$((TEST_PASS + 1))
    else
        echo "✗ Failed"
    fi
    echo "----------------------------------------"
}

# Function to create test directories and files
setup_test_environment() {
    echo "Setting up test environment..."
    
    # Create test directories with normal permissions initially
    mkdir -p "$INDEX_DIR"
    mkdir -p "$INVALID_DIR"
    touch "$INVALID_DIR/index.txt"
}

# Function to clean up test files
cleanup() {
    echo "Cleaning up test environment..."
    
    # Restore permissions before removal to ensure cleanup works
    chmod 755 "$INVALID_DIR" 2>/dev/null
    chmod 644 "$INVALID_DIR/index.txt" 2>/dev/null
}

echo "Starting Indexer tests..."
setup_test_environment

# Section 1: Testing invalid arguments for indexer
echo "Testing invalid arguments..."

# Test with no arguments
run_test "No arguments" "! $INDEXER"

# Test with one argument
run_test "One argument" "! $INDEXER $CRAWLER_DIR"

# Test with three arguments
run_test "Three arguments" "! $INDEXER $CRAWLER_DIR output.txt extra"

# Test with non-existent directory
run_test "Non-existent directory" "! $INDEXER /nonexistent/path output.txt"

# Test with invalid crawler directory (no .crawler file)
run_test "Invalid crawler directory" "! $INDEXER $INVALID_DIR output.txt"

# Test with read-only output directory
# Make directory read-only before test
chmod 555 "$INVALID_DIR"
run_test "Read-only output directory" "! $INDEXER $CRAWLER_DIR $INVALID_DIR/out.txt"
# Restore permissions after test
chmod 755 "$INVALID_DIR"

# Test with existing read-only output file
# Make file read-only before test
chmod 444 "$INVALID_DIR/index.txt"
run_test "Read-only output file" "! $INDEXER $CRAWLER_DIR $INVALID_DIR/index.txt"
# Restore permissions after test
chmod 644 "$INVALID_DIR/index.txt"

# Section 2: Integration testing with valid inputs
echo "Running integration tests..."

# Test case with small crawler directory
INDEX_FILE="$INDEX_DIR/test1.index"
run_test "Building index from small crawler directory" "$INDEXER $CRAWLER_DIR $INDEX_FILE"

# Validate the created index using indextest
if [ -f "$INDEX_FILE" ]; then
    NEW_INDEX="$INDEX_DIR/test1_copy.index"
    run_test "Validating index with indextest" "$INDEXTEST $INDEX_FILE $NEW_INDEX"
    
    run_test "Comparing index files" "~/cs50-dev/shared/tse/indexcmp $INDEX_FILE $NEW_INDEX"
fi

# Print test summary
echo "Test Summary:"
echo "Passed: $TEST_PASS/$TEST_COUNT tests"

# Clean up
cleanup

# Exit with success if all tests passed
[ "$TEST_PASS" -eq "$TEST_COUNT" ]