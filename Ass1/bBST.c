#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bBST.h"
#include "List.h"
#include <err.h>
#include <sysexits.h>

////////////////////////////////////////////////////////////////////////
/* IMPORTANT:
   Do NOT modify the code above this line.
*/
////////////////////////////////////////////////////////////////////////

// z5413887 COMP2521 Assignment 1

// Auxiliary function prototypes
// Write Auxiliary function prototypes here, and declare them as static

////////////////////////////////////////////////////////////////////////

static Node NodeNew(int key);
static void FreeNodes(Node n);
static Node SearchNodes(Node n, int key);
static Node NodeInsert(Node n, int key);
static Node NodeDelete(Node n, int key);

static Node RotateLeft(Node x);
static Node RotateRight(Node y);
static int GetHeight(Node n);
static int Max(int a, int b);
static int GetBalanceFactor (Node n);
static Node BalanceTree(Node n);

static Node NodeKthSmallest(Node n, int k);
static Node NodeKthLargest(Node n, int k);
static Node NodeLCA(Node n, int lower, int upper);
static void NodesToList(Node n, List l);
static Node NodeCeiling(Node n, int key);
static Node NodeFloor(Node n, int key);
static void NodeSearchBetween(List l, Node n, int lower, int upper);

int count = 0;

////////////////////////////////////////////////////////////////////////

/**
 * Creates a new empty tree.
 * The time complexity of this function must be O(1).
 */
Tree TreeNew(void) {
    Tree new = malloc(sizeof(*new));
	if (new == NULL) {
		err(EX_OSERR, "couldn't allocate BST node");
	}
    new->root = NULL;
    return new;
}

/**
 * Creates a new node with key
 * Time Complexity is O(1)
*/
static Node NodeNew(int key) {
	Node new = malloc(sizeof(*new));
	if (new == NULL) {
		err(EX_OSERR, "couldn't allocate BST node");
	}
	new->key = key;
	new->left = NULL;
	new->right = NULL;
    new->height = 1;
	return new;
}

////////////////////////////////////////////////////////////////////////

/**
 * Frees all memory allocated for the given tree.
 * The time complexity of this function must be O(n).
 */
void TreeFree(Tree t) {
    FreeNodes(t->root);
}

// Frees all nodes within the tree recursively
static void FreeNodes(Node n) {
    if (n == NULL) return;
	FreeNodes(n->left);
	FreeNodes(n->right);
	free(n);
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches the tree for a given key and returns true if the key is in
 * the tree or false otherwise.
 * The time complexity of this function must be O(log n).
 */
bool TreeSearch(Tree t, int key) {
    if (t == NULL) return false;
    if (SearchNodes(t->root, key) != NULL) {
        return true;
    } else {
        return false;
    }
}

// Helper function to implement TreeSearch. Recurse through all tree elements, if key exists return true
static Node SearchNodes(Node n, int key) {
    if (n == NULL) return NULL;
    if (n->key == key) return n;
	
    else if (key < n->key) {
		return SearchNodes(n->left, key);
	} else if (key > n->key) {
		return SearchNodes(n->right, key);
	}
    return NULL;
}

////////////////////////////////////////////////////////////////////////

/**
 * Inserts the given key into the tree.
 * You must use the proper AVL insertion algorithm.
 * Returns true if the key was inserted successfully, or false if the
 * key was already present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeInsert(Tree t, int key) {
    if (t == NULL) {
        return false;
    } else if (key == UNDEFINED) {
        return false;
    } else if (t->root == NULL) { // if tree is empty, create new root
        t->root = NodeNew(key);
        return true;
    } else if (SearchNodes(t->root, key) != NULL) { // check for dupes
        return false;
    }

    Node n = NodeInsert(t->root, key);
    if (n != t->root) { // If tree has been rebalanced, t->root may no longer be the actual root of the BST
        t->root = n;
    }
    return true;
}

/**
 * Inserts a node with key into the BST
 * Follows AVL insertion algorithm, considering LL, LR, RL and RR cases of unbalanced tree
 * Updates heights of all traversed nodes in the process
 * Returns the updated value of the root node
*/
static Node NodeInsert(Node n, int key) {
    if (n == NULL) {
        return NodeNew(key);

    // regular insertion into a BST
    } else if (key < n->key) {
		n->left = NodeInsert(n->left, key);
	} else if (key > n->key) {
		n->right = NodeInsert(n->right, key);
	}

    // update heights of all nodes traversed
    n->height = Max(GetHeight(n->left), GetHeight(n->right)) + 1;
    return BalanceTree(n);
}

static Node BalanceTree(Node n) {
    if (GetBalanceFactor(n) > 1 && GetBalanceFactor(n->left) >= 0) { // Left Left Case
        return RotateRight(n);
    } else if (GetBalanceFactor(n) > 1 && GetBalanceFactor(n->left) <= -1) { // Left Right Case
        n->left = RotateLeft(n->left);
        return RotateRight(n);
    } else if (GetBalanceFactor(n) < -1 && GetBalanceFactor(n->right) <= 0) { // Right Right Case
        return RotateLeft(n);
    } else if (GetBalanceFactor(n) < -1 && GetBalanceFactor(n->right) >= 1) { // Right Left Case
        n->right = RotateRight(n->right);
        return RotateLeft(n);
    }
    return n; // no imbalance
}

// Helper function to return node height
static int GetHeight(Node n) {
    if (n == NULL) return 0;
    return n->height;
}

// Helper function to return max of x and y using ternary operator
static int Max(int x, int y) {
    return (x > y)? x : y;
}

// Helper function to get determine whether tree is unbalanced or not
static int GetBalanceFactor(Node n) {
    if (n == NULL) return 0;
    return GetHeight(n->left) - GetHeight(n->right);
}

// rotate right around Node y, to make node x new root of the subtree
static Node RotateRight(Node y) {
    if (y == NULL) return false;

    Node x = y->left;
    Node transition = x->right;
    x->right = y;
    y->left = transition;

    y->height = Max(GetHeight(y->left), GetHeight(y->right)) + 1;
    x->height = Max(GetHeight(x->left), GetHeight(x->right)) + 1;
    return x;
}

// rotate left around Node x, to make node y new root of the subtree
static Node RotateLeft(Node x) {
    if (x == NULL) return false;

    Node y = x->right;
    Node transition = y->left;
    y->left = x;
    x->right = transition;

    x->height = Max(GetHeight(x->left), GetHeight(x->right)) + 1;
    y->height = Max(GetHeight(y->left), GetHeight(y->right)) + 1;
    return y;
}

////////////////////////////////////////////////////////////////////////

/**
 * Deletes the given key from the tree if it is present.
 * You must use the proper AVL deletion algorithm.
 * Returns true if the key was deleted successfully, or false if the key
 * was not present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeDelete(Tree t, int key) {
    if (t == NULL || t->root == NULL) return false;
    if (!(TreeSearch(t, key))) {
        return false;
    }

    Node n = NodeDelete(t->root, key);
    if (n != t->root) { // If tree has been rebalanced, t->root may no longer be the actual root of the BST
        t->root = n;
    }
    return true;
}

static Node NodeDelete(Node n, int key) {
    if (n == NULL) return NULL; // base case

    else if (key < n->key) { // finding correct node to delete, search left subtree
		n->left = NodeDelete(n->left, key);
	} else if (key > n->key) { // finding correct node to delete, search right subtree
		n->right = NodeDelete(n->right, key);

	} else { // found correct node
        if (n->left == NULL || n->right == NULL) {
            Node swap = n->left ? n->left : n->right;

            if (swap == NULL) { // Swap node is a leaf
                swap = n;
                n = NULL;
            }

            else { // Swap node has one child, just swap contents
            *n = *swap;
            free(swap);
            }
        }

        else { // Node with 2 children

            Node Successor = NodeKthSmallest(n->right, 1);
            count = 0;
            n->key = Successor->key;
            n->right = NodeDelete(n->right, Successor->key);
        }
    }

    if (n == NULL) {
        return n;
    }

    n->height = Max(GetHeight(n->left), GetHeight(n->right)) + 1;
    return BalanceTree(n);
}



////////////////////////////////////////////////////////////////////////

/**
 * Creates a list containing all the keys in the given tree in ascending
 * order.
 * The time complexity of this function must be O(n).
 */
List TreeToList(Tree t) {
    if (t == NULL || t->root == NULL) return NULL;
    List l = ListNew();
    NodesToList(t->root, l);
    return l;
}

/**
 * Completes an inorder traversal of the BST, appending each element to the List
*/
static void NodesToList(Node n, List l) {
    if (n == NULL) return;
    NodesToList(n->left, l);
	ListAppend(l, n->key);
	NodesToList(n->right, l);
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th smallest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the smallest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */
int TreeKthSmallest(Tree t, int k) {
    if (t == NULL) return 0;
    if (k == UNDEFINED) return 0;
    if (k < 1) return 0;

    Node n = NodeKthSmallest(t->root, k);
    count = 0;
    if (n == NULL) return 0; 
    return n->key;
}

/**
 * Performs an inorder traversal of the tree, pushing all elements off the stack. 
 * Count is iterated each time an element is pushed, until the kth element is found. 
*/
static Node NodeKthSmallest(Node n, int k) {
    if (n == NULL) return NULL;

    Node kMinusNsmallest = NodeKthSmallest(n->left, k);
    if (kMinusNsmallest != NULL) {
        return kMinusNsmallest;
    }

    count+=1;
	if (count == k) {
        return n;
    }
	return NodeKthSmallest(n->right, k);
    // if the whole tree finishes recursing, there are not enough elements in Tree t
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th largest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the largest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */
int TreeKthLargest(Tree t, int k) {
    if (t == NULL) return 0;
    if (k == UNDEFINED) return 0;
    if (k < 1) return 0;

    Node n = NodeKthLargest(t->root, k);
    count = 0;
    if (n == NULL) return 0; 
    return n->key;
}

/**
 * Performs an backwards inorder traversal of the tree, pushing all elements off the stack. 
 * Count is iterated each time an element is pushed, until the kth element is found. 
*/
static Node NodeKthLargest(Node n, int k) {
    if (n == NULL) return NULL;

    Node kMinusNlargest = NodeKthLargest(n->right, k);
    if (kMinusNlargest != NULL) {
        return kMinusNlargest;
    }

    count+=1;
	if (count == k) {
        return n;
    }
	return NodeKthLargest(n->left, k);
    // if the whole tree finishes recursing, there are not enough elements in Tree t
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the least common ancestor of two keys, a and b.
 * Returns UNDEFINED if either a or b are not present in the tree.
 * a may be larger than b, or equal to each other.
 * Either a or b may be the LCA in some cases.
 * The time complexity of this function must be O(log n).
 */
int TreeLCA(Tree t, int a, int b) {
    if (t == NULL) return UNDEFINED;
    if (!(TreeSearch(t, a)) || !(TreeSearch(t, b))) return UNDEFINED;

    Node n = NodeLCA(t->root, a, b);
    if (n == NULL) return UNDEFINED;
    return n->key;
}

/**
 * Searches the tree for the LCA of a given node. If a and b are within different subtrees
 * then the current node is the LCA of the node.
 * Recursively searches through it finds the last node that satisfies that condition.
*/
static Node NodeLCA(Node n, int a, int b) {
    if (n == NULL) {
        return NULL; // Base case: if n is NULL, return NULL.
    }

    if (n->key == a || n->key == b) {
        return n;
    }
    
    Node left = NodeLCA(n->left, a, b); // Recurse on the left subtree.
    Node right = NodeLCA(n->right, a, b); // Recurse on the right subtree.
    
    if (left != NULL && right != NULL) {
        return n; // If a and b are in different subtrees, then the current node is the LCA.
    } else if (left != NULL) {
        return NodeLCA(left, a, b); // If both a and b are in the left subtree, then the LCA is in the left subtree.
    } else {
        return NodeLCA(right, a, b); // If both a and b are in the right subtree, then the LCA is in the right subtree.
    }
}
////////////////////////////////////////////////////////////////////////

/**
 * Returns the largest key less than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeFloor(Tree t, int key) {
    if (t == NULL) {
        return UNDEFINED;
    }
    Node FloorNode = NodeFloor(t->root, key);
    if (FloorNode == NULL) {
        return UNDEFINED;
    } else {
        return FloorNode->key;
    }
}

/**
 * Traverses tree downwards, searching left subtree if there are no values less than
 * given key in right subtree, otherwise searching right subtree.
 * Returns greatest key less than or equal to given key
*/
static Node NodeFloor(Node n, int key) {
    if (n == NULL) {
        return NULL;
    }
    if (n->key == key) { // Exact key found
        return n;
    } else if (n->key > key) { // Look for floor value in left subtree
        return NodeFloor(n->left, key);
    } else { // Look for floor value in right subtree, if doens't exist return n as floor value
        Node floor = NodeFloor(n->right, key);
        if (floor == NULL) {
            return n;
        } else {
            return floor;
        }
    }
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the smallest key greater than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeCeiling(Tree t, int key) {
    if (t == NULL) {
        return UNDEFINED;
    }
    Node CeilingNode = NodeCeiling(t->root, key);
    if (CeilingNode == NULL) {
        return UNDEFINED;
    } else {
        return CeilingNode->key;
    }
}

/**
 * Traverses tree downwards, searching right subtree if there are no values less than
 * given key in left subtree, otherwise searching left subtree.
 * Returns smallest key less than or equal to given key
*/
static Node NodeCeiling(Node n, int key) {
    if (n == NULL) {
        return NULL;
    }
    if (n->key == key) { // Exact key found
        return n;
    } else if (n->key < key) { // Look for ceiling value in right subtree
        return NodeCeiling(n->right, key);
    } else { // Look for ceiling value in left subtree, if doens't exist return n as ceiling value
        Node ceiling = NodeCeiling(n->left, key);
        if (ceiling == NULL) {
            return n;
        } else {
            return ceiling;
        }
    }
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches for all keys between the two given keys (inclusive) and
 * returns the keys in a list in ascending order.
 * Assumes that lower is less than or equal to upper.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 */
List TreeSearchBetween(Tree t, int lower, int upper) {
    List l = ListNew();
    if (t == NULL || lower > upper) {
        return l;
    }
    NodeSearchBetween(l, t->root, lower, upper);
    return l;
}

/**
 * Adjusts the search range of all nodes within the tree to find the desired
 * Search between. Once found, peforms an inorder traversal of all nodes
 * within the search range, and appends to List l. Returns list l 
*/
static void NodeSearchBetween(List l, Node n, int lower, int upper) {
    if (n == NULL) return;

    // if current key is less than range, search right subtree
    if (n->key < lower) {
        NodeSearchBetween(l, n->right, lower, upper);
        return;
    }

    // if current key is greater than range, search right subtree
    if (n->key > upper) {
        NodeSearchBetween(l, n->left, lower, upper);
        return;
    }

    // if within the range, perform inorder traversal and append to list
    if (n->key >= lower && n->key <= upper) {
        NodeSearchBetween(l, n->left, lower, upper);
        ListAppend(l, n->key);
        NodeSearchBetween(l, n->right, lower, upper);
    }

}

////////////////////////////////////////////////////////////////////////
