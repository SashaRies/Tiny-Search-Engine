# DESIGN.md - TSE Querier
### Author: Sasha Ries

## Data Structures

My TSE Querier implementation leverages the following key data structures:

1. **Index**: A hashtable where
   - key is a word (normalized)
   - item is a counters set where each (key,count) pair represents (docID,count) where count is the number of occurrences of the word in the document with that ID

2. **Query representation**: 
   - An array of strings containing the normalized words from the query
   - An integer tracking the total number of words in the query

3. **Query result**: A counters struct where
   - key is a docID
   - count represents the document's total score (rank) for the given query

## Control Flow

The querier follows this high-level control flow:

1. Process and validate command-line arguments
2. Load the inverted index from an existing indexfile
3. For each query from stdin:
   - Tokenize and validate the query
   - Process the query against the index
   - Rank and display the results
4. Clean up

## Error Handling

The implementation handles several error conditions:

1. Invalid command-line arguments → print usage and exit(1)
2. Invalid pageDirectory → print error and exit(2)
3. Cannot read indexFile → print error and exit(3)
4. Invalid query syntax → print error and continue to next query
5. Memory allocation failures → check after each allocation

## Detailed Pseudo code

### main

``` 
Parse and validate command-line arguments
Verify pageDirectory exists and was created by crawler
Verify indexFilename exists and is readable
Load the index from indexFilename

while there's input from stdin:
    prompt user (if stdin is a terminal)
    read a line
    tokenize the query
    if query is valid:
        execute the query
        sort and display results
    free memory for this query

free the index
return 0 (success) 
```

### tokenize_query
```
Copyif input string is empty or NULL:
    return NULL
    
create an array to hold words
initialize word count = 0

for each character in the input string:
    if character is a letter:
        collect consecutive letters as a word
        normalize to lowercase
        add to array
        increment word count
    else if character is a space:
        continue (skip spaces)
    else:
        print error message (invalid character)
        free memory
        return NULL

return the array of words and count
```

### process_query_array
```
Copycreate a new counters object for accumulating results

for each word in the query:
    if word is "and":
        skip (implicit in processing)
    else if word is "or":
        merge current accumulated score with final result
        reset accumulated score
    else:
        find counters for this word in the index
        if first word after "or" or beginning:
            copy word counters to accumulator
        else:
            intersect word counters with accumulator (taking minimum counts)

return final result counters
```

### print_ranked_results
```
Copyif result counters is empty:
    print "No documents match."
    return

count matching documents
create array of (docID, score) pairs
populate array from counters

sort array by score (descending)

for each entry in sorted array:
    open document file
    read URL
    print "score: docID URL"

free memory
```

### Utility Functions
```
prompt_user
Copyif stdin is connected to a terminal:
    print "Query? "
free_words
Copyfor each word in the array:
    free the word
free the array itself
```