// A List ADT for integers

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef LIST_H
#define LIST_H

typedef struct list *List;

// Creates a new empty list
// Complexity: O(1)
List ListNew(void);

// Frees the given list
// Complexity: O(1)
void ListFree(List l);

// Adds a number to the end of the list
// Complexity: O(1)
void ListAppend(List l, int i);

// Prints the list to stdout
// This should only be used for debugging, none of the tasks
// require this function.
void ListShow(List l);

#endif

