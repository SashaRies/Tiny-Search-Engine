/*
Author: Sasha Ries
Date: 2/17/25
File: index.c
Description: (CS-50) Module to implement an index data structure.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "index.h"
#include "mem.h"
#include "hashtable.h"
#include "webpage.h"
#include "counters.h"
#include "word.h"
#include "pagedir.h"
#include "file.h"

/*------------------------------------------------- Local Functions --------------------------------------------------*/
static void print_word_counters(void* arg, const char* word, void* item);
static void print_docID_count(void* arg, const int docID, const int count);
static void counters_delete_wrapper(void* item);


/*----------------------------------------------- Global Functions ----------------------------------------------------*/
hashtable_t* indexBuild(char* pageDirectory){
    hashtable_t* index = hashtable_new(700); // Create the index data structure (initial size of 700)

    // Loop until we can't open a file (end of crawled files)
    for (int docID = 1; ; docID++) {
        // Construct the filepath: dpageDirectory/docID
        int path_len = strlen(pageDirectory) + 20;  // Store the buffer length for filepath string
        char* filepath = malloc(path_len); // Allocate memory for the filepath string

        if (filepath == NULL) {
            continue;
        }
        snprintf(filepath, path_len, "%s/%d", pageDirectory, docID); // Write the full path to filepath

        FILE* fp = fopen(filepath, "r"); // Try to open the file

        if (docID == 1 && fp == NULL) { // If we can't open the file, we've reached the end
            fprintf(stderr, "Error: Unable to open file %s\n", filepath);
            mem_free(filepath);  // Free memory
            return NULL; 
        }else if (fp == NULL){ //We've reached the end
            mem_free(filepath);  // Free memory
            return index;
        }

        webpage_t* page; // Now create a webpage instance
        if ((page = webpage_create_fromFile(fp)) == NULL){
            fprintf(stderr, "Error: failed to read from file %s\n", filepath);
            mem_free(filepath);  // Free memory
            continue;
        }

        // Fill the indedx with data from webpage_t page (URL (string), pagedepth (int), HTML (string))
        indexPage(page, docID, index);
        mem_free(filepath);  // Done with filepath
        fclose(fp); // Close the file
    }
}


void indexPage(webpage_t* page, const int docID, hashtable_t* index){

    int pos = 0; // Create position indicator (pos) to keep track of where we are in page->HTML
    char* word; // Create string pointer for storing the word

    // Loop through each word of the webpage
    while ((word = webpage_getNextWord(page, &pos)) != NULL) {
        if (strlen(word) < 3) { // Skip words that are shorter than 3 characters
            free(word);
            continue;
        }

        // Normalize the word
        if (!word_normalize(word)) {
            free(word);
            continue;
        }

        // Add to to the hashtable index (will either create a new counters instance or increment an existing one)
        if (!index_add(index, word, docID)){
            fprintf(stderr, "Failed to add to index");
        }
        free(word); // Free memory since hashtable_t creates a copy of each word
    }
}


bool index_add(hashtable_t* index, const char* word, const int docID){
    if (index == NULL || word == NULL || docID < 0) { // Validate parameters
        return false;
    }

    // Find the counters for this word, or create if needed
    counters_t* ctrs = hashtable_find(index, word);
    
    if (ctrs == NULL) { // This word (key) does not exist yet in index hashtable; create new counters
        ctrs = counters_new();
        if (ctrs == NULL) {// Out of memory or other error
            return false;
        }
        
        // Insert the counters struct instance into the hashtable index at the key (word)
        if (!hashtable_insert(index, word, ctrs)) { // Failed to insert
            counters_delete(ctrs);
            return false;
        }
    }
    // Use the stored pointer to counters to increment a value associated with docID
    counters_add(ctrs, docID); // word: (docID, count++)
    return true;
}


bool saveIndex_toPage(hashtable_t* index, char* filepath){
    if (index == NULL || filepath == NULL) { // Validate parameters
        return false;
    }
    FILE* fp = fopen(filepath, "w"); // Try to open the file

    if (fp == NULL){
        return false;
    }    

    // Use the hashtable_iterate function with our custom print function
    hashtable_iterate(index, fp, print_word_counters);
    fclose(fp);
    return true;
}

hashtable_t* load_index(FILE* fp, int size) {
    if (fp == NULL) {
        return NULL;
    }
 
    // Create a new hashtable for the index
    hashtable_t* index = hashtable_new(size); // Size chosen based on expected number of words
    if (index == NULL) {
        return NULL;
    }
 
    char* line = NULL;
 
    // Read the file line by line
    while ((line = file_readLine(fp)) != NULL) {      
        // Parse the line and add to index
        parse_index_line(line, index);
        mem_free(line);
    }
    return index;
}

void parse_index_line(char* line, hashtable_t* index){
    if (line == NULL || index == NULL) {
        return;
    }

    // Make a copy of the line
    char* lineCopy = mem_malloc(strlen(line) + 1);
    strcpy(lineCopy, line);

    if (lineCopy == NULL) {
        return;
    }

    // Get the word (first token)
    char* word = strtok(lineCopy, " ");
    if (word == NULL) {
        free(lineCopy);
        return;
    }

    // Process all docID-count pairs
    char* token;
    while ((token = strtok(NULL, " ")) != NULL) {
        // Get docID
        int docID = atoi(token);
        
        // Get count
        token = strtok(NULL, " ");
        if (token == NULL) {
            break;  // Malformed line
        }
        int count = atoi(token);

        // If both docID and count are valid (positive numbers)
        if (docID > 0 && count > 0) {
            // Add to index - assumes index_add handles creating counters if needed
            index_add(index, word, docID);
        }
    }

    mem_free(lineCopy);
}


void index_delete(hashtable_t* index){
    // Call hashtable_delete and pass in counters_delete as the function pointer
    hashtable_delete(index, counters_delete_wrapper);
}


/* ----------------------------------------- Local Helper functions --------------------------------------------------*/
static void print_word_counters(void* arg, const char* word, void* item){
    FILE* fp = arg;
    counters_t* ctrs = item;
    
    if (fp != NULL && word != NULL && ctrs != NULL) {
        // Print the word
        fprintf(fp, "%s", word);
        
        // Use counters_iterate to print all docID/count pairs
        counters_iterate(ctrs, fp, print_docID_count);
        
        // End the line
        fprintf(fp, "\n");
    }
}

static void print_docID_count(void* arg, const int docID, const int count){
    FILE* fp = arg;
    if (count > 0) {
        fprintf(fp, " %d %d", docID, count);
    }
}

// Wrapper function to make counters_delete compatible with hashtable_delete
static void counters_delete_wrapper(void* item) {
    // Cast the void* back to counters_t* before passing to counters_delete
    counters_delete((counters_t*) item);
}
