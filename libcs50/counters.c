/*
Author: Sasha Ries
Date: 1/27/25
File: counters.c
Description: (CS-50) Module to implement a counter data structure.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "counters.h"
#include "mem.h"

/**************** local types ****************/
typedef struct counter {
    int count; // Count value
    int key; // Key value
    struct counter* next; // Pointer to next node
} counter_t;


/**************** global types ****************/
typedef struct counters {
    struct counter* head;  // Pointer to first node in list   
    size_t size;      // Current number of elements
} counters_t;


/**************** local functions ****************/
static counter_t* make_counter(int key, int count); // Function to create a new counter


/**************** global functions ****************/
counters_t* counters_new(void){
  counters_t* counters = mem_malloc(sizeof(counters_t)); // Allocate memory to store all values from the counter struct

  if (counters == NULL) {
    return NULL;              // error allocating counter
  } else {
    // initialize contents of counters structure
    counters->head = NULL; // Point head of counter to null
    counters->size = 0; // Number of counters
    return counters;
  }
}

int counters_add(counters_t* ctrs, const int key){
    if (ctrs != NULL && key >= 0){
        // Check if key exists
        for (counter_t* ctr = ctrs->head; ctr != NULL; ctr = ctr->next) {
            if (ctr->key == key) { // Check if keys are equal
                ctr->count++; // Increment counter
                return ctr->count;
            } 
        }
        counter_t* new = make_counter(key, 1); // Allocate a new node to be added to the list
        new->next = ctrs->head;
        ctrs->head = new; // Add it to the head of the list
        ctrs->size++;
        return new->count; // Should always be 1
    }
    return 0; // Default 0 if no counters created or incremented
}

int counters_get(counters_t* ctrs, const int key){
    if (ctrs != NULL && key >= 0){
        // Check if key exists
        for (counter_t* ctr = ctrs->head; ctr != NULL; ctr = ctr->next) {
            if (ctr->key == key) { // Check if keys are equal
                return ctr->count;
            }
        }
    }
    return 0; // Default to 0 if no key found or invalid arguments
}

bool counters_set(counters_t* ctrs, const int key, const int count){
    if (ctrs != NULL && key >= 0 && count >= 0){
        // Check if key exists
        for (counter_t* ctr = ctrs->head; ctr != NULL; ctr = ctr->next) {
            if (ctr->key == key) { // Check if keys are equal
                ctr->count = count; // Update counter value
                return true;
            } 
        }
        counter_t* new = make_counter(key, count); // Allocate a new node to be added to the list
        new->next = ctrs->head;
        ctrs->head = new; // Add it to the head of the list
        ctrs->size++;
        return true; // Should always be 1
    }
    return false; // Default false if no counters created, updated, or arguments invalid
}

void counters_print(counters_t* ctrs, FILE* fp){
    if (fp != NULL) {
        if (ctrs != NULL) {
            for (counter_t* ctr = ctrs->head; ctr != NULL; ctr = ctr->next) {
                // Print this counter
                fprintf(fp, " %d  %d", ctr->key, ctr->count);
            }
        } else {
            fputs("(null)", fp);
        }
    }
}

void counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count)){
    if (ctrs != NULL && itemfunc != NULL) {
        // Call itemfunc with arg, on each item
        for (counter_t* ctr = ctrs->head; ctr != NULL; ctr = ctr->next) {
        (*itemfunc)(arg, ctr->key, ctr->count); 
        }
    }
}

void counters_delete(counters_t* ctrs){
    if (ctrs != NULL) {
        counter_t* current = ctrs->head;
        while (current != NULL) {
            counter_t* next = current->next;  // Save the next pointer before freeing
            mem_free(current);                // Free current node
            current = next;                   // Move to saved next node
        }
        mem_free(ctrs);                      // Free the counters structure
    }
}


/******************* local function *******************/
static counter_t* make_counter(int key, int count){
    counter_t* counter = mem_malloc(sizeof(counter_t));

    if (counter == NULL) {
        // error allocating memory for counter; return error
        return NULL;
    } else {
        counter->key = key;
        counter->count = count; // Initialize any new counter with default count = 1
        counter->next = NULL;
        return counter;
    }
}