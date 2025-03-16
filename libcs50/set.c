/*
Author: Sasha Ries
Date: 1/26/25
File: set.c
Description: (CS-50) Module to implement a set data structure.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"
#include "mem.h"

/**************** local types ****************/
typedef struct node {
    void* item; // Pointer to arbitrary data
    char* key; // Pointer to character based key
    struct node* next; // Pointer to next node
} node_t;


/**************** global types ****************/
typedef struct set {
    struct node* head;  // Pointer to first node in list   
    size_t size;      // Current number of elements
} set_t;


/**************** local functions ****************/
static node_t* make_node(const char* key, void* item); // Function to create a new node


/**************** global functions ****************/
set_t* set_new(void) {
  set_t* set = mem_malloc(sizeof(set_t)); // Allocate memory to store all values from the set struct

  if (set == NULL) {
    return NULL;              // error allocating set
  } else {
    // initialize contents of set structure
    set->head = NULL; // Point head of set to null
    set->size = 0; // Number of nodes in set
    return set;
  }
}

bool set_insert(set_t* set, const char* key, void* item) {
    if (set != NULL && key != NULL && item != NULL){
        // Check if key exists
        for (node_t* node = set->head; node != NULL; node = node->next) {
            if (strcmp(node->key, key) == 0) { // Check if keys are equal
                return false; // Key exists already
            } 
        }

        node_t* new = make_node(key, item); // allocate a new node to be added to the list

        if (new == NULL) {
            return false;
        }
        
        new->next = set->head;
        set->head = new; // add it to the head of the list
        set->size++;
        return true;
    }
    return false; // Default if a new node doesnt get added and true returned
}

void* set_find(set_t* set, const char* key){
    for (node_t* node = set->head; node != NULL; node = node->next) {
        if (strcmp(node->key, key) == 0) { // Check if keys are equal
            return node->item;
        } 
    }
    return NULL;
}

void set_iterate(set_t* set, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) ) {
    if (set != NULL || itemfunc != NULL) {
        // call itemfunc with arg, on each item
        for (node_t* node = set->head; node != NULL; node = node->next) {
        (*itemfunc)(arg, node->key, node->item); 
        }
    }
}

void set_print(set_t* set, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item)) {
    if (fp != NULL) {
        if (set != NULL) {
            fputc('{', fp);
            for (node_t* node = set->head; node != NULL; node = node->next) {
                // print this node
                if (itemprint != NULL) { // print the node's item 
                    (*itemprint)(fp, node->key, node->item); 
                    if (node->next != NULL) {  // Only print comma if not last item
                        fputs(", ", fp);
                    }
                }
            }
            fputc('}', fp);
        } else {
            fputs("(null)", fp);
        }
    }
}

void set_delete(set_t* set, void (*itemdelete)(void* item) ){
    if (set != NULL) {
        node_t* node = set->head;
        while (node != NULL) {
            node_t* next = node->next;  // Save the next pointer before freeing
            
            // Free the key string that was allocated with mem_malloc
            if (node->key != NULL) {
                mem_free(node->key);
            }

            // Delete the item if deletion function provided
            if (itemdelete != NULL && node->item != NULL) {
                (*itemdelete)(node->item);
            }

            mem_free(node);                // Free current node
            node = next;                   // Move to saved next node
        }
        mem_free(set);                      // Free the set structure pointer
    }
}


/******************* local function *******************/
static node_t* make_node(const char* key, void* item) {
    node_t* node = mem_malloc(sizeof(node_t));

    if (node == NULL) {
        return NULL; // Error allocating memory for node; return error
    }
    // Make a copy of the key
    node->key = mem_malloc(strlen(key) + 1);  // +1 for null terminator
    if (node->key == NULL) {
        mem_free(node);  // Clean up if key allocation fails
        return NULL;
    }
    strcpy(node->key, key);
    
    node->item = item;
    node->next = NULL;
    return node;
}