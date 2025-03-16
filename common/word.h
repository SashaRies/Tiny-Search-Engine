/*
Author: Sasha Ries
Date: 2/17/25
File: index.h
Description: header file for CS50 index module
*/

#ifndef __WORD_H
#define __WORD_H
#include <stdio.h>
#include <stdbool.h>

/**************** word_normalize ****************/
/* Normalize a word by converting it to all lowercase.
 *
 * Caller provides:
 *   valid pointer to a null-terminated string
 * We return:
 *   true if normalization was successful
 *   false if word is NULL
 * We guarantee:
 *   original string is modified in place if successful
 * Notes:
 *   only modifies uppercase letters A-Z, converting them to lowercase a-z
 *   other characters (numbers, symbols, etc.) are unchanged
 */
bool word_normalize(char* word);

/**************** str_readWord ****************/
/* Read characters from a string until a space or newline is encountered.
 * Returns a newly allocated string containing the word (without spaces).
 * Skips any leading spaces to find the actual start of the word.
 * 
 * Parameters:
 *   str - the source string to read from
 *   pos - pointer to an integer tracking the current position in the string
 *         will be updated to the position after the space/newline
 * 
 * Returns:
 *   A pointer to a new string containing the word; caller must free() later.
 *   Returns NULL if no word was found (empty string or only spaces) or on error.
 */
char* str_readWord(const char* str, int* pos);

#endif // __WORD_H