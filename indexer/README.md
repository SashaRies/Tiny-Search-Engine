# TSE Indexer
### Author: Sasha Ries

The TSE (Tiny Search Engine) Indexer creates a searchable index from web pages previously collected by the crawler. It reads each crawled page, extracts words from the HTML content, and builds an inverted index mapping each word to all the documents where it appears, along with word occurrence counts.

## Purpose

The indexer enables efficient text search across a collection of web pages by:
1. Processing and normalizing words from crawled documents
2. Creating a data structure that records where each word appears
3. Saving this information in a format that can be quickly loaded and searched

## Implementation

The indexer consists of several key components working together:

### Main Indexer (`indexer.c`)

The indexer follows these core steps:
1. Validates command-line arguments
2. Verifies the crawler directory contains valid page files
3. Builds an inverted index data structure in memory
4. Writes the completed index to a file

### Index Module (`index.c`)

This module provides the core indexing functionality:
- Creates and manages the inverted index data structure
- Processes individual web pages to extract words
- Tracks word frequencies across documents
- Provides functions for saving/loading index data

### File Format:
word docID1 count docID1 count docID3...

### One thing to note:
In the testing script the there was a count difference of 1 between the indextest and index produced file for the word "home" at docID 3. 
It is shown in the testing.out file, and I could not figure out why. Aside from that everything works normally and without problems based on my tests.