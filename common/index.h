/*
Author: Sasha Ries
Date: 2/17/25
File: index.h
Description: header file for CS50 index module
 
 * An "index" is a mapping from words to (docID, count) pairs, where each
 * pair represents the number of occurrences of that word in a given document.
 * The index is implemented as a hashtable keyed by word and storing counters
 * as items. Each counters set is keyed by docID and stores the count of
 * word occurrences in that document.
 */

 #ifndef __INDEX_H
 #define __INDEX_H
 
 #include <stdio.h>
 #include <stdbool.h>
 #include "hashtable.h"
 #include "webpage.h"  // for webpage_t type


 /**************** indexBuild ****************/
 /* Build an index from all the webpage files in a directory.
  *
  * Caller provides:
  *   valid pathname to a directory containing webpage files
  * We return:
  *   pointer to a new index; NULL if error (out of memory, invalid directory).
  * We guarantee:
  *   index contains entries for all the words in all webpages in the directory.
  * Caller is responsible for:
  *   later calling index_delete().
  */
 hashtable_t* indexBuild(char* pageDirectory);
 

 /**************** indexPage ****************/
 /* Index all the words on a single webpage, incrementing counts for existing 
  * words and adding new words with count=1.
  * 
  * Caller provides:
  *   valid pointer to webpage, valid docID (must be > 0), 
  *   valid pointer to an existing index
  * We do:
  *   extract all words from the webpage and add them to the index
  *   (normalizing by converting to lowercase)
  * We guarantee:
  *   every word from the webpage will be added to the index with the 
  *   correct docID and count
  * Notes:
  *   if webpage or index is NULL, function does nothing
  */
 void indexPage(webpage_t* page, const int docID, hashtable_t* index);
 

 /**************** index_add ****************/
 /* Add a word occurrence to the index.
  *
  * Caller provides:
  *   valid pointer to an index, valid string for word (must not be NULL),
  *   valid docID (must be > 0)
  * We return:
  *   false if index or word is NULL, docID <= 0, or memory allocation fails
  *   true if successful
  * We do:
  *   increment the count of 'word' in document 'docID'
  *   if the word is not in the index, add it with count=1 for the given docID
  *   if the word is in the index but not for this docID, add docID with count=1
  *   if the word is in the index for this docID, increment the count
  */
 bool index_add(hashtable_t* index, const char* word, const int docID);

 
 /**************** save_index_toFile ****************/
 /* Save the index to a file in a specified format.
  *
  * Caller provides:
  *   valid pointer to an index, and FILE open for writing
  * We return:
  *   false if index or file pointer is NULL
  *   true if index is successfully saved
  * We guarantee:
  *   output file has one word per line with format:
  *   word docID count [docID count]...
  * Notes:
  *   words will be lowercase letters
  *   docIDs and counts will be positive non-zero integers
  *   items are separated by spaces
  */
 bool saveIndex_toPage(hashtable_t* index, char* filepath);


/**************** load_index ****************/
/* Load an index from a file into a new index data structure.
 * The file format must be:
 *   word docID count [docID count]...
 * where word is a string of lowercase letters and docID and count are positive integers.
 * 
 * We return:
 *   A pointer to a new hashtable containing the index;
 *   NULL on error
 * Notes:
 *   Caller is responsible for later calling index_delete().
 */
 hashtable_t* load_index(FILE* fp, int size);

 
 /**************** parse_index_line ****************/
/* 
 * Parse a line from an index file and add the word and its documents to the index.
 * 
 * Caller provides:
 *   line - a line from an index file in format "word docID count [docID count]..." (must not be NULL)
 *   index - the hashtable representing the index (must not be NULL)
 *
 * We do:
 *   Make a copy of the line
 *   Extract the word (first token)
 *   Extract each docID-count pair
 *   Add each document occurrence to the index
 *   Free the line copy when done
 *
 * Notes:
 *   Silently returns if line or index is NULL
 *   Silently ignores malformed lines or invalid docID/count values
 *   Uses strtok() to parse the line, which modifies the string
 *   Only counts with positive values are added to the index
 */
 void parse_index_line(char* line, hashtable_t* index);


  /**************** index_delete ****************/
 /* Delete the entire index, freeing all memory it occupies.
  *
  * Caller provides:
  *   valid pointer to an index
  * We do:
  *   free all memory associated with this index, including the 
  *   hashtable, all words, all counters, and the index itself
  * Notes:
  *   we ignore NULL index
  */
 void index_delete(hashtable_t* index);
 
 #endif // __INDEX_H