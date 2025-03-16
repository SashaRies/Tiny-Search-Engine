/*
Author: Sasha Ries
Date: 2/24/25
File: indexer.c
Description: This file contains the main function for the querier program. 
The TSE Querier is a standalone program that reads the index file produced by the TSE Indexer, 
and page files produced by the TSE Crawler, and answers search queries submitted via stdin
*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <ctype.h>
 #include <unistd.h>
 #include "mem.h"
 #include "common/pagedir.h"
 #include "common/index.h"  
 #include "hashtable.h"
 #include "common/query.h"
 #include "file.h"     // from libcs50
 


 
 // Function declarations
 static void prompt_user(void);
 

 
 int main(const int argc, char* argv[]){
    // Check arguments
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of arguments -> Usage: %s pageDirectory indexFilename\n", argv[0]);
        return 1; // Exit status 1 for issues with number of arguments
    }
    
    char* pageDirectory = argv[1];
    char* indexFilename = argv[2];
    
    // Validate pageDirectory (must exist and is crawler generated)
    if (!is_crawler_directory(pageDirectory)) {
        fprintf(stderr, "Error: invalid pageDirectory %s\n", pageDirectory);
        return 2; // Exit status 2 for issues with crawler files
    }

    // Validate first document can be read (pageDirectory/1)
    int path_len = strlen(pageDirectory) + 10;  // Store the buffer length for filepath string
    char* filepath = mem_malloc(path_len); // Allocate memory for the filepath string

    sprintf(filepath, "%s/1", pageDirectory);
    FILE* fp_pageDir = fopen(filepath, "r");
    if (fp_pageDir == NULL) {
        fprintf(stderr, "Error: cannot read %s/1\n", pageDirectory);
        return 2; // Exit status 2 for issues with crawler files
    }
    fclose(fp_pageDir);
    mem_free(filepath);
     

    // Validate indexFile exists and is readable
    FILE* fp_indexFile = fopen(indexFilename, "r");
    if (fp_indexFile == NULL) {
        fprintf(stderr, "Error: cannot read %s\n", indexFilename);
        return 3; // Exit status 3 for issues reading indexFilename 
    }
    int size = file_numLines(fp_indexFile); // Get number of lines (total words) in indexFilename


    // Load the index from file and store in internal seperate index
    hashtable_t* index = load_index(fp_indexFile, size);
    if (index == NULL) {
        fprintf(stderr, "Error: failed to load index from %s\n", indexFilename);
        return 3; // Exit status 3 for issues reading indexFilename
    }
    fclose(fp_indexFile);
    
    char* line = NULL;
    prompt_user();

    // Read queries from stdin, one per line, until EOF
    while ((line = file_readLine(stdin)) != NULL) {  
        int num_words = 0; // Use pointer to this to update total words in query     
        char** words = tokenize_query(line, &num_words); // Tokenize the query into words
        mem_free(line); // Free the query string input
        
        if (words == NULL) { // We check if the query is valid in tokenize_query()
            continue; // Go to next query input if not valid
        }else{
            // Get counters of (docID, total rank) pairs
            counters_t* result_counters = process_query_array(words, num_words, index);
            
            // Print ranked results
            print_ranked_results(result_counters, pageDirectory);
            
            counters_delete(result_counters); // Cleanup
        }
        free_words(words, num_words); // Free the tokenized words array
        prompt_user();
    }
    index_delete(index); // Cleanup
    return 0; // Succesfully made it through every step
} 

int fileno(FILE *stream);
/* Print a prompt if stdin is a tty (terminal) */
static void prompt_user(void){
    if (isatty(fileno(stdin))) { // print a prompt iff stdin is a tty (terminal)
        printf("Query? ");
    }
}