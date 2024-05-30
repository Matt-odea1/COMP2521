#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bBST.h"

static void printTree(Tree t);

// COMP2521 23T1 Assignment 1
// Test file to be added.
// For now, a simple main function that exits.

// Once again, don't do this - you should not access
// the internals of the ADT in your program
// This is bad practice.
typedef struct listnode {
    int value;
    struct listnode *next;
} *ListNode;
struct list {
    ListNode head;
    ListNode tail;
};

static void doPrintBSTree(Node t, FILE *out);

// Prints a BSTree to the given file.
// Assumes that the file is open for writing.
void BSTreePrint(Node t, FILE *out) {
	doPrintBSTree(t, out);
}

// Prints a BSTree to stdout.
void BSTreeShow(Node t) {
	BSTreePrint(t, stdout);
}

// Prints the value in the given node
static void showBSTreeNode(Node t) {
	if (t == NULL) return;
	printf("%d ", t->key);
}

// Prints the in-order traversal of the given BSTree
void BSTreeInOrder(Node t) {
	if (t == NULL) return;

	BSTreeInOrder(t->left);
	showBSTreeNode(t);
	BSTreeInOrder(t->right);
}

// Prints the pre-order traversal of the given BSTree
void BSTreePreOrder(Node t) {
	if (t == NULL) return;

	showBSTreeNode(t);
	BSTreePreOrder(t->left);
	BSTreePreOrder(t->right);
}

// Prints the post-order traversal of the given BSTree
void BSTreePostOrder(Node t) {
	if (t == NULL) return;

	BSTreePostOrder(t->left);
	BSTreePostOrder(t->right);
	showBSTreeNode(t);
}


////////////////////////////////////////////////////////////////////////
// ASCII tree printer
// Courtesy: ponnada
// Via: http://www.openasthra.com/c-tidbits/printing-binary-trees-in-ascii/

// data structures
typedef struct asciinode_struct asciinode;
struct asciinode_struct {
	asciinode *left, *right;
	// length of the edge from this node to its children
	int edge_length;
	int height;
	int lablen;
	// -1=I am left, 0=I am root, 1=I am right
	int parent_dir;
	// max supported unit32 in dec, 10 digits max
	char label[11];
};

// functions
static void print_level(asciinode *node, int x, int level, FILE *out);
static void compute_edge_lengths(asciinode *node);
static void compute_lprofile(asciinode *node, int x, int y);
static void compute_rprofile(asciinode *node, int x, int y);
static asciinode *build_ascii_tree(Node t);
static void free_ascii_tree(asciinode *node);

#define MAX_HEIGHT 1000
static int lprofile[MAX_HEIGHT];
static int rprofile[MAX_HEIGHT];
#define INFINITY (1 << 20)

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

static int gap = 3; // gap between left and right nodes

// used for printing next node in the same level,
// this is the x coordinate of the next char printed
static int print_next;

// prints ascii tree for given Tree structure
static void doPrintBSTree(Node t, FILE *out)
{
	asciinode *proot;
	int xmin, i;
	if (t == NULL)
		return;
	proot = build_ascii_tree(t);
	compute_edge_lengths(proot);
	for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
		lprofile[i] = INFINITY;
	compute_lprofile(proot, 0, 0);
	xmin = 0;
	for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
		xmin = MIN(xmin, lprofile[i]);
	for (i = 0; i < proot->height; i++) {
		print_next = 0;
		print_level(proot, -xmin, i, out);
		fprintf(out, "\n");
	}
	if (proot->height >= MAX_HEIGHT) {
		fprintf(out,
			"(Tree is taller than %d; may be drawn incorrectly.)\n",
			MAX_HEIGHT);
	}
	free_ascii_tree(proot);
}

// This function prints the given level of the given tree, assuming
// that the node has the given x cordinate.
static void print_level(asciinode *node, int x, int level, FILE *out)
{
	int i, isleft;
	if (node == NULL)
		return;
	isleft = (node->parent_dir == -1);
	if (level == 0) {
		for (i = 0;
			 i < (x - print_next - ((node->lablen - isleft) / 2));
			 i++)
			fprintf(out, " ");
		print_next += i;
		fprintf(out, "%s", node->label);
		print_next += node->lablen;
	} else if (node->edge_length >= level) {
		if (node->left != NULL) {
			for (i = 0; i < (x - print_next - (level)); i++) {
				fprintf(out, " ");
			}
			print_next += i;
			fprintf(out, "/");
			print_next++;
		}
		if (node->right != NULL) {
			for (i = 0; i < (x - print_next + (level)); i++) {
				fprintf(out, " ");
			}
			print_next += i;
			fprintf(out, "\\");
			print_next++;
		}
	} else {
		print_level(
			node->left,
			x - node->edge_length - 1,
			level - node->edge_length - 1,
			out);
		print_level(
			node->right,
			x + node->edge_length + 1,
			level - node->edge_length - 1,
			out);
	}
}

// This function fills in the edge_length and
// height fields of the specified tree
static void compute_edge_lengths(asciinode *node)
{
	int h, hmin, i, delta;
	if (node == NULL)
		return;
	compute_edge_lengths(node->left);
	compute_edge_lengths(node->right);

	/* first fill in the edge_length of node */
	if (node->right == NULL && node->left == NULL)
		node->edge_length = 0;
	else {
		if (node->left == NULL)
			hmin = 0;
		else {
			for (i = 0; i < node->left->height && i < MAX_HEIGHT; i++)
				rprofile[i] = -INFINITY;
			compute_rprofile(node->left, 0, 0);
			hmin = node->left->height;
		}
		if (node->right == NULL)
			hmin = 0;
		else {
			for (i = 0; i < node->right->height && i < MAX_HEIGHT; i++)
				lprofile[i] = INFINITY;
			compute_lprofile(node->right, 0, 0);
			hmin = MIN(node->right->height, hmin);
		}
		delta = 4;
		for (i = 0; i < hmin; i++) {
			int w = gap + 1 + rprofile[i] - lprofile[i];
			delta = (delta > w) ? delta : w;
		}

		// If the node has two children of height 1, then we allow the
		// two leaves to be within 1, instead of 2
		if (((node->left != NULL && node->left->height == 1) ||
			(node->right != NULL && node->right->height == 1)) &&
			delta > 4)
			delta--;
		node->edge_length = ((delta + 1) / 2) - 1;
	}

	// now fill in the height of node
	h = 1;
	if (node->left != NULL)
		h = MAX(node->left->height + node->edge_length + 1, h);
	if (node->right != NULL)
		h = MAX(node->right->height + node->edge_length + 1, h);
	node->height = h;
}

static asciinode *build_ascii_tree_recursive(Node t)
{
	asciinode *node;

	if (t == NULL)
		return NULL;
	node = malloc(sizeof(asciinode));
	node->left = build_ascii_tree_recursive(t->left);
	node->right = build_ascii_tree_recursive(t->right);
	if (node->left != NULL)
		node->left->parent_dir = -1;
	if (node->right != NULL)
		node->right->parent_dir = 1;
	snprintf(node->label, sizeof(node->label), "%d", t->key);
	node->lablen = (int) strlen(node->label);

	return node;
}

// Copy the tree into the ascii node structre
static asciinode *build_ascii_tree(Node t)
{
	asciinode *node;
	if (t == NULL)
		return NULL;
	node = build_ascii_tree_recursive(t);
	node->parent_dir = 0;
	return node;
}

// Free all the nodes of the given tree
static void free_ascii_tree(asciinode *node)
{
	if (node == NULL)
		return;
	free_ascii_tree(node->left);
	free_ascii_tree(node->right);
	free(node);
}

// The following function fills in the lprofile array for the given
// tree. It assumes that the center of the label of the root of this tree
// is located at a position(x,y).  It assumes that the edge_length
// fields have been computed for this tree.
static void compute_lprofile(asciinode *node, int x, int y)
{
	int i, isleft;
	if (node == NULL)
		return;
	isleft = (node->parent_dir == -1);
	lprofile[y] = MIN(lprofile[y], x - ((node->lablen - isleft) / 2));
	if (node->left != NULL) {
		for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++)
			lprofile[y + i] = MIN(lprofile[y + i], x - i);
	}
	compute_lprofile(
		node->left,
		x - node->edge_length - 1,
		y + node->edge_length + 1);
	compute_lprofile(
		node->right,
		x + node->edge_length + 1,
		y + node->edge_length + 1);
}

static void compute_rprofile(asciinode *node, int x, int y)
{
	int i, notleft;
	if (node == NULL)
		return;
	notleft = (node->parent_dir != -1);
	rprofile[y] = MAX(rprofile[y], x + ((node->lablen - notleft) / 2));
	if (node->right != NULL) {
		for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++)
			rprofile[y + i] = MAX(rprofile[y + i], x + i);
	}
	compute_rprofile(
		node->left,
		x - node->edge_length - 1,
		y + node->edge_length + 1);
	compute_rprofile(
		node->right,
		x + node->edge_length + 1,
		y + node->edge_length + 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW ""
#define BLU "\033[0;34m"
#define CLR "\033[0m"
// Don't use macros in your assignment, especially no variadic ones.
#define PERR(msg, ...) {fprintf(stderr, RED "-- " msg CLR, ## __VA_ARGS__); TreeFree(t); return false;}
#define PLERR(msg, ...) {fprintf(stderr, RED "-- " msg CLR, ## __VA_ARGS__); ListFree(l); TreeFree(t); return false;}

#define SIZE 11

static Tree makeTree() {
    Tree t = TreeNew();
    int vals[SIZE] = {10, 15, 25, 20, 23, 5, 1, 3, 22, 21, 17};
    for (int i = 0; i < SIZE; i++)
        TreeInsert(t, vals[i]);
    return t;
}

static bool testTreeInsertSearch(void) {
    printf(YLW "%s...\n" CLR, __func__);
    int vals[SIZE] = {10, 15, 25, 20, 23, 5, 1, 3, 22, 21, 17};
    int absent[3] = {2, 30, -2};
    Tree t = TreeNew();

    if (TreeSearch(t, vals[0]))
        PERR("Found value in empty tree.\n");
    for (int i = 0; i < SIZE; i++) {
        if (!TreeInsert(t, vals[i]))
            PERR("Failed to insert %d.\n", vals[i]);
        if (!TreeSearch(t, vals[i]))
            PERR("Failed to find %d after insertion.\n", vals[i]);
    }
    for (int i = 0; i < SIZE; i++)
        if (TreeInsert(t, vals[i]))
            PERR("Insert of %d succeeded twice.\n", vals[i]);
    for (int i = 0; i < SIZE; i++)
        if (!TreeSearch(t, vals[i]))
            PERR("Failed to find %d in final tree.\n", vals[i]);
    for (int i = 0; i < 3; i++)
        if (TreeSearch(t, absent[i]))
            PERR("Found %d in final tree.\n", absent[i]);
    
    TreeFree(t);
    return true;
}

static bool testTreeDelete(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int vals[SIZE] = {15, 25, 22, 5, 23, 1, 17, 10, 20, 21, 3};

    for (int i = 0; i < SIZE; i++) {
        if (!TreeDelete(t, vals[i]))
            PERR("Failed to delete %d.\n", vals[i]);
        if (TreeSearch(t, vals[i]))
            PERR("Found %d after deletion.\n", vals[i]);
        for (int j = i + 1; j < SIZE; j++)
            if (!TreeSearch(t, vals[j]))
                PERR("Failed to find %d but not deleted (after deleting %d).\n", vals[j], vals[i]);
    }

    TreeFree(t);
    return true;
}

static bool testTreeToList(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int exp[11] = {1, 3, 5, 10, 15, 17, 20, 21, 22, 23, 25};

    List l = TreeToList(t);
    printf(BLU "-- TreeList returned: [");
    ListNode curr = l->head;

    int index = -1;
    int indexVal = -1;
    int i = 0;
    while (curr && i < 11) {
        printf("%d", curr->value);
        if (index == -1 && curr->value != exp[i]) {
            index = i;
            indexVal = curr->value;
        }
        if (curr->next)
            printf(", ");
        curr = curr->next;
        i++;
    }

    if (curr != NULL)
        printf("...");
    printf("]\n" CLR);
    if (i != 11 || curr != NULL)
        PLERR("List sizes mismatched!\n");
    if (index != -1)
        PLERR("Expected %d not %d at index %d.\n", exp[index], indexVal, index);

    ListFree(l);
    TreeFree(t);
    return true;
}

static bool testTreeKthSmallest(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int kVals[4] = {1, 11, 2, 5};
    int results[4] = {1, 25, 3, 15};

    for (int i = 0; i < 4; i++) {
        int val = TreeKthSmallest(t, kVals[i]);
        if (val != results[i])
            PERR("Expected %d but got %d for %d-th smallest.\n", results[i], val, kVals[i]);
    }
    
    TreeFree(t);
    return true;
}

static bool testTreeKthLargest(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int kVals[4] = {1, 11, 2, 5};
    int results[4] = {25, 1, 23, 20};

    for (int i = 0; i < 4; i++) {
        int val = TreeKthLargest(t, kVals[i]);
        if (val != results[i])
            PERR("Expected %d but got %d for %d-th largest.\n", results[i], val, kVals[i]);
    }

    TreeFree(t);
    return true;
}

static bool testTreeLCA(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int aVals[6] = {10, 23, 20, 15, 3, -2};
    int bVals[6] = {20, 20, 7, 22, 5, 30};
    int results[6] = {15, 21, UNDEFINED, 15, 5, UNDEFINED};
    
    for (int i = 0; i < 6; i++) {
        int result = TreeLCA(t, aVals[i], bVals[i]);
        if (result != results[i])
            PERR("Expected %d but got %d for LCA(%d, %d). Failed on case: %d\n", results[i], result, aVals[i], bVals[i], i+1);
    }

    TreeFree(t);
    return true;
}

static bool testTreeFloor(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int vals[7] = {10, 2, 15, 24, -2, 16, 30};
    int results[7] = {10, 1, 15, 23, UNDEFINED, 15, 25};

    for (int i = 0; i < 7; i++) {
        int result = TreeFloor(t, vals[i]);
        if (result != results[i])
            PERR("Expected %d but got %d as floor of %d.\n", results[i], result, vals[i]);
    }

    TreeFree(t);
    return true;
}

static bool testTreeCeiling(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int vals[7] = {10, 2, 15, 18, -2, 16, 30};
    int results[7] = {10, 3, 15, 20, 1, 17, UNDEFINED};

    for (int i = 0; i < 7; i++) {
        int result = TreeCeiling(t, vals[i]);
        if (result != results[i])
            PERR("Expected %d but got %d as ceiling of %d.\n", results[i], result, vals[i]);
    }

    TreeFree(t);
    return true;
}

static bool runTreeSearchBetween(Tree t, int lower, int upper, int *exp, int len) {
    List l = TreeSearchBetween(t, lower, upper);
    printf(BLU "-- TreeSearchBetween(%d, %d) returned: [", lower, upper);
    ListNode curr = l->head;

    int index = -1;
    int indexVal = -1;
    int i = 0;
    while (curr && i < len) {
        printf("%d", curr->value);
        if (index == -1 && curr->value != exp[i]) {
            index = i;
            indexVal = curr->value;
        }
        if (curr->next)
            printf(", ");
        curr = curr->next;
        i++;
    }

    if (curr != NULL)
        printf("...");
    printf("]\n" CLR);
    if (i != len || curr != NULL)
        PLERR("List sizes mismatched!\n");
    if (index != -1)
        PLERR("Expected %d not %d at index %d.\n", exp[index], indexVal, index);

    ListFree(l);
    return true;
}

static bool testTreeSearchBetween(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int lowers[5] = {1, 17, 10, -2, 11};
    int uppers[5] = {25, 17, 30, 16, 14};
    int exp[5][11] = {
        {1, 3, 5, 10, 15, 17, 20, 21, 22, 23, 25},
        {17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {10, 15, 17, 20, 21, 22, 23, 25, 0, 0, 0},
        {1, 3, 5, 10, 15, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    int lens[5] = {11, 1, 8, 5, 0};

    for (int i = 0; i < 5; i++)
        if (!runTreeSearchBetween(t, lowers[i], uppers[i], exp[i], lens[i]))
            return false;

    TreeFree(t);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printTree(Tree tree) {
    Node t = tree->root;
    printf("Tree:\n");
	BSTreeShow(t);
	printf("\n");
	printf("In-order:    "); BSTreeInOrder(t);    printf("\n\n");
}

// TEST CASE 1
static bool TestInsertSearch() {
    Tree t = TreeNew();
    srand(time(0));
    printf("\nORDER OF INSERTION: ");
    for (int i = 1; i <= 7; i++) {  
        int num = rand() % 100 + 1; // use rand() function to get the random number
        TreeInsert(t, num);
        printf("%d ", num); 
    }

    // standard key insert
    TreeInsert(t, 105);
    //printTree(t);
    bool result = TreeSearch(t, 105);
    TreeFree(t);
    return result;
}

// TEST CASE 2
static bool TestInsertBadSearch() {
    Tree t = TreeNew();
    TreeInsert(t, 1);
    bool result = TreeSearch(t, 5);
    TreeFree(t);
    return result;
}

// TEST CASE 3
static bool TestTreeDeleteLeaf() {
    Tree t = TreeNew();
    TreeInsert(t, 50);
    srand(time(0));
    for (int i = 1; i <= 13; i++) {
        int num = rand() % 100 + 1; // use rand() function to get the random number
        TreeInsert(t, num);
    }
    // standard key insert
    TreeDelete(t, 50);
    bool result = TreeSearch(t, 5);
    TreeFree(t);
    return result;
}

// TEST CASE 4
static bool TestGoodLCA() {
    Tree t = TreeNew();
    TreeInsert(t, 50);
    TreeInsert(t, 25);
    TreeInsert(t, 75);
    TreeInsert(t, 100);
    TreeInsert(t, 60);
    TreeInsert(t, 35);
    TreeInsert(t, 15);
    TreeInsert(t, 5);
    printTree(t);
    List l = TreeSearchBetween(t, 15, 60);
    ListShow(l);

    if (TreeLCA(t, 5, 60) == 50) {
        TreeFree(t);
        return true;
    }
    TreeFree(t);
    return false;
}

int main(int argc, char **argv) {
    bool (*tests[])(void) = {
        testTreeInsertSearch,
        testTreeDelete,
        testTreeToList,
        testTreeKthSmallest,
        testTreeKthLargest,
        testTreeLCA,
        testTreeFloor,
        testTreeCeiling,
        testTreeSearchBetween,
        NULL
    };
    int numTests = 9;
    int success = 0;
    int total = 0;

    if (argc == 1) {
        for (int i = 0; tests[i]; i++) {
            bool result = (tests[i])();
            if (result)
                printf(GRN "Test passed.\n\n" CLR);
            else
                printf(RED "Test failed.\n\n" CLR);
            success += result;
            total++;
        }
    } else {
        for (int i = 1; i < argc; i++) {
            int testNo = atoi(argv[i]) - 1;
            if (testNo < 0 || testNo >= numTests)
                continue;

            bool result = (tests[testNo])();
            if (result)
                printf(GRN "Test passed.\n\n" CLR);
            else
                printf(RED "Test failed.\n\n" CLR);
            success += result;
            total++;
        }
    }

    if (total == success)
        printf(GRN "All %d tests passed!\n" CLR, total);
    else
        printf(RED "%d of %d tests passed.\n" CLR, success, total);

    // TEST 1!
    if (TestInsertSearch()) printf(GRN "Test 1 passed.\n" CLR);
    if (!TestInsertBadSearch()) printf(GRN "Test 2 passed.\n" CLR);
    if (!TestTreeDeleteLeaf()) printf(GRN "Test 3 passed.\n" CLR);;
    if (TestGoodLCA()) printf(GRN "Test 4 passed.\n" CLR);;

    return 0;
}


