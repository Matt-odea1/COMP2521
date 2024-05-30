// Operations on Balanced Binary Search Trees (BBST).
// You MUST NOT modify this file. You do not submit it, and
// we will compile your program with an unedited file.

#ifndef TREE_H
#define TREE_H

#include <limits.h>
#include <stdbool.h>

#include "List.h"

#define UNDEFINED INT_MIN

typedef struct tree *Tree;
typedef struct node *Node;

// These definitions are here so they cannot be modified
// We will compile with the original bBST.h file for
// testing. It is not good practice to include internals
// of an ADT in a header like this.
struct node {
    int key;
    Node left;
    Node right;
    int height;
};

struct tree {
    Node root;
};

////////////////////////////////////////////////////////////////////////
// All complexities below are in terms of n, the number of nodes in the
// tree, unless otherwise specified.

////////////////////////////////////////////////////////////////////////

/**
 * Creates a new empty tree.
 * The time complexity of this function must be O(1).
 */
Tree TreeNew(void);

/**
 * Frees all memory allocated for the given tree.
 * The time complexity of this function must be O(n).
 */
void TreeFree(Tree t);

/**
 * Searches the tree for a given key and returns true if the key is in
 * the tree or false otherwise.
 * The time complexity of this function must be O(log n).
 */
bool TreeSearch(Tree t, int key);

/**
 * Inserts the given key into the tree.
 * You must use the proper AVL insertion algorithm.
 * Returns true if the key was inserted successfully, or false if the
 * key was already present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeInsert(Tree t, int key);

/**
 * Deletes the given key from the tree if it is present.
 * You must use the proper AVL deletion algorithm.
 * Returns true if the key was deleted successfully, or false if the key
 * was not present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeDelete(Tree t, int key);

/**
 * Creates a list containing all the keys in the given tree in ascending
 * order.
 * The time complexity of this function must be O(n).
 */
List TreeToList(Tree t);

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th smallest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the smallest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */
int TreeKthSmallest(Tree t, int k);

/**
 * Returns the k-th largest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the largest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */
int TreeKthLargest(Tree t, int k);

/**
 * Returns the least common ancestor of two keys, a and b.
 * Returns UNDEFINED if either a or b are not present in the tree.
 * a may be larger than b, or equal to each other.
 * Either a or b may be the LCA in some cases.
 * The time complexity of this function must be O(log n).
 */
int TreeLCA(Tree t, int a, int b);

/**
 * Returns the largest key less than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeFloor(Tree t, int key);

/**
 * Returns the smallest key greater than or equal to the given value.
 * Return UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeCeiling(Tree t, int key);

/**
 * Searches for all keys between the two given keys (inclusive) and
 * returns the keys in order in a list.
 * Assumes that lower is less than or equal to upper.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 */
List TreeSearchBetween(Tree t, int lower, int upper);

#endif

