// Implementation of the Wall ADT

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sysexits.h>

#include "Wall.h"

typedef struct rock Rock;

struct wall {
    int height;
    int width;
    int numRocks;
    Rock** rocks;
};

static bool CalcInRange(int p1col, int p1row, int p2col, int p2row, double radius);
static int CompareRocks(const void *ptr1, const void *ptr2);

/**
 * Creates a new blank wall with the given dimensions
 */
Wall WallNew(int height, int width) {
    Wall w = malloc(sizeof(*w));
	if (w == NULL) {
		fprintf(stderr, "couldn't allocate Wall\n");
		exit(EXIT_FAILURE);
	}

	w->rocks = malloc(width * sizeof(Rock));
    for (int i = 0; i <= width; i++) {
        w->rocks[i] = malloc(height * sizeof(Rock));
    }

	if (w->rocks == NULL) {
		fprintf(stderr, "couldn't allocate Wall\n");
		exit(EXIT_FAILURE);
	}

	w->width = width;
	w->height = height;
    w->numRocks = 0;
	return w;
}

/**
 * Frees all memory allocated to the wall 
 */
void WallFree(Wall w) {
    for (int i = 0; i < w->width; i++) {
        free(w->rocks[i]);
    }
    free(w->rocks);
    free(w);
}

/**
 * Returns the height of the wall
 */
int WallHeight(Wall w) {
    return w->height;
}

/**
 * Returns the width of the wall
 */
int WallWidth(Wall w) {
    return w->width;
}

/**
 * Adds a rock to the wall
 * If there is already a rock at the given coordinates, replaces it
 */
void WallAddRock(Wall w, Rock rock) {
    for (int i = 0; i < w->width; i++) {
        for (int j = 0; j < w->height; j++) {
            if (w->rocks[i][j].col == rock.col && w->rocks[i][j].row == rock.row) {
                w->rocks[i][j].colour = rock.colour;
                return;
            }
            else if (rock.col == i && rock.row == j) {
                w->rocks[i][j] = rock;
            }
        }
    }

    w->numRocks += 1;
    return;
}

/**
 * Returns the number of rocks on the wall
 */
int WallNumRocks(Wall w) {
    return w->numRocks;
}

/**
 * Stores all rocks on the wall in the given `rocks` array and returns
 * the number of rocks stored. Assumes that the array is at least as
 * large as the number of rocks on the wall.
 */
int WallGetAllRocks(Wall w, Rock rocks[]) {
    int count = 0;
    for (int i = 0; i < w->width; i++) {
        for (int j = 0; j < w->height; j++) {
            if (w->rocks[i][j].col > -1 && w->rocks[i][j].row > -1) {
                rocks[count] = w->rocks[i][j];
                count++;
            }
        }
    }
    return w->numRocks;
}

/**
 * Stores all rocks that are within a distance of `dist` from the given
 * coordinates in the given `rocks` array and returns the number of rocks
 * stored. Assumes that the array is at least as large as the number of
 * rocks on the wall.
 */
int WallGetRocksInRange(Wall w, int row, int col, int dist, Rock rocks[]) {
    int numInRange = 0;
    for (int i = 0; i < w->width; i++) {
        for (int j = 0; j < w->height; j++) {
            if (w->rocks[i][j].col > -1 && w->rocks[i][j].row > -1) {
                if (CalcInRange(i, j, col, row, dist)) {
                    rocks[numInRange] = w->rocks[i][j];
                    numInRange+=1;
                }
            }
        }
    }
    return numInRange;
}

static bool CalcInRange(int p1col, int p1row, int p2col, int p2row, double radius) {
    double distance = sqrt((p1col - p2col) * (p1col - p2col) + (p1row - p2row) * (p1row - p2row));
    if (distance > radius) {
        return false;
    }
    return true;
}

/**
 * Stores all rocks with the colour `colour` that are within a distance
 * of `dist` from the given coordinates in the given `rocks` array and
 * returns the number of rocks stored. Assumes that the array is at
 * least as large as the number of rocks on the wall.
 */
int WallGetColouredRocksInRange(Wall w, int row, int col, int dist, Colour colour, Rock rocks[]) {
    int numInRange = 0;
    for (int i = 0; i < w->width; i++) {
        for (int j = 0; j < w->height; j++) {
            if (CalcInRange(i, j, col, row, dist) && w->rocks[i][j].colour == colour) {
                rocks[numInRange] = w->rocks[i][j];
                numInRange++;
            }
        }
    }
    return numInRange;
}

////////////////////////////////////////////////////////////////////////

/**
 * Prints the wall out in a nice format
 * NOTE: DO NOT MODIFY THIS FUNCTION! This function will work once
 *       WallGetAllRocks and all the functions above it work.
 */
void WallPrint(Wall w) {
    int height = WallHeight(w);
    int width = WallWidth(w);
    int numRocks = WallNumRocks(w);
    struct rock *rocks = malloc(numRocks * sizeof(struct rock));
    WallGetAllRocks(w, rocks);
    qsort(rocks, numRocks, sizeof(struct rock), CompareRocks);

    int i = 0;
    for (int y = height; y >= 0; y--) {
        for (int x = 0; x <= width; x++) {
            if ((y == 0 || y == height) && (x == 0 || x % 5 == 0)) {
                printf("+ ");
            } else if ((x == 0 || x == width) && (y == 0 || y % 5 == 0)) {
                printf("+ ");
            } else if (y == 0 || y == height) {
                printf("- ");
            } else if (x == 0 || x == width) {
                printf("| ");
            } else if (i < numRocks && y == rocks[i].row && x == rocks[i].col) {
                char *color;
                switch (rocks[i].colour) {
                    case GREEN: color = "\x1B[32m"; break;
                    case TEAL:  color = "\x1B[96m"; break;
                    case PINK:  color = "\x1B[95m"; break;
                    case RED:   color = "\x1B[91m"; break;
                    default:    color = "\x1B[0m";  break;
                }
                printf("%so %s", color, RESET);
                i++;
            } else {
                printf("\u00B7 ");
            }
        }
        printf("\n");
    }

    free(rocks);
}

static int CompareRocks(const void *ptr1, const void *ptr2) {
    struct rock *r1 = (struct rock *)ptr1;
    struct rock *r2 = (struct rock *)ptr2;
    if (r1->row != r2->row) {
        return r2->row - r1->row;
    } else {
        return r1->col - r2->col;
    }
}

