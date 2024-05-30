// Main program for testing boulder climbing algorithms

// !!! DO NOT MODIFY THIS FILE !!!

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "climber.h"
#include "Wall.h"
#include "wallReader.h"

struct test {
    Wall wall;
    int reach;
    Colour colour;
    int energyCosts[NUM_COLOURS];
    int maxEnergy;
};

static void testFindShortestPath(FILE *fp);
static void testFindMinEnergyPath(FILE *fp);
static void testFindMinTurnsPath(FILE *fp);

static void scanWall(FILE *fp, Wall *wall);
static void scanReach(FILE *fp, int *reach);
static void scanColour(FILE *fp, Colour *colour);
static void scanEnergyCosts(FILE *fp, int energyCosts[]);
static void scanMaxEnergy(FILE *fp, int *maxEnergy);

static char *colourToStr(Colour c);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        errx(EXIT_FAILURE, "usage: %s <test file>", argv[0]);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        errx(EXIT_FAILURE, "error: failed to open '%s' for reading\n",
             argv[1]);
    }

    int task = 0;
    if (fscanf(fp, "%d", &task) != 1) {
        errx(EXIT_FAILURE, "error: failed to read task number\n");
    }

    switch (task) {
        case 1:
            testFindShortestPath(fp);
            break;
        case 2:
            testFindMinEnergyPath(fp);
            break;
        case 3:
            testFindMinTurnsPath(fp);
            break;
        default:
            errx(EXIT_FAILURE, "error: invalid task number '%d'\n", task);
    }

    fclose(fp);
}

////////////////////////////////////////////////////////////////////////

static void testFindShortestPath(FILE *fp) {
    struct test t = {};
    scanWall(fp, &(t.wall));
    scanReach(fp, &(t.reach));
    scanColour(fp, &(t.colour));

    printf("Wall:\n");
    WallPrint(t.wall);
    printf("\n");

    printf("Reach: %d\n", t.reach);
    printf("Colour: %s\n", colourToStr(t.colour));
    printf("\n");

    struct path path = findShortestPath(t.wall, t.reach, t.colour);

    if (path.numRocks == 0) {
        printf("No path found!\n");
    } else {
        printf("Path:\n");
        for (int i = 0; i < path.numRocks; i++) {
            struct rock r = path.rocks[i];
            printf("-> %s rock at (%d, %d)\n", colourToStr(r.colour),
                   r.row, r.col);
        }
        printf("\nTotal rocks: %d\n", path.numRocks);
    }

    WallFree(t.wall);
    free(path.rocks);
}

static void testFindMinEnergyPath(FILE *fp) {
    struct test t = {};
    scanWall(fp, &(t.wall));
    scanReach(fp, &(t.reach));
    scanEnergyCosts(fp, t.energyCosts);

    printf("Wall:\n");
    WallPrint(t.wall);
    printf("\n");

    printf("Reach: %d\n", t.reach);
    printf("Energy costs:\n");
    for (Colour c = 0; c < NUM_COLOURS; c++) {
        printf("- %s: %d\n", colourToStr(c), t.energyCosts[c]);
    }
    printf("\n");

    struct path path = findMinEnergyPath(t.wall, t.reach, t.energyCosts);

    if (path.numRocks == 0) {
        printf("No path found!\n");
    } else {
        printf("Path:\n");
        int totalEnergy = 0;
        for (int i = 0; i < path.numRocks; i++) {
            struct rock r = path.rocks[i];
            printf("-> %s rock at (%d, %d)\n", colourToStr(r.colour),
                   r.row, r.col);
            totalEnergy += t.energyCosts[path.rocks[i].colour];
        }
        printf("\nTotal energy: %d\n", totalEnergy);
    }

    WallFree(t.wall);
    free(path.rocks);
}

static void testFindMinTurnsPath(FILE *fp) {
    struct test t = {};
    scanWall(fp, &(t.wall));
    scanReach(fp, &(t.reach));
    scanEnergyCosts(fp, t.energyCosts);
    scanMaxEnergy(fp, &(t.maxEnergy));

    printf("Wall:\n");
    WallPrint(t.wall);
    printf("\n");

    printf("Reach: %d\n", t.reach);
    printf("Energy costs:\n");
    for (Colour c = 0; c < NUM_COLOURS; c++) {
        printf("- %s: %d\n", colourToStr(c), t.energyCosts[c]);
    }
    printf("Maximum energy: %d\n", t.maxEnergy);
    printf("\n");

    struct path path = findMinTurnsPath(t.wall, t.reach, t.energyCosts,
                                        t.maxEnergy);

    if (path.numRocks == 0) {
        printf("No path found!\n");
    } else {
        printf("Path:\n");
        int energyLeft = t.maxEnergy;
        for (int i = 0; i < path.numRocks; i++) {
            struct rock r = path.rocks[i];
            printf("-> ");
            if (i > 0 && r.row == path.rocks[i - 1].row &&
                    r.col && path.rocks[i - 1].col) {
                printf("Rest");
                energyLeft = t.maxEnergy;
            } else {
                printf("%s rock at (%d, %d)", colourToStr(r.colour),
                       r.row, r.col);
                energyLeft -= t.energyCosts[r.colour];
            }
            printf(" (energy left: %d)\n", energyLeft);
        }
        printf("\nTotal turns: %d\n", path.numRocks);
    }

    WallFree(t.wall);
    free(path.rocks);
}

////////////////////////////////////////////////////////////////////////

static void scanWall(FILE *fp, Wall *wall) {
    char filename[100] = {0};
    if (fscanf(fp, "%s", filename) != 1) {
        errx(EXIT_FAILURE, "error: failed to read wall filename\n");
    }
    char path[100] = {0};
    snprintf(path, sizeof(char)*100, "data/%s", filename);

    *wall = readWall(path);
}

static void scanReach(FILE *fp, int *reach) {
    if (fscanf(fp, "%d", reach) != 1) {
        errx(EXIT_FAILURE, "error: failed to read reach\n");
    }

    if (*reach <= 0) {
        errx(EXIT_FAILURE, "error: invalid reach '%d'\n", *reach);
    }
}

static void scanColour(FILE *fp, Colour *colour) {
    if (fscanf(fp, "%d", colour) != 1) {
        errx(EXIT_FAILURE, "error: failed to read colour\n");
    }

    if (*colour < 0 || *colour >= NUM_COLOURS) {
        errx(EXIT_FAILURE, "error: invalid colour '%d'\n", *colour);
    }
}

static void scanEnergyCosts(FILE *fp, int energyCosts[]) {
    for (int i = 0; i < NUM_COLOURS; i++) {
        if (fscanf(fp, "%d", &energyCosts[i]) != 1) {
            errx(EXIT_FAILURE, "error: failed to read energy cost\n");
        }

        if (energyCosts[i] < 0) {
            errx(EXIT_FAILURE, "error: invalid energy cost '%d'\n",
                 energyCosts[i]);
        }
    }
}

static void scanMaxEnergy(FILE *fp, int *maxEnergy) {
    if (fscanf(fp, "%d", maxEnergy) != 1) {
        errx(EXIT_FAILURE, "error: failed to read max energy\n");
    }

    if (*maxEnergy <= 0) {
        errx(EXIT_FAILURE, "error: invalid energy '%d'\n", *maxEnergy);
    }
}

////////////////////////////////////////////////////////////////////////

static char *colourToStr(Colour c) {
    switch (c) {
        case GREEN: return "Green";
        case TEAL:  return "Teal";
        case PINK:  return "Pink";
        case RED:   return "Red";
        default:    return "Unknown";
    }
}

////////////////////////////////////////////////////////////////////////

