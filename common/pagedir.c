/*
Author: Sasha Ries
Date: 2/9/25
File: pagedir.c
Description: A module for a Tiny Search Engine Crawler
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pagedir.h"
#include "webpage.h"
#include "file.h"
#include "mem.h"     // for mem_assert

/* Function to create the hidden file .crawler in the "pageDirectory: directory */
bool pagedir_init(const char* pageDirectory) {
    char* pathname = mem_malloc(strlen(pageDirectory) + 10); // Construct the pathname for the .crawler file
    sprintf(pathname, "%s/.crawler", pageDirectory);
    
    FILE* fp = fopen(pathname, "w"); // Open the file for writing
    mem_free(pathname);
    
    if (fp == NULL) { // Exit with false status if error
        return false;
    }
    
    fclose(fp); // Close new file
    return true;
}

/* Function to save a webpage's content, its URL, and the link recursion depth to the 
file "DOCid" in the "pageDirectory" directory*/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID) {
    char* pathname = mem_malloc(strlen(pageDirectory) + 50); // Construct the pathname for the page file

    // Create a string of the form pageDirectory/DOCid
    sprintf(pathname, "%s/%d", pageDirectory, docID); // Names the file DOCid and puts it in "pageDirectory"
    
    // Open the file for writing
    FILE* fp = fopen(pathname, "w");

    // Create an error message if the file cannot be opened
    char errorMessage[50]; // Buffer size of 50 bytes
    sprintf(errorMessage, "Cannot open file %s", pathname);
    mem_assert(fp, errorMessage); // Print error message if fp is NULL
    mem_free(pathname);
    
    fprintf(fp, "%s\n", webpage_getURL(page)); // Print the URL
    fprintf(fp, "%d\n", webpage_getDepth(page)); // Print the depth
    fprintf(fp, "%s", webpage_getHTML(page)); // Print the contents
    
    fclose(fp);
}


bool is_crawler_directory(const char* dir_path) {
    if (dir_path == NULL) {
        return false;
    }

    // Construct the path to the .crawler file
    int path_len = strlen(dir_path) + 12;  // +12 for "/.crawler\0"
    char* crawler_path = mem_malloc(path_len);
    
    if (crawler_path == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for %s/.crawler path\n", dir_path);
        return false;
    }

    // Write the entire file path for pageDirectory/.crawler
    snprintf(crawler_path, path_len, "%s/.crawler", dir_path);

    FILE* fp = fopen(crawler_path, "r"); // Try to open the file
    free(crawler_path);  // Done with the path string

    if (fp == NULL) {
        return false;  // File doesn't exist or can't be opened
    }

    fclose(fp); // Close the file
    return true;
}


webpage_t* webpage_create_fromFile(FILE* fp){
  if (fp == NULL) {
    return NULL;  // Validate input
  }

  // Local variables to hold parsed data
  char* url = NULL;
  int depth = 0;
  char* html = NULL;
  char* depthStr = NULL;

  // STEP 1: Read the URL (first line)
  url = file_readLine(fp);
  if (url == NULL) {
    return NULL;  // Error: couldn't read URL line
  }

  // STEP 2: Read the depth integer (second line)
  depthStr = file_readLine(fp);
  if (depthStr == NULL) {
    mem_free(url);  // Free previously allocated memory
    return NULL;  // Error: couldn't read depth line
  }

  // Convert string to integer
  if (sscanf(depthStr, "%d", &depth) != 1) {
    mem_free(url);   // Free previously allocated memory
    mem_free(depthStr);
    return NULL;  // Error: depth isn't a valid integer
  }
  mem_free(depthStr);  // Done with depth string

  // STEP 3: Read the HTML content (remaining lines)
  html = file_readFile(fp);
  if (html == NULL) {
    mem_free(url);
    return NULL;  // Error: couldn't read HTML content
  }

  // STEP 4: Create and return webpage structure
  webpage_t* webpage = webpage_new(url, depth, html);
  
  // If webpage creation failed, clean up
  if (webpage == NULL) {
    mem_free(url);
    mem_free(html);
  }
  
  return webpage;
}


char* get_url(char* pageDirectory, int docID){
    int path_len = strlen(pageDirectory) + 5;  // Store the buffer length for docID
    char* filepath = malloc(path_len); // Allocate memory for the filepath string
    sprintf(filepath, "%s/%d", pageDirectory, docID); // Create the filepath
    
    FILE* fp = fopen(filepath, "r");
    char* URL = file_readLine(fp); // Read in URL from first line of the file
    mem_free(filepath); // Free URL
    fclose(fp);
    return URL;
}