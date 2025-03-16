#!/bin/bash
# testing.sh - Testing script for CS50 TSE Querier
# Author: Sasha Ries
# Date: 2/26/25

# Define directory paths and programs
QUERIER=./querier
PAGEDATA=~/cs50-dev/shared/tse/output/letters-3
INDEXFILE=~/cs50-dev/shared/tse/output/indexer/index-letters-3
INVALID_DIR=invalid
TEST_DIR=test-files
FUZZBIN=~/cs50-dev/shared/tse/fuzzquery

# Status tracking
TEST_COUNT=0
TEST_PASS=0

# Function to run a test case and track its success/failure
run_test() {
    TEST_COUNT=$((TEST_COUNT + 1))
    echo "Test $TEST_COUNT: $1"
    if eval "$2"; then
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
    
    # Create test directories
    mkdir -p "$TEST_DIR"
    mkdir -p "$INVALID_DIR"
    
    # Create test query files
    cat > "$TEST_DIR/basic_queries.txt" << EOF
playground
playground and home
playground or home
playground and home or computer
playground and home and computer
EOF

    cat > "$TEST_DIR/edge_cases.txt" << EOF
and playground
or playground
playground and
playground or
playground and and home
playground or or home
EOF

    cat > "$TEST_DIR/complex_queries.txt" << EOF
playground and home and computer or school and teacher
home or school or office and computer or playground
playground and home or computer and school or teacher and student
EOF

    # Create a file with invalid characters
    cat > "$TEST_DIR/invalid_chars.txt" << EOF
playground and home!
playground 123
computer@school
EOF
}

# Function to clean up test files
cleanup() {
    echo "Cleaning up test environment..."
    rm -rf "$TEST_DIR" "$INVALID_DIR"
}

echo "Starting Querier tests..."
setup_test_environment

# Section 1: Testing invalid arguments for querier
echo "Testing invalid arguments..."

# Test with no arguments
run_test "No arguments" "! $QUERIER"

# Test with one argument
run_test "One argument" "! $QUERIER $PAGEDATA"

# Test with three arguments
run_test "Three arguments" "! $QUERIER $PAGEDATA $INDEXFILE extra"

# Test with non-existent directory
run_test "Non-existent directory" "! $QUERIER /nonexistent/path $INDEXFILE"

# Test with non-existent index file
run_test "Non-existent index file" "! $QUERIER $PAGEDATA /nonexistent/index"

# Test with directory that is not a crawler directory
run_test "Invalid crawler directory" "! $QUERIER $INVALID_DIR $INDEXFILE"

# Section 2: Testing with invalid queries
echo "Testing invalid queries..."

# Test with queries containing invalid characters
echo -e "\nRunning invalid character queries:"
$QUERIER $PAGEDATA $INDEXFILE < $TEST_DIR/invalid_chars.txt
RESULT=$?
run_test "Queries with invalid characters (should handle gracefully)" "[ $RESULT -eq 0 ]"

# Section 3: Testing with valid queries
echo "Testing with valid queries..."

# Test with basic queries
echo -e "\nRunning basic queries:"
$QUERIER $PAGEDATA $INDEXFILE < $TEST_DIR/basic_queries.txt
RESULT=$?
run_test "Basic queries" "[ $RESULT -eq 0 ]"

# Test with complex queries
echo -e "\nRunning complex queries:"
$QUERIER $PAGEDATA $INDEXFILE < $TEST_DIR/complex_queries.txt
RESULT=$?
run_test "Complex queries" "[ $RESULT -eq 0 ]"

# Test with edge case queries
echo -e "\nRunning edge case queries:"
$QUERIER $PAGEDATA $INDEXFILE < $TEST_DIR/edge_cases.txt
RESULT=$?
run_test "Edge case queries" "[ $RESULT -eq 0 ]"

# Section 4: Fuzz testing
echo "Running fuzz tests..."


if [ -x "$FUZZBIN" ]; then
    echo "Generating 10 random queries with fuzzquery..."
    $FUZZBIN $INDEXFILE 10 > "$TEST_DIR/fuzz_queries.txt"
    
    echo -e "\nRunning fuzz-generated queries:"
    $QUERIER $PAGEDATA $INDEXFILE < $TEST_DIR/fuzz_queries.txt
    RESULT=$?
    run_test "Fuzz-generated queries" "[ $RESULT -eq 0 ]"
else
    echo "Warning: fuzzquery could not be compiled. Skipping fuzz tests."
fi

# Print test summary
echo "Test Summary:"
echo "Passed: $TEST_PASS/$TEST_COUNT tests"

# Clean up
cleanup

# Exit with success if all tests passed
[ "$TEST_PASS" -eq "$TEST_COUNT" ]