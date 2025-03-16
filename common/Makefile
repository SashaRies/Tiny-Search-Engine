# Makefile for CS50 common module (Tiny Search Engine)
# Author: Sasha Ries
# Date: 2/10/25 
# This Makefile builds pagedir.o for use by other modules

# Compiler configuration
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

# Object files
OBJS = pagedir.o index.o word.o query.o

INCLUDES = -I../libcs50

# Main target - we only need the object file, not a library
all: $(OBJS)

# Build pagedir.o
pagedir.o: pagedir.h pagedir.c
	$(CC) $(CFLAGS) $(INCLUDES) -c pagedir.c

# Build index.o
index.o: index.h index.c word.h word.c pagedir.h pagedir.c
	$(CC) $(CFLAGS) $(INCLUDES) -c index.c

# Build word.o
word.o: word.h word.c
	$(CC) $(CFLAGS) $(INCLUDES) -c word.c

# Build query.o
query.o: query.h query.c word.h word.c pagedir.h pagedir.c
	$(CC) $(CFLAGS) $(INCLUDES) -c query.c


.PHONY: clean

# Remove generated files
clean:
	rm -f $(OBJS)
	rm -f *~