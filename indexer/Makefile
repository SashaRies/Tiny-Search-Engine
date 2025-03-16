# Makefile for CS50 Indexer (Tiny Search Engine)
# Author: Sasha Ries
# Date: 2/17/25

CC = gcc

# Compiler flags:
# -Wall:      show all warnings
# -pedantic:  require strict ISO C compliance
# -std=c11:   use C11 standard
# -ggdb:      include debugging symbols for gdb
CFLAGS = -Wall -pedantic -std=c11 -ggdb

COMMON_PATH = ../common/

# Include paths:
# -I../libcs50 for hashtable.h, counters.h, webpage.h and other libcs50 headers
# -I.. for the common directory (index.h, pagedir.h, word.h), because inludes already have common/ in c files
INCLUDES = -I../libcs50 -I..

# Library path for libcs50.a
LIBS = -L../libcs50 -lcs50

# Programs to build
PROGs = indexer indextest

# Build PROG by default
all: $(PROGs)

# The indexer program - depends on common module objects
indexer: indexer.c
	$(CC) $(CFLAGS) $(INCLUDES) indexer.c $(COMMON_PATH)index.o $(COMMON_PATH)pagedir.o $(COMMON_PATH)word.o $(LIBS) -o indexer


# The indextest program - depends on common module objects
indextest: indextest.c
	$(CC) $(CFLAGS) $(INCLUDES) indextest.c $(COMMON_PATH)index.o $(COMMON_PATH)pagedir.o $(COMMON_PATH)word.o $(LIBS) -o indextest

.PHONY: all clean test

# Test target - runs test script and saves output
test: $(PROGs)
	bash testing.sh &> testing.out

# Clean target - remove all generated files
clean:
	rm -f $(PROGs)
	rm -rf test-files
	rm -f *~ *.o
	rm -rf invalid index_dir