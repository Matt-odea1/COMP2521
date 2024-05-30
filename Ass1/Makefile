# COMP2521 Assignment 1 Makefile

# Only modify this file by changing CFLAGS if you need to
# for debugging.

CC = clang
CFLAGS0 = -Wall -Werror -g # if you want to use valgrind
CFLAGS1 = -Wall -Werror -g -fsanitize=address,leak,undefined
CFLAGS2 = -Wall -Werror -g -fsanitize=memory,undefined
# We will compile with CFLAGS1
# Use CFLAGS0 if using valgrind, or CFLAGS2 if using gdb
CFLAGS = $(CFLAGS1)

.PHONY: all
all: testBBST

testBBST: bBST.o List.o testBBST.o
	$(CC) $(CFLAGS) -o testBBST bBST.o List.o testBBST.o

.PHONY: clean
clean:
	rm -f *.o testBBST

