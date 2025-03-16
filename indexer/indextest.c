/*
Author: Sasha Ries
Date: 2/17/25
File: indexetest.c
Description:
 * The indextest program loads an index file created by the indexer,
 * rebuilds the index data structure, and writes it to a new file.
 * This serves as both a test of the indexer's file format and the
 * index save/load functionality.
 */

 #define _GNU_SOURCE
 #include <stdio.h> 
 #include <stdlib.h>
 #include <string.h>
 #include "common/index.h"
 #include "common/pagedir.h"
 #include "common/word.h"
 #include "hashtable.h"
 #include "file.h"
 #include "common/index.h"
 
 
 int main(int argc, char* argv[]) 
 {
     // Verify command line arguments
     if (argc != 3) {
         fprintf(stderr, "Usage: %s oldIndexFilename newIndexFilename\n", argv[0]);
         return 1;
     }
 
     // Extract filenames from command line
     char* oldIndexFilename = argv[1];
     char* newIndexFilename = argv[2];
 
     // Try to open the old index file for reading
     FILE* oldFile = fopen(oldIndexFilename, "r");
     if (oldFile == NULL) {
         fprintf(stderr, "Error: cannot open %s for reading\n", oldIndexFilename);
         return 2;
     }
 
     // Load the index from the old file
     hashtable_t* index = load_index(oldFile, 500);
     fclose(oldFile);
     
     if (index == NULL) {
         fprintf(stderr, "Error: failed to load index from %s\n", oldIndexFilename);
         return 3;
     }
 
     // Try to open the new file for writing
     FILE* newFile = fopen(newIndexFilename, "w");
     if (newFile == NULL) {
         fprintf(stderr, "Error: cannot open %s for writing\n", newIndexFilename);
         index_delete(index);
         return 4;
     }
 
     // Write the index to the new file
     if (!saveIndex_toPage(index, newIndexFilename)) {
         fprintf(stderr, "Error: failed to write index to %s\n", newIndexFilename);
         fclose(newFile);
         index_delete(index);
         return 5;
     }
 
     // Clean up
     fclose(newFile);
     index_delete(index);
     return 0;
 }
 
 /*
  * Load an index from a file into a new index data structure.
  * The file format must be:
  *   word docID count [docID count]...
  * where word is a string of lowercase letters and docID and count are positive integers.
  * 
  * We return:
  *   A pointer to a new hashtable containing the index;
  *   NULL on error
  */
 
 /*
  * Parse a line from the index file and add its contents to the index.
  * Line format: word docID count [docID count]...
  * 
  * We do not return anything, but we add the word and its occurrences to the index.
  */