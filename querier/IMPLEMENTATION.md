# IMPLEMENTATION.md - TSE Querier
## Author: Sasha Ries

This document describes the implementation details and testing plan for my TSE Querier module.

## Implementation Details

### Major Components

My querier implementation consists of the following major components:

1. **Argument Validation and Index Loading**
   - Validates pageDirectory using `is_crawler_directory()` from common/pagedir.h
   - Attempts to open the first document to verify directory contents
   - Loads the index using `load_index()` from common/index.h

2. **Query Processing**
   - `tokenize_query()`: Parses and validates user input
   - `process_query_array()`: Processes query words against the index
   - `print_ranked_results()`: Sorts and displays matching documents

3. **Memory Management**
   - All dynamically allocated memory is properly tracked and freed
   - Uses mem.h functions for allocation/deallocation to help with debugging

### Data Management

The querier manages data flow using these strategies:

1. **Index Storage**
   - Uses the hashtable_t structure to store the index
   - Each word (key) in the index maps to a counters_t structure containing (docID, count) pairs

2. **Query Representation**
   - Queries are parsed into arrays of normalized words
   - The tokenize_query function removes spaces and normalizes characters
   - All words are normalized to lowercase

3. **Result Accumulation**
   - Uses counters_t to accumulate scores for matching documents
   - Implements the query logic directly in process_query_array()
   - Handles AND/OR operators according to precedence rules

### Key Implementation Functions

1. `main()`: 
   - Validates arguments and loads resources
   - Contains the main input loop for receiving queries
   - Frees all resources on completion

2. `tokenize_query()`:
   - Processes raw query strings into word arrays
   - Normalizes words to lowercase
   - Performs validation (letters and spaces only, 'and' and 'or' cannot be adjacent, are at begginning or end)
   - Returns NULL for invalid queries

3. `process_query_array()`:
   - Implements the query logic for AND/OR operators
   - Uses the index to find document matches
   - Returns a counters object with (docID, rank) pairs 

4. `print_ranked_results()`:
   - Sorts matching documents by score
   - Opens crawled document files to retrieve URLs
   - Formats and prints results

### Error Handling

My implementation handles errors with specific status codes:
- Status 1: Incorrect number of arguments
- Status 2: Issues with crawler directory or files
- Status 3: Issues reading index file

For query-specific errors (such as invalid characters), the program prints an error message but continues processing subsequent queries.

## Testing Plan

I tested my querier implementation using the following approach:

## Testing Plan

I tested my querier implementation using a comprehensive test script (testing.sh) that performs the following tests:

### Argument Validation Tests

1. **Invalid Arguments**
   - Test with no arguments (should fail)
   - Test with only one argument (should fail)
   - Test with too many arguments (should fail)
   - Test with non-existent directory (should fail)
   - Test with non-existent index file (should fail)
   - Test with directory that is not a crawler directory (should fail)

### Query Processing Tests

1. **Basic Queries**
   - Single word queries (e.g., "playground")
   - Two-word AND queries (e.g., "playground and home")
   - Two-word OR queries (e.g., "playground or home")
   - Mixed operator queries (e.g., "playground and home or computer")
   - Multiple AND operators (e.g., "playground and home and computer")

2. **Edge Case Queries**
   - Queries starting with operators (e.g., "and playground")
   - Queries ending with operators (e.g., "playground and")
   - Queries with consecutive operators (e.g., "playground and and home")

3. **Complex Queries**
   - Longer queries with mixed operators
   - Multiple OR conditions with nested AND conditions
   - Queries testing operator precedence

4. **Invalid Query Handling**
   - Queries with invalid characters (e.g., "playground and home!")
   - Queries with numbers (e.g., "playground 123")
   - Queries with special characters (e.g., "computer@school")

### Fuzz Testing

- Used the CS50 `fuzzquery` program to generate 10 random queries from the index file
- Verified the querier handles these unpredictable inputs without crashing
- Ensured robust handling of a wide variety of query patterns

### Test Environment

- Used the `letters-3` dataset from the shared CS50 TSE output directory
- Used a pre-built index file for the `letters-3` dataset
- Created test files with different categories of queries
- Set up automatic test validation to track passing/failing tests

### Test Execution and Reporting

- Each test is executed and its success/failure is tracked
- The script reports on each test individually
- A summary of passed tests is provided at the end
- The test environment is automatically cleaned up after testing

### Success Criteria

- All test categories should execute without crashing
- The querier should handle invalid inputs gracefully
- Valid queries should produce appropriate results
- The program should exit with proper status codes
- All test cases should pass successfully