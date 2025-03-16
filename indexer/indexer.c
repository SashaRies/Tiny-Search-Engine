/*
Author: Sasha Ries
Date: 2/17/25
File: indexer.c
Description: This file contains the main function for the indexer program. 
The indexer program reads in a file of URLs and indexes the words in the URLs. The program then writes the index to a file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "common/index.h"
#include "common/pagedir.h"



int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of arguments -> Usage: %s inverted-index-file output-file\n", argv[0]);
        return 1; // Exit status 1 for issues with number of arguments
    }

    char* pageDirectory = argv[1];
    char* indexFilename = argv[2];

    // Validate pageDirectory (must exist and is crawler generated)
    if (!is_crawler_directory(pageDirectory)) {
        fprintf(stderr, "Error: invalid pageDirectory %s\n", pageDirectory);
        return 2; // Exit status 2 for issues with pageDirectory/.crawler
    }

    // Build the index from files in pageDirectory
    hashtable_t* index = indexBuild(pageDirectory); // indexBuild will have printed the error statements
    if (index == NULL){
        return 3; // Exit status 3 for issues reading files from pageDirectory
    }

    // Save the index to indexFilename
    if (!saveIndex_toPage(index, indexFilename)) {
        fprintf(stderr, "Error: unable to write index to '%s' for writing\n", indexFilename);
        index_delete(index);
        return 4; // Exit status 4 for issues with indexFilename
    }

    // Clean up
    index_delete(index);
    return 0; // Exit status 0 for no issues
}