// Reads a wall data file

// !!! DO NOT MODIFY THIS FILE !!!

#include <stdio.h>
#include <stdlib.h>

#include "Wall.h"

Wall readWall(char *wallFilename) {
    FILE *fp = fopen(wallFilename, "r");
    if (fp == NULL) {
        fprintf(stderr, "error: failed to open '%s' for reading\n",
                wallFilename);
        exit(EXIT_FAILURE);
    }

    // Reading the dimensions
    int height, width;
    if (fscanf(fp, "%d %d", &height, &width) != 2) {
        fprintf(stderr, "error: failed to read wall dimensions\n");
        exit(EXIT_FAILURE);
    }

    if (height <= 0 || width <= 0) {
        fprintf(stderr, "error: invalid dimensions\n");
        exit(EXIT_FAILURE);
    }

    Wall wall = WallNew(height, width);

    // Reading the rocks
    int row, col, colour;
    while (fscanf(fp, "%d %d %d", &row, &col, &colour) == 3) {
        if (row <= 0 || row >= height) {
            fprintf(stderr, "error: invalid row '%d'\n", row);
            exit(EXIT_FAILURE);
        } else if (col <= 0 || col >= width) {
            fprintf(stderr, "error: invalid column '%d'\n", col);
            exit(EXIT_FAILURE);
        } else if (colour < 0 || colour >= NUM_COLOURS) {
            fprintf(stderr, "error: invalid colour '%d'\n", colour);
            exit(EXIT_FAILURE);
        }

        struct rock rock = {row, col, colour};
        WallAddRock(wall, rock);
    }

    fclose(fp);

    return wall;
}

