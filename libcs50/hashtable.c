/*
Author: Sasha Ries
Date: 1/28/25
File: hashtable.c
Description: (CS-50) Module to implement a hashtable data structure.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "hash.h" 
#include "hashtable.h"
#include "set.h"


/**************** global types ****************/
typedef struct hashtable {
    set_t** setlist; // Pointer to array of pointers to sets
    int size; // Size of the array
} hashtable_t;


/**************** global functions ****************/
hashtable_t* hashtable_new(const int num_slots) {
    hashtable_t* hashtable = mem_malloc(sizeof(hashtable_t)); // Create pointer to hashtable
    if (hashtable == NULL) {
        return NULL;              // error allocating hashtable
    } else {
        hashtable->setlist = mem_malloc(num_slots * sizeof(struct set_t*)); // Create pointer to array of pointers to sets
        if (hashtable->setlist == NULL) {
            mem_free(hashtable);  // clean up before returning
            return NULL;
        }
        
        // Initialize all slots to NULL
        for (int i = 0; i < num_slots; i++) {
            hashtable->setlist[i] = NULL;
        }
        
        hashtable->size = num_slots;
        return hashtable;
    }
}

bool hashtable_insert(hashtable_t* ht, const char* key, void* item){
    if (ht == NULL || key == NULL || item == NULL){
        return false;
    }

    unsigned long index = hash_jenkins(key, ht->size); // Get the index for the array
    if (ht->setlist[index] == NULL) { // No set at index
        set_t* newSet = set_new(); // Make new set and store pointer to it
        if (set_insert(newSet, key, item) == false ){
            return false; // Bad set or key
        }
        ht->setlist[index] = newSet; // Put pointer to new set in setlist array
        return true;
    } 
    // Set exists at index 
    return set_insert(ht->setlist[index], key, item); // return status of adding an element to the existing set
}

void* hashtable_find(hashtable_t* ht, const char* key){
    unsigned long index = hash_jenkins(key, ht->size); // Get the index for the array
    if (ht->setlist[index] == NULL) { // No set at index
        return NULL;
    } 
    // Set exists at index 
    return set_find(ht->setlist[index], key); // return either null or value associated with key
}

void hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item)) 
{
    // Check for NULL file pointer
    if (fp == NULL) {
        ; // Do nothing
    } 
    else if (ht == NULL) {// Check for NULL hashtable
        fprintf(fp, "(null)\n"); // Print null hashtable
    }
    else if (itemprint == NULL) { // If no itemprint function provided, just print empty slots
        for (int i = 0; i < ht->size; i++) {
            fprintf(fp, "Slot %d: \n", i);
        }
    }
    else{
        // Print each slot and its contents
        for (int i = 0; i < ht->size; i++) {
            fprintf(fp, "Slot %d: ", i);
            
            if (ht->setlist[i] != NULL) {
                // Use the provided iterate function to print items in the set
                set_print(ht->setlist[i], fp, itemprint);
            }
            fprintf(fp, "\n");
        }
    }
}

void hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item)){
    if (ht != NULL && itemfunc != NULL) {
        // Loop through each set in the hashtable
        for (int i = 0; i < ht->size; i++) {
            if (ht->setlist[i] != NULL) {
                set_iterate(ht->setlist[i], arg, itemfunc);
            }
        }
    }
}

void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item)) {
    if (ht != NULL) {
        // Delete each set in the array
        for (int i = 0; i < ht->size; i++) {
            if (ht->setlist[i] != NULL) {
                set_delete(ht->setlist[i], itemdelete);
            }
        }
        // Free the array of set pointers
        mem_free(ht->setlist);
        // Free the hashtable structure itself
        mem_free(ht);
    }
}