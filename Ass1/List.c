// Implementation of the List ADT

// !!! DO NOT MODIFY THIS FILE !!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"

typedef struct listnode {
    int value;
    struct listnode *next;
} *ListNode;

struct list {
    ListNode head;
    ListNode tail;
};

////////////////////////////////////////////////////////////////////////

static void listNodeFree(ListNode n);
static ListNode listNodeNew(int i);

// Creates a new empty list
List ListNew(void) {
    List l = malloc(sizeof (*l));
    if (l == NULL) {
        fprintf(stderr, "Insufficient memory!\n");
        exit(EXIT_FAILURE);
    }

    l->head = l->tail = NULL;
    return l;
}

// Frees the given list
void ListFree(List l) {
    listNodeFree(l->head);
    free(l);
}

static void listNodeFree(ListNode n) {
    if (!n)
        return;

    listNodeFree(n->next);
    free(n);
}

// Adds a number to the end of the list
void ListAppend(List l, int i) {
    ListNode newNode = listNodeNew(i);
    if (!l->head)
        l->head = l->tail = newNode;
    else
        l->tail = l->tail->next = newNode;
}

static ListNode listNodeNew(int i) {
    ListNode n = malloc(sizeof (*n));
    if (n == NULL) {
        fprintf(stderr, "Insufficient memory!\n");
        exit(EXIT_FAILURE);
    }

    n->value = i;
    n->next = NULL;
    return n;
}

// Prints the list to stdout
// This should only be used for debugging, none of the tasks
// require this function.
void ListShow(List l) {
    printf("[");
    for (ListNode curr = l->head; curr != NULL; curr = curr->next) {
        printf("%d", curr->value);
        if (curr->next != NULL) {
            printf(", ");
        }
    }
    printf("]");
}

