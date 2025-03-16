#!/bin/bash

# Author: Sasha Ries
# Date: 2/11/25 
# Test script for CS50 TSE Crawler
# Tests various scenarios based on the design spec requirements


# Initialize test counter to track test progression
test_num=1

# Function to display formatted test headers
# Parameters:
#   $1: Description of the test being run
print_test_header() {
    echo -e "\n${GREEN}Test $test_num: $1${NC}"
    ((test_num++))
}

# Function to execute crawler and verify its exit status
# Parameters:
#   $1: URL to crawl (seedURL)
#   $2: Directory to save pages (pageDirectory)
#   $3: Maximum crawl depth (maxDepth)
#   $4: Expected exit status for validation
# The function compares the actual exit status with the expected status
run_crawler() {
    ./crawler $1 $2 $3      # Execute crawler with provided arguments
    status=$?               # Capture crawler's exit status
    if [ $status -eq $4 ]; then
        echo -e "✓ Test passed: Expected exit status $4$"
    else
        echo -e "✗ Test failed: Got exit status $status, expected $4$"
    fi
}

# These directories serve as pageDirectories where crawled pages will be saved
mkdir -p letters    # For testing with letters website

# Test 1: Verify crawler handles incorrect argument count
print_test_header "Testing with invalid number of arguments"
./crawler "http://cs50tse.cs.dartmouth.edu/tse/letters/index.html" 1

# Test 2: Verify crawler handles invalid URLs
# First '2' is maxDepth (unused in this test)
# Second '2' is expected exit status for invalid URL error
print_test_header "Testing with invalid URL"
run_crawler "not_a_url" "./test_data/invalid" 2 2

# Test 3: Verify crawler rejects negative depths, exit status 4 means invalid depth
print_test_header "Testing with negative maxDepth"
run_crawler "http://cs50tse.cs.dartmouth.edu/tse/letters/index.html" "letters" -1 4

# Test 4: Verify crawler enforces maximum depth limit, exit status 4 means invalid depth
print_test_header "Testing with maxDepth > 10"
run_crawler "http://cs50tse.cs.dartmouth.edu/tse/letters/index.html" "letters" 11 4

# Test 5: Verify basic crawling functionality at depth 0
# index.html is the default entry point for web directories
print_test_header "Testing letters site at depth 0"
run_crawler "http://cs50tse.cs.dartmouth.edu/tse/letters/index.html" "letters" 0 0

# Verify crawler created output files correctly
if [ -f "./letters/1" ]; then
    echo -e "${GREEN}✓ Created file for depth 0${NC}"
else
    echo -e "${RED}✗ Failed to create file for depth 0${NC}"
fi

# Test 6: Verify crawling at depth 1 (following immediate links)
print_test_header "Testing letters site at depth 1"
run_crawler "http://cs50tse.cs.dartmouth.edu/tse/letters/index.html" "letters" 1 0

# Test 7: Verify crawler works with different starting pages
print_test_header "Testing with different seed page (A.html)"
run_crawler "http://cs50tse.cs.dartmouth.edu/tse/letters/A.html" "letters" 1 0

# Test 8: Verify deeper crawling functionality
print_test_header "Testing letters site at depth 2"
run_crawler "http://cs50tse.cs.dartmouth.edu/tse/letters/index.html" "letters" 2 0

# Verify final directory structure and file count
print_test_header "Verifying directory structure"
if [ -d "./letters" ]; then
    echo -e "${GREEN}✓ Test directory structure is correct${NC}"
    file_count=$(ls ./letters | wc -l)
    echo "Number of files crawled: $file_count"
else
    echo -e "${RED}✗ Test directory structure is incorrect${NC}"
fi

# Verify saved pages contain required information
print_test_header "Checking file content format"
if [ -f "./test_data/letters/1" ]; then
    line_count=$(head -n 2 "./test_data/letters/1" | wc -l)
    if [ $line_count -eq 2 ]; then
        echo -e "${GREEN}✓ File format appears correct (has URL and depth)${NC}"
    else
        echo -e "${RED}✗ File format incorrect${NC}"
    fi
fi

# Clean up all test directories and files
echo -e "\nCleaning up test directories..."
rm -rf ./letters

echo -e "\n${GREEN}Testing complete!${NC}"