

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "query.h"
#include "word.h"
#include "mem.h"
#include "counters.h"
#include "hashtable.h"
#include "pagedir.h"

/* ------------------------------------------------ Declare local functions -------------------------------------------------*/
static bool is_query_valid(char** words, int num_words);
static void counter_copy_helper(void* arg, const int key, const int count);
static void copy_counters(counters_t* src, counters_t* new);
static void intersection_helper(void* arg, const int key, const int count);
static void intersect_counters(counters_t* result, counters_t* counters2);
static void union_helper(void* arg, const int key, const int count);
static void union_counters(counters_t* result, counters_t* counters2);
static void count_non_zero(void* arg, const int key, const int count);
static void find_max_docID(void* arg, const int key, const int count);


/* ------------------------------------------------- Global Functions -------------------------------------------------------*/
counters_t* process_query_array(char** words, int num_words, hashtable_t* index){    
    // Create counters instance to hold the overall result (union of andsequences) and store pointer
    counters_t* result = counters_new();
    if (result == NULL) {
        fprintf(stderr, "Error: failed to create overall result counters instance\n");
        counters_delete(result);
        return NULL;
    }
    
    // Loop through the words array
    int i = 0;
    while (i < num_words) {
        // Process an andsequence (from current position until 'or' or end)
        counters_t* and_result = counters_new(); // Create counters instance for each andsequence and store pointer
        if (and_result == NULL) {
            fprintf(stderr, "Error: failed to create and_result counters\n");
            counters_delete(result);
            return NULL;
        }
        
        // First word in andsequence
        counters_t* word_counters = hashtable_find(index, words[i]); // Get pointer to counters for this word
        if (word_counters != NULL) {
            copy_counters(word_counters, and_result); // Copy scores from this word to and_result
        }
        i++; // Move to next word
        
        // Process remaining words in andsequence
        while (i < num_words && strcmp(words[i], "or") != 0) {
            if (strcmp(words[i], "and") == 0) { // Skip word 'and'
                i++;
            }
            word_counters = hashtable_find(index, words[i]); // Get pointer to counters for this word
            
            // Intersect with running result
            if (word_counters != NULL) {
                intersect_counters(and_result, word_counters); // Take minimum of scores (characteristic of AND operation)
            } else { // No documents have this word
                counters_delete(and_result);
                and_result = counters_new();
                break; // No need to continue this andsequence
            }
            
            i++; // Move to next word
        }
        
        // Union the and_result with the overall result
        union_counters(result, and_result);
        counters_delete(and_result);
        
        // Skip the 'or' to start the next andsequence
        if (i < num_words && strcmp(words[i], "or") == 0) {
            i++;
        }
    }
    return result;
}


char** tokenize_query(char* query_str, int* word_count) {
    int position = 0; // Value to keep track of character position in string

    // Check for NULL pointer
    if (query_str == NULL) {
        return NULL;
    }
    
    // First pass: verify the query contains only letters and spaces
    for (int i = 0; query_str[i] != '\0'; i++) {
        if (!isalpha(query_str[i]) && !isspace(query_str[i])) {
            printf("Error: bad character '%c' in query.\n", query_str[i]);
            return NULL;
        }
    }
    
    // Count the maximum possible number of words
    int max_possible_words = strlen(query_str) + 1; // Estimate the maximum number of words (conservatively)

    // Allocate array to hold the words
    char** words = malloc(max_possible_words * sizeof(char*));
    if (words == NULL) {
        fprintf(stderr, "Error: memory allocation for query input failed\n");
        return NULL;
    }

    char* word;
    while ((word = str_readWord(query_str, &position)) != NULL) {
        // Store the word in the array
        word_normalize(word);
        words[*word_count] = word;
        (*word_count)++;

        if ((*word_count) == max_possible_words){ // Add check to prevent infinite while loop
            break;
            fprintf(stderr, "Error in tokenizing query input");
        }
    }

    if (!is_query_valid(words, *word_count)){ // Error statements within the function
        return NULL;
    }
    return words;
}

void print_ranked_results(counters_t* result_counters, char* pageDirectory){
    
    // Count how many documents have non-zero scores
    int num_matches = 0;
    counters_iterate(result_counters, &num_matches, count_non_zero);
    
    if (num_matches == 0) {
        printf("No documents match.\n");
        return;
    }
    
    // Create a copy of the counters to work with
    counters_t* copy = counters_new();
    if (copy == NULL) {
        fprintf(stderr, "Error: failed to create counters copy\n");
        return;
    }
    
    // Copy all scores to our working copy
    counters_iterate(result_counters, copy, counter_copy_helper);
    
    // Print heading
    printf("Matches %d documents (ranked):\n", num_matches);
    
    // Repeatedly find the highest-scoring document and print it
    for (int i = 0; i < num_matches; i++) {
        // Define a temporary struct instance to store a docID and max rank
        struct {
            int max_docID;
            int max_score;
        } max = {0, 0}; // Create a new instance every iteration
        
        counters_iterate(copy, &max, find_max_docID); // Find max score in current state of counters
        
        if (max.max_score > 0) {
            char* url = get_url(pageDirectory, max.max_docID); // Get the URL for this document
            
            // Print document information
            printf("score %4d doc %4d: %s\n", max.max_score, max.max_docID, url);
            
            mem_free(url); // Free the URL
            
            // Set this doc's score to zero so it won't be found again
            counters_set(copy, max.max_docID, 0);
        }
    }
    counters_delete(copy); // Free memory
}


void free_words(char** words, int num_words){
    if (words != NULL) {
        for (int i = 0; i < num_words; i++) {
            if (words[i] != NULL){
                free(words[i]);
            }
        }
        free(words);
    }
}

/* ------------------------------------------------ Local Helper Functions ----------------------------------------------*/
/* Function to check if query is valid */
static bool is_query_valid(char** words, int num_words) {
    if (num_words == 0) {
        return true; // Empty query is technically valid
    }

    // Rule 1: First word cannot be 'and' or 'or'
    if (strcmp(words[0], "and") == 0) {
        fprintf(stderr, "Error: 'and' cannot be first\n");
        return false;
    }
    if (strcmp(words[0], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be first\n");
        return false;
    }
    
    // Rule 2: Last word cannot be 'and' or 'or'
    if (strcmp(words[num_words-1], "and") == 0) {
        fprintf(stderr, "Error: 'and' cannot be last\n");
        return false;
    }
    if (strcmp(words[num_words-1], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be last\n");
        return false;
    }
    
    // Rule 3: Operators cannot be adjacent
    for (int i = 0; i < num_words - 1; i++) {
        // Check for 'and' followed by 'and'
        if (strcmp(words[i], "and") == 0 && strcmp(words[i+1], "and") == 0) {
            fprintf(stderr, "Error: 'and' and 'and' cannot be adjacent\n");
            return false;
        }
        // Check for 'and' followed by 'or'
        if (strcmp(words[i], "and") == 0 && strcmp(words[i+1], "or") == 0) {
            fprintf(stderr, "Error: 'and' and 'or' cannot be adjacent\n");
            return false;
        }
        // Check for 'or' followed by 'and'
        if (strcmp(words[i], "or") == 0 && strcmp(words[i+1], "and") == 0) {
            fprintf(stderr, "Error: 'or' and 'and' cannot be adjacent\n");
            return false;
        }
        // Check for 'or' followed by 'or'
        if (strcmp(words[i], "or") == 0 && strcmp(words[i+1], "or") == 0) {
            fprintf(stderr, "Error: 'or' and 'or' cannot be adjacent\n");
            return false;
        }
    }
    return true; // If all checks passed, query is valid
}


/* Copy all entries from src counters to new counters */
static void copy_counters(counters_t* src, counters_t* new){
    counters_iterate(src, new, counter_copy_helper); // Iterate over source counters and copy to destination
}


/* Union two counters objects, summing the counts for each docID */
static void union_counters(counters_t* result, counters_t* counters2){
    // Iterate over counters2 and update result
    counters_iterate(counters2, result, union_helper);
}


/* Intersect two counters objects, keeping the minimum count for each docID */
static void intersect_counters(counters_t* result, counters_t* counters2){
    // Create an array of counters pointers
    counters_t* counters_array[2];
    counters_array[0] = result;
    counters_array[1] = counters2;
    
    // Iterate over all items in result and update them
    counters_iterate(result, counters_array, intersection_helper);
}


/* Helper function for copying counter entries */
static void counter_copy_helper(void* arg, const int key, const int count){
    counters_t* new = arg;
    counters_set(new, key, count);
}


/* Helper function for union operation */
static void union_helper(void* arg, const int key, const int count){
    counters_t* result = arg;
    int curr_count = counters_get(result, key); // Get current count in result
    counters_set(result, key, curr_count + count); // Add this count to current value
}


/* Helper function for intersection operation */
static void intersection_helper(void* arg, const int key, const int count) {
    counters_t** counters_array = (counters_t**)arg; // arg is a pointer to an array of two counters pointers
    counters_t* result = counters_array[0];
    counters_t* counters2 = counters_array[1];
    
    int count2 = counters_get(counters2, key); // Get count from second counters
    
    if (count2 == 0) { // If DocID is not in counters2, remove from result
        counters_set(result, key, 0);
    } else { // Update to minimum of the two counts
        counters_set(result, key, (count < count2) ? count : count2); // If count < count2 use count, otherwise count2
    }
}

/* Helper function to count non-zero items in counters */
static void count_non_zero(void* arg, const int key, const int count){
    int* n = arg;
    
    if (count > 0) {
        (*n)++;
    }
}

/* Helper function to find the maximum score and its document ID */
static void find_max_docID(void* arg, const int key, const int count){
    struct {
        int max_docID;
        int max_score;
    }* max = arg;
    
    if (count > max->max_score) {
        max->max_docID = key;
        max->max_score = count;
    }
}