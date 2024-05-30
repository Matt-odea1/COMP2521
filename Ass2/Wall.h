// Interface to the Wall ADT

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef WALL_H
#define WALL_H

typedef struct wall *Wall;

// Rock colours
#define NUM_COLOURS 4

// enums are used to create groups of related constants. It is like
// creating multiple #defines, except you can also provide a type name.
typedef enum {
    GREEN = 0,
    TEAL  = 1,
    PINK  = 2,
    RED   = 3,
} Colour;

// Terminal output colours
#define GRN "\x1B[32m"
#define CYN "\x1B[96m"
#define MAG "\x1B[95m"
#define RD  "\x1B[91m"
#define RESET "\x1B[0m"

struct rock {
    int row;
    int col;
    Colour colour;
};

/**
 * Creates a new blank wall with the given dimensions
 */
Wall WallNew(int height, int width);

/**
 * Frees all memory allocated to the wall 
 */
void WallFree(Wall w);

/**
 * Returns the height of the wall
 */
int WallHeight(Wall w);

/**
 * Returns the width of the wall
 */
int WallWidth(Wall w);

/**
 * Adds a rock to the wall
 * If there is already a rock at the given coordinates, replaces it
 */
void WallAddRock(Wall w, struct rock rock);

/**
 * Returns the number of rocks on the wall
 */
int WallNumRocks(Wall w);

/**
 * Stores all rocks on the wall in the given `rocks` array and returns
 * the number of rocks stored. Assumes that the array is at least as
 * large as the number of rocks on the wall. 
 */
int WallGetAllRocks(Wall w, struct rock rocks[]);

/**
 * Stores all rocks that are within a distance of `dist` from the given
 * coordinates in the given `rocks` array and returns the number of rocks
 * stored. Assumes that the array is at least as large as the number of
 * rocks on the wall.
 */
int WallGetRocksInRange(Wall w, int row, int col, int dist,
                        struct rock rocks[]);

/**
 * Stores all rocks with the colour `colour` that are within a distance
 * of `dist` from the given coordinates in the given `rocks` array and
 * returns the number of rocks stored. Assumes that the array is at
 * least as large as the number of rocks on the wall.
 */
int WallGetColouredRocksInRange(Wall w, int row, int col, int dist,
                                Colour colour, struct rock rocks[]);

/**
 * Prints the wall out in a nice format
 */
void WallPrint(Wall w);

#endif

