// Main program for testing the Wall ADT

// !!! DO NOT MODIFY THIS FILE !!!

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Wall.h"
#include "wallReader.h"

#define MAX 8192

static void processArgs(int argc, char *argv[], char **wallFilename);
static void showUsage(char *progName);
static void showWelcomeMessage(void);
static bool getCommand(char *buf);
static char **tokenize(char *s, int *ntokens);
static void freeTokens(char **tokens);
static void showHelp(void);

static void runWallDimensions(Wall w, int argc, char *argv[]);
static void runWallAddRock(Wall w, int argc, char *argv[]);
static void runWallNumRocks(Wall w, int argc, char *argv[]);
static void runWallGetAllRocks(Wall w, int argc, char *argv[]);
static void runWallGetRocksInRange(Wall w, int argc, char *argv[]);
static void runWallGetColouredRocksInRange(Wall w, int argc, char *argv[]);
static void runWallPrint(Wall w, int argc, char *argv[]);

static void showCmdHelp(void);

static void printRocksOnWall(Wall w, struct rock *rocks, int numRocks,
                             int row, int col);
static int compareRocks(const void *ptr1, const void *ptr2);

static char *colourToStr(Colour c);

////////////////////////////////////////////////////////////////////////

typedef struct command {
    char  *code;
    void (*fn)(Wall, int, char **);
    char  *argHint;
    char  *helpMsg;
} Command;

static Command COMMANDS[] = {
    {"d", runWallDimensions,
     "<name>",
     "get the dimensions of the wall"},
    {"+", runWallAddRock,
     "<row> <col> <colour>",
     "add a rock to the wall"},
    {"n", runWallNumRocks,
     "",
     "get the number of rocks on the wall"},
    {"a", runWallGetAllRocks,
     "",
     "get all the rocks on the wall"},
    {"r", runWallGetRocksInRange,
     "<row> <col> <dist>",
     "get all the rocks within a certain distance"},
    {"c", runWallGetColouredRocksInRange,
     "<row> <col> <dist> <colour>",
     "get all the rocks of a particular colour within a certain distance"},
    {"p", runWallPrint,
     "",
     "print out the wall"},
    // Meta-commands
    {"?", NULL, "", "show this message"},
    {"q", NULL, "", "quit"},
};

////////////////////////////////////////////////////////////////////////

static Command *currCommand = NULL;

int main(int argc, char *argv[]) {
    char *wallFilename;
    processArgs(argc, argv, &wallFilename);

    Wall w = readWall(wallFilename);

    showWelcomeMessage();

    bool done = false;
    char cmd[MAX + 1] = {0};

    while (!done && getCommand(cmd)) {
        int ntokens = 0;
        char **tokens = tokenize(cmd, &ntokens);
        if (ntokens == 0) {
            free(tokens);
            continue;
        }
        
        char *cmdName = tokens[0];

        // Meta-commands
        if (strcmp(cmdName, "?") == 0) {
            showHelp();
        } else if (strcmp(cmdName, "q") == 0) {
            done = true;
        
        // Actual commands
        } else {
            bool validCommand = false;

            int numCommands = sizeof(COMMANDS) / sizeof(Command);
            for (int i = 0; i < numCommands; i++) {
                if (strcmp(cmdName, COMMANDS[i].code) == 0) {
                    validCommand = true;
                    currCommand = &COMMANDS[i];
                    COMMANDS[i].fn(w, ntokens, tokens);
                }
            }

            if (!validCommand) {
                printf("Unknown command '%s'\n", cmdName);
            }
        }
        freeTokens(tokens);
    }

    WallFree(w);
}

static void processArgs(int argc, char *argv[], char **wallFilename) {
    if (argc != 2) {
        showUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    *wallFilename = argv[1];
}

static void showUsage(char *progName) {
    printf("Usage: %s <wall data file>\n", progName);
}

static void showWelcomeMessage(void) {
    printf("Wall ADT Tester\n");
    printf("Enter ? to see the list of commands.\n");
}

static bool getCommand(char *buf) {
    printf("> ");
    if (fgets(buf, MAX, stdin) != NULL) {
        int len = strlen(buf);
        if (len > 0 && buf[len - 1] != '\n') {
            buf[len] = '\n';
            buf[len + 1] = '\0';
        }
        return true;
    } else {
        return false;
    }
}

static char **tokenize(char *s, int *ntokens) {
    char *separators = " \t\r\n";
    *ntokens = 0;

    char **tokens = calloc((strlen(s) + 1), sizeof(*tokens));
    assert(tokens != NULL);

    while (*s != '\0') {
        s += strspn(s, separators);

        if (*s == '\0') {
            break;
        }

        size_t length = strcspn(s, separators);

        char *token = strndup(s, length);
        assert(token != NULL);
        s += length;

        tokens[*ntokens] = token;
        (*ntokens)++;
    }

    tokens[*ntokens] = NULL;

    tokens = realloc(tokens, (*ntokens + 1) * sizeof(*tokens));
    assert(tokens != NULL);

    return tokens;
}

static void freeTokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

static void showHelp(void) {
    printf("Commands:\n");
    int numCommands = sizeof(COMMANDS) / sizeof(Command);
    for (int i = 0; i < numCommands; i++) {
        printf("%5s %-30s %s\n", COMMANDS[i].code, COMMANDS[i].argHint,
                                 COMMANDS[i].helpMsg);
    }
    printf("\n");
}

////////////////////////////////////////////////////////////////////////
// Commands

static void runWallDimensions(Wall w, int argc, char *argv[]) {
    if (argc != 1) {
        showCmdHelp();
        return;
    }

    int height = WallHeight(w);
    int width = WallWidth(w);
    printf("Height: %d, Width: %d\n", height, width);
}

static void runWallAddRock(Wall w, int argc, char *argv[]) {
    if (argc != 4) {
        showCmdHelp();
        return;
    }

    char s[MAX];
    snprintf(s, sizeof(char)*8192, "%s %s %s", argv[1], argv[2], argv[3]);
    int row;
    int col;
    int colour;
    char c;
    if (sscanf(s, "%d %d %d%c", &row, &col, &colour, &c) != 3) {
        showCmdHelp();
        return;
    }
    if (row <= 0 || row >= WallHeight(w)) {
        printf("Invalid row number\n");
        return;
    }
    if (col <= 0 || col >= WallWidth(w)) {
        printf("Invalid column number\n");
        return;
    }
    if (colour < 0 || colour >= NUM_COLOURS) {
        printf("Invalid colour\n");
        return;
    }

    WallAddRock(w, (struct rock){
        .row = row,
        .col = col,
        .colour = colour,
    });
    printf("Rock added!\n");
}

static void runWallNumRocks(Wall w, int argc, char *argv[]) {
    if (argc != 1) {
        showCmdHelp();
        return;
    }

    int numRocks = WallNumRocks(w);
    printf("There are %d rocks\n", numRocks);
}

static void runWallGetAllRocks(Wall w, int argc, char *argv[]) {
    if (argc != 1) {
        showCmdHelp();
        return;
    }

    int totalRocks = WallNumRocks(w);
    struct rock *rocks = malloc(totalRocks * sizeof(struct rock));
    assert(rocks != NULL);
    int numRocks = WallGetAllRocks(w, rocks);
    
    printf("All rocks:\n\n");
    printRocksOnWall(w, rocks, numRocks, -1, -1);
    free(rocks);
}

static void runWallGetRocksInRange(Wall w, int argc, char *argv[]) {
    if (argc != 4) {
        showCmdHelp();
        return;
    }

    char s[MAX];
    snprintf(s, sizeof(char)*8192, "%s %s %s", argv[1], argv[2], argv[3]);
    int row;
    int col;
    int dist;
    char c;
    if (sscanf(s, "%d %d %d%c", &row, &col, &dist, &c) != 3) {
        showCmdHelp();
        return;
    }
    if (row < 0 || row > WallHeight(w)) {
        printf("Invalid row number\n");
        return;
    }
    if (col < 0 || col > WallWidth(w)) {
        printf("Invalid column number\n");
        return;
    }
    if (dist < 0) {
        printf("Invalid dist\n");
        return;
    }

    int totalRocks = WallNumRocks(w);
    struct rock *rocks = malloc(totalRocks * sizeof(struct rock));
    assert(rocks != NULL);
    int numRocks = WallGetRocksInRange(w, row, col, dist, rocks);
    
    printf("Rocks within %d units of (%d, %d):\n\n", dist, row, col);
    printRocksOnWall(w, rocks, numRocks, row, col);
    free(rocks);
}

static void runWallGetColouredRocksInRange(Wall w, int argc, char *argv[]) {
    if (argc != 5) {
        showCmdHelp();
        return;
    }

    char s[MAX];
    snprintf(s, sizeof(char)*8192, "%s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
    int row;
    int col;
    int dist;
    int colour;
    char c;
    if (sscanf(s, "%d %d %d %d%c", &row, &col, &dist, &colour, &c) != 4) {
        showCmdHelp();
        return;
    }
    if (row < 0 || row > WallHeight(w)) {
        printf("Invalid row number\n");
        return;
    }
    if (col < 0 || col > WallWidth(w)) {
        printf("Invalid column number\n");
        return;
    }
    if (dist < 0) {
        printf("Invalid dist\n");
        return;
    }
    if (colour < 0 || colour >= NUM_COLOURS) {
        printf("Invalid colour\n");
        return;
    }

    int totalRocks = WallNumRocks(w);
    struct rock *rocks = malloc(totalRocks * sizeof(struct rock));
    assert(rocks != NULL);
    int numRocks = WallGetColouredRocksInRange(w, row, col, dist,
                                               colour, rocks);
    printf("%s rocks within %d units of (%d, %d):\n\n",
           colourToStr(colour), dist, row, col);
    printRocksOnWall(w, rocks, numRocks, row, col);
    free(rocks);
}

static void runWallPrint(Wall w, int argc, char *argv[]) {
    if (argc != 1) {
        showCmdHelp();
        return;
    }

    WallPrint(w);
}

static void showCmdHelp(void) {
    printf("Usage: %s %s\n", currCommand->code, currCommand->argHint);
}

////////////////////////////////////////////////////////////////////////

static void printRocksOnWall(Wall w, struct rock *rocks, int numRocks,
                             int row, int col)
{
    int height = WallHeight(w);
    int width = WallWidth(w);
    qsort(rocks, numRocks, sizeof(struct rock), compareRocks);

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
}

static int compareRocks(const void *ptr1, const void *ptr2) {
    struct rock *r1 = (struct rock *)ptr1;
    struct rock *r2 = (struct rock *)ptr2;
    if (r1->row != r2->row) {
        return r2->row - r1->row;
    } else {
        return r1->col - r2->col;
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

