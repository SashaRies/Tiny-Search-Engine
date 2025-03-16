/*
Author: Sasha Ries
Date: 2/18/25
File: word.c
Description: (CS-50) Module to implement functions for normalizing strings (words).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


bool word_normalize(char* word){
    if (word == NULL) { // Check for NULL pointer
        return false;
    }
    
    // Iterate through each character and convert to lowercase
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] >= 'A' && word[i] <= 'Z') {
            word[i] = word[i] - 'A' + 'a';
        }
    }
    
    return true;
}


char* str_readWord(const char* str, int* pos){
    if (str == NULL || pos == NULL) {
        return NULL;
    }

    // Skip leading spaces
    while (str[*pos] != '\0' && (str[*pos] == ' ' || str[*pos] == '\n' || str[*pos] == '\t')) {
        (*pos)++;
    }

    // If we reached the end of the string, return NULL
    if (str[*pos] == '\0') {
        return NULL;
    }

    // Allocate buffer for the word
    int len = 81;  // Initial buffer size
    char* buf = malloc(len * sizeof(char));
    if (buf == NULL) {
        return NULL;
    }

    // Read characters until space, newline, or end of string
    int word_pos = 0;
    
    while (str[*pos] != '\0' && str[*pos] != ' ' && str[*pos] != '\n' && str[*pos] != '\t') {
        // Grow the buffer if needed
        if (word_pos + 1 > len - 1) {
            char* newbuf = realloc(buf, ++len * sizeof(char));
            if (newbuf == NULL) {
                free(buf);
                return NULL;
            } else {
                buf = newbuf;
            }
        }
        
        // Add the character to our word buffer
        buf[word_pos++] = str[*pos];
        (*pos)++;
    }

    // Skip the space or newline that terminated the word
    if (str[*pos] != '\0') {
        (*pos)++;
    }

    // Terminate the string
    buf[word_pos] = '\0';
    
    return buf;
}