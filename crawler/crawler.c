/*
Author: Sasha Ries
Date: 2/9/25
File: crawler.c
Description: A module for a Tiny Search Engine Crawler
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep((x)*1000)
#else
#include <unistd.h>
#endif
#include "mem.h"
#include "bag.h"
#include "hashtable.h"
#include "webpage.h"
#include "common/pagedir.h"

/* Local functions */
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);


/* Main function to start the crawler given the arguments: seedURL, pageDirectory, and maxDepth */
int main(const int argc, char* argv[]) {
    // Initialize variables
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;

    // Parse the arguments and start the crawler
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth); // Pass pointers to the arguments
    crawl(seedURL, pageDirectory, maxDepth);

    // Free allocated memory for pageDirectory since seedURL is already freed in crawl
    mem_free(pageDirectory);
    return 0;
}


/* Function to check if enough arguments are passed in, normalize URL, initalize page directory, 
and check if maxDepth is within range 0-10 */
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) {
    if (argc != 4) { // Check number of arguments
        fprintf(stderr, "Usage: %s seedURL pageDirectory maxDepth\n", argv[0]);
        exit(1);
    }

    // Normalize seedURL and validate that it is an internal and valid URL
    *seedURL = normalizeURL(argv[1]);
    if (*seedURL == NULL || !isInternalURL(*seedURL)) {
        fprintf(stderr, "Error: invalid or non-internal URL '%s'\n", argv[1]);
        exit(2);
    }

    // Copy and validate pageDirectory
    *pageDirectory = mem_malloc(strlen(argv[2]) + 1);
    strcpy(*pageDirectory, argv[2]);
    if (!pagedir_init(*pageDirectory)) {
        fprintf(stderr, "Error: unable to initialize page directory '%s'\n", *pageDirectory);
        exit(3);
    }

    // Parse and validate maxDepth
    *maxDepth = atoi(argv[3]);
    if (*maxDepth < 0 || *maxDepth > 10) {
        fprintf(stderr, "Error: maxDepth must be between 0 and 10\n");
        exit(4);
    }
}

/* Function to scan each webpage --------- */
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth) {
    // Initialize data structures
    hashtable_t* pagesSeen = hashtable_new(200);
    bag_t* pagesToCrawl = bag_new();
    int docID = 1;
    
    hashtable_insert(pagesSeen, seedURL, ""); // Add seedURL to hashtable
    webpage_t* seedPage = webpage_new(seedURL, 0, NULL); // Create a pointer to webpage_t struct for the seedURL
    bag_insert(pagesToCrawl, seedPage); // Insert pointer to seedpage into bag
    
    // Crawl each page in the bag untill empty
    webpage_t* page;
    while ((page = bag_extract(pagesToCrawl)) != NULL) {
        if (webpage_fetch(page)) { // Fetch the page HTML code using the webpage module
            pagedir_save(page, pageDirectory, docID++); // Save page to directory
            
            // If not too deep, scan for more URLs
            if (webpage_getDepth(page) < maxDepth) {
                pageScan(page, pagesToCrawl, pagesSeen); // Adds new pages to the bag if URLs found
            }
        }
        webpage_delete(page); // Clear allocated memory for the webpage
    }
    
    // Free allocated memory for each structure
    hashtable_delete(pagesSeen, NULL);
    bag_delete(pagesToCrawl, NULL);
}

/* **************************************** */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {
    int pos = 0;
    char* url;

    while ((url = webpage_getNextURL(page, &pos)) != NULL) {
        char* normalURL = normalizeURL(url); // Normalize the URL
        if (normalURL != NULL) {
            if (isInternalURL(normalURL)) { // Check URL is internal

                // Add URL key to hashtable, only true if not already in Hashtable
                if (hashtable_insert(pagesSeen, normalURL, "")) {
                    // Create copy of normalURL to pass to webpage_new
                    char* normalURL_copy = mem_malloc(strlen(normalURL) + 1);
                    strcpy(normalURL_copy, normalURL);

                    // Create new webpage_t struct with the normalized URL
                    webpage_t* newPage = webpage_new(normalURL_copy, webpage_getDepth(page) + 1, NULL);
                    bag_insert(pagesToCrawl, newPage); // Add new page to bag to be crawled
                } else {
                    mem_free(normalURL);  // Only free if we didn't give it to webpage_new
                }
            } else {
                mem_free(normalURL);  // Free if not internal
            }
        }
        mem_free(url);
    }
}