# TSE Crawler
### Author: Sasha Ries

The TSE (Tiny Search Engine) Crawler is a C program that systematically explores and downloads web pages from a specified seed URL, saving the content for later indexing and searching. Starting from a seed URL, it recursively retrieves pages and follows their embedded links up to a specified depth limit.

## Implementation 

The crawler is implemented with three main components:

### Main Crawler Logic (`crawler.c`)
The main crawler program follows these steps:
1. Parses command-line arguments and validates inputs
2. Initializes data structures (hashtable for tracking visited URLs, bag for managing URLs to visit)
3. Begins crawling from the seed URL
4. For each discovered page:
   - Fetches and saves the page content
   - Extracts and normalizes embedded URLs
   - Adds new internal URLs to the crawling queue if within depth limit

### Page Directory Module (`pagedir.c`)
Handles all file operations related to saving crawled pages:
- Creates a `.crawler` marker file to identify directories the crawler is writing files to
- Saves each webpage with a unique ID as filename

#### File format
- URL (string)
- depth (integer) 
- HTML content (raw HTML)

## Assumptions
"pageDirectory" must already exist for the crawler to access it

## Usage
```bash
./crawler seedURL pageDirectory maxDepth