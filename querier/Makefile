# Makefile for CS50 Querier (Tiny Search Engine)
# Author: Sasha Ries
# Date: 2/25/25

CC = gcc

# Compiler flags:
# -Wall:      show all warnings
# -pedantic:  require strict ISO C compliance
# -std=c11:   use C11 standard
# -ggdb:      include debugging symbols for gdb
CFLAGS = -Wall -pedantic -std=c11 -ggdb

COMMON_PATH = ../common/

# Include paths:
# -I../libcs50 for hashtable.h, counters.h, file.h and other libcs50 headers
# -I.. for the common directory (index.h, pagedir.h, word.h, query.h), because inludes already have common/ in c files
INCLUDES = -I../libcs50 -I..

# Library path for libcs50.a
LIBS = -L../libcs50 -lcs50

# Programs to build
PROG = querier

# Build PROG by default
all: $(PROG)

# The querier program - depends on common module objects
querier: querier.c $(COMMON_PATH)query.o $(COMMON_PATH)index.o $(COMMON_PATH)pagedir.o $(COMMON_PATH)word.o
	$(CC) $(CFLAGS) $(INCLUDES) querier.c $(COMMON_PATH)query.o $(COMMON_PATH)index.o $(COMMON_PATH)pagedir.o $(LIBS) $(COMMON_PATH)word.o -o querier


.PHONY: all clean test

# Test target - runs test script and saves output
test: $(PROG)
	bash testing.sh &> testing.out

# Clean target - remove all generated files
clean:
	rm -f $(PROG)
	rm -rf test-files
	rm -f *~ *.o $(COMMON_PATH)*.o
	rm -rf invalid query_test_output