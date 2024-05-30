# COMP2521 Assignment 2 Makefile
#
# HOW TO USE
# Read all the comments below and edit the Makefile accordingly

CC = clang

CFLAGS0 = -Wall -Werror -g
CFLAGS1 = -Wall -Werror -g -fsanitize=address,leak,undefined
CFLAGS2 = -Wall -Werror -g -fsanitize=memory,undefined

# If you want to use GDB, change this to $(CFLAGS0)
# If you want to use the MemorySanitizer, change this to $(CFLAGS2)
# - MemorySanitizer detects uninitialized memory accesses
# You should change back to $(CFLAGS1) when you are done
CFLAGS = $(CFLAGS1)

# List all your supporting .c files here. Do NOT include .h files in this list.
# Example: SUPPORTING_FILES = hello.c world.c
SUPPORTING_FILES =

.PHONY: all
all: testWall testClimber

testWall: testWall.c wallReader.c Wall.c $(SUPPORTING_FILES)
	$(CC) $(CFLAGS) -o testWall testWall.c wallReader.c Wall.c $(SUPPORTING_FILES) -lm

testClimber: testClimber.c climber.c wallReader.c Wall.c $(SUPPORTING_FILES)
	$(CC) $(CFLAGS) -o testClimber testClimber.c climber.c wallReader.c Wall.c $(SUPPORTING_FILES) -lm

.PHONY: clean
clean:
	rm -f testWall testClimber

