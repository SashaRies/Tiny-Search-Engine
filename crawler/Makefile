# Makefile for CS50 Crawler (Tiny Search Engine)
# Author: Sasha Ries
# Date: 2/10/25 

CC = gcc

# Compiler flags:
# -Wall:      show all warnings
# -pedantic:  require strict ISO C compliance
# -std=c11:   use C11 standard
# -ggdb:      include debugging symbols for gdb
CFLAGS = -Wall -pedantic -std=c11 -ggdb


#   -I.        look for header files in current directory
#   -I..       look in parent directory
#   -L.        look for libraries in current directory
#   -lcs50     link with libcs50.a (drops 'lib' prefix and '.a' suffix)


# Include paths:
# -I../libcs50 for mem.h, bag.h, hashtable.h, webpage.h
# -I.. for the common directory (pagedir.h)
INCLUDES = -I../libcs50 -I..

# Library path for libcs50.a
LIBS = -L../libcs50 -lcs50

# The target executable
PROG = crawler

# Build the crawler program
$(PROG): crawler.c
	$(CC) $(CFLAGS) $(INCLUDES) crawler.c ../common/pagedir.o $(LIBS) -o $(PROG)


.PHONY:	clean test

test: $(PROG)
	bash ./testing.sh > testing.out 2>&1

# Clean target
clean:
	rm -f crawler
	rm -f *~ *.o

