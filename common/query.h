/*
Author: Sasha Ries
Date: 2/25/25
File: query.h
Description: header file for CS50 query module
*/

#ifndef __QUERY_H
#define __QUERY_H

#include "counters.h"
#include "hashtable.h"

/**************** process_query_array ****************/
/* 
 * Process a tokenized query according to the BNF grammar and return matching documents.
 * 
 * The query grammar has two non-terminals:
 *   query       ::= <andsequence> [or <andsequence>]...
 *   andsequence ::= <word> [ [and] <word>]...
 *
 * Caller provides:
 *   words - array of tokenized query words (must not be NULL)
 *   num_words - number of words in the array (must be > 0)
 *   index - the index data structure to search (must not be NULL)
 *
 * Returns:
 *   A counters object with document IDs and their scores, or NULL on error
 *   For AND sequences, scores are the minimum count across all words
 *   For OR operations, scores are the sum of the AND sequence scores
 *
 * Caller is responsible for:
 *   Later freeing the returned counters with counters_delete()
 */
counters_t* process_query_array(char** words, int num_words, hashtable_t* index);

/**************** tokenize_query ****************/
/* 
 * Tokenize a query string into an array of normalized words.
 * 
 * Caller provides:
 *   query_str - a string containing the query to tokenize (must not be NULL)
 *
 * Returns:
 *   An array of strings (char**) containing the tokenized words, or NULL on error
 *   Each word in the array is normalized (lowercase)
 *   The array is terminated by a NULL pointer
 *
 * We validate:
 *   The query contains only letters and spaces
 *   The query follows syntax rules (operators cannot be first/last, operators cannot be adjacent)
 *
 * Notes:
 *   Prints appropriate error messages for invalid queries
 *   The caller is responsible for freeing the returned array and all its strings using free_words()
 */
char** tokenize_query(char* query_str, int* word_count);


/**************** print_ranked_results ****************/
/* 
 * Print the documents that match a query, in decreasing order by score.
 * 
 * Caller provides:
 *   result_counters - a counters data structure mapping docIDs to scores (must not be NULL)
 *   pageDirectory - path to the directory containing the crawled pages (must not be NULL)
 *
 * We do:
 *   Count the number of documents with non-zero scores
 *   If no matches, print a message and return
 *   Otherwise, create a copy of the counters to work with
 *   Print the total number of matching documents
 *   Repeatedly find the highest-scoring document and print its score, ID, and URL
 *   Clean up memory when done
 *
 * We print:
 *   "No documents match." if no documents match
 *   Otherwise, "Matches N documents (ranked):" followed by
 *   One line per document with format "score SSSS doc DDDD: URL"
 *
 * Notes:
 *   Uses helper functions to find max scores and copy counters
 *   URLs are retrieved from the page directory and freed after printing
 */
void print_ranked_results(counters_t* result_counters, char* pageDirectory);

/**************** free_words ****************/
/* 
 * Free memory allocated for an array of words.
 *
 * Caller provides:
 *   words - array of word strings created by tokenize_query
 *   num_words - the number of words in the array
 *
 * We do:
 *   Free each word string in the array
 *   Free the array itself
 *
 * Notes:
 *   Does nothing if words is NULL
 */
void free_words(char** words, int num_words);

#endif // __QUERY_H