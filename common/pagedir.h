/*
Author: Sasha Ries
Date: 2/17/25
File: pagedir.h
Description: header file for CS50 pagedir module
*/

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdbool.h>
#include "webpage.h"  // for webpage_t type

/* Initialize the page directory by creating the .crawler file.
 * Return true if successful, false otherwise.
 */
bool pagedir_init(const char* pageDirectory);


/* Save a webpage to a file.
 * Parameters:
 *   page - the webpage to save
 *   pageDirectory - the directory where to save the page
 *   docID - the document ID number to use as the file name
 * Returns: nothing
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);


/* Check if the pageDirectory argument , dir_path, is a valid directory created by the crawler.
 * Parameters:
 *   dir_path - the directory to check
 * Return true if the directory has an openable .crawler file, false otherwise.
 */
bool is_crawler_directory(const char* dir_path);


/**************** webpage_create_fromFile **********************/
/* Allocates and initializes a new webpage_t structure with the URL, depth, and HTML fields filled
* in from the file pointer fp
* 
* Caller provides:
* file pointer FILE* fp
*
* We return:
*   pointer to new webpage_t, or NULL on any error.
*
* Caller is responsible for:
*   later calling webpage_delete with the returned pointer.
*/
webpage_t* webpage_create_fromFile(FILE* fp);


/**************** get_url ****************/
/* 
 * Retrieve the URL for a document from its file in the page directory.
 * 
 * Caller provides:
 *   pageDirectory - path to the directory containing the crawled pages (must not be NULL)
 *   docID - the ID of the document to retrieve (must be > 0)
 *
 * Returns:
 *   The URL as a newly allocated string, or NULL on error
 *
 * Notes:
 *   Caller is responsible for freeing the returned URL string
 */
char* get_url(char* pageDirectory, int docID);


#endif // __PAGEDIR_H