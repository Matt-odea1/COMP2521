// z5413887 Submission - COMP2521 ASS2

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "climber.h"
#include "Wall.h"

typedef struct rock Rock;
struct wall {
    int height;
    int width;
    int numRocks;
    Rock** rocks;
};

typedef struct edge {
    Rock from;
    Rock to;
    double distance;
} Edge;

struct energyPath {
    struct path path;
    int energyCost;
};

static struct path DijkstraSolve(int numRocks, Edge* edgeList, int numEdges, Rock allRocks[], 
    int startRockIndex, int finishRockIndex, int reach, int energyCosts[]);
static struct path DijkstraSolveEnergy(int numRocks, Edge* edgeList, int numEdges, 
    Rock allRocks[], int startRockIndex, int finishRockIndex, int reach, int energyCosts[]);
//static struct energyPath DijkstraSolveEnergyRest(int numRocks, Edge* edgeList, int numEdges, 
//    Rock allRocks[], int startRockIndex, int finishRockIndex, int reach, int energyCosts[], int maxEnergy);
static double CalcRange(int p1col, int p1row, int p2col, int p2row, double radius);
static int getRockIndex(Rock allRocks[], Rock target, int numRocks);
static bool allVisited(bool visited[], int numRocks);

static struct path FindBestSolution(Wall w, int numRocks, Edge* EdgeList, int numEdges, 
    Rock allRocks[], int reach, int energyCosts[], struct path (*f)(int, Edge*, int, Rock[], int, int, int, int[]));

// --------------------------------------------------------------------------------------------------------------------------------------
//                                                           TASK 1
// --------------------------------------------------------------------------------------------------------------------------------------

struct path findShortestPath(Wall w, int reach, Colour colour) {
    Rock allRocks[w->height*w->width];
    // get array of all coloured rocks within the wall (use pythagoras to ensure we search a large enough radius)
    int numRocks = WallGetColouredRocksInRange(w, 0, 0, sqrt((w->width * w->width) + 
        (w->height * w->height)) + 1, colour, allRocks);

    // 2D Array that shows the rocks reachable from each other rock
    Rock** reachables = malloc(numRocks * sizeof(Rock));
    for (int i = 0; i <= numRocks; i++) {
        reachables[i] = malloc(numRocks * sizeof(Rock));
    }
    // init reachable rocks array
    for (int i = 0; i < numRocks; i++) {
        WallGetColouredRocksInRange(w, allRocks[i].row, allRocks[i].col, reach, colour, reachables[i]);
    } 

    // Array of all the edges to consider
    Edge* EdgeList = malloc(numRocks * numRocks * sizeof(Edge));
    // init edge array
    int numEdges = 0;
    for (int i = 0; i < numRocks; i++) {
        for (int j = 0; j < numRocks; j++) {
            if (reachables[i][j].col > -1 && reachables[i][j].row > -1) {
                EdgeList[numEdges].from = allRocks[i];
                EdgeList[numEdges].to = reachables[i][j];
                EdgeList[numEdges].distance = CalcRange(allRocks[i].col, 
                    allRocks[i].row, reachables[i][j].col, reachables[i][j].row, reach);
                numEdges++;
            }
        }
    }

    int energyCosts[] = {0, 0, 0, 0};  
    // loop through all possible starts and finishes, return one of the ones with the smallest number of rocks 
    struct path p = FindBestSolution(w, numRocks, EdgeList, numEdges, allRocks, reach, energyCosts, DijkstraSolve);

    free(EdgeList);
    free(reachables);

    Rock temp;
    for(int i = 0; i<p.numRocks/2; i++){
        temp = p.rocks[i];
        p.rocks[i] = p.rocks[p.numRocks-i-1];
        p.rocks[p.numRocks-i-1] = temp;
    }

    return p;
}

// --------------------------------------------------------------------------------------------------------------------------------------
//                                                           TASK 2
// --------------------------------------------------------------------------------------------------------------------------------------

struct path findMinEnergyPath(Wall w, int reach, int energyCosts[NUM_COLOURS]) {
    Rock allRocks[w->height*w->width];
    int numRocks = WallGetAllRocks(w, allRocks);

    // 2D Array that shows the rocks reachable from each other rock
    Rock** reachables = malloc(numRocks * sizeof(Rock));
    for (int i = 0; i <= numRocks; i++) {
        reachables[i] = malloc(numRocks * sizeof(Rock));
    }
    // init reachable rocks array
    for (int i = 0; i < numRocks; i++) {
        WallGetRocksInRange(w, allRocks[i].row, allRocks[i].col, reach, reachables[i]);
    }

    // init edge array
    int numEdges = 0;
    Edge* EdgeList = malloc(numRocks * numRocks * sizeof(Edge));
    for (int i = 0; i < numRocks; i++) {
        for (int j = 0; j < numRocks; j++) {
            if (reachables[i][j].col > -1) {
                EdgeList[numEdges].from = allRocks[i];
                EdgeList[numEdges].to = reachables[i][j];
                EdgeList[numEdges].distance = CalcRange(allRocks[i].col, 
                    allRocks[i].row, reachables[i][j].col, reachables[i][j].row, reach);
                numEdges++;
            }
        }
    }

    struct path p = FindBestSolution(w, numRocks, EdgeList, numEdges, allRocks, reach, energyCosts, DijkstraSolveEnergy);

    int countRocks = 0;
    while (abs(p.rocks[countRocks].col) < 1000 && abs(p.rocks[countRocks].row) < 1000) {
        countRocks++;
    }
    p.numRocks = countRocks;

    free(EdgeList);
    free(reachables);

    Rock temp;
    for(int i = 0; i<p.numRocks/2; i++){
        temp = p.rocks[i];
        p.rocks[i] = p.rocks[p.numRocks-i-1];
        p.rocks[p.numRocks-i-1] = temp;
    }

    return p;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Given a wall and a list of edges, this function will return the best solution from all the start and end positions possible,
// using the most relevant version of dijkstra's algorithm applicable
static struct path FindBestSolution(Wall w, int numRocks, Edge* EdgeList, int numEdges, 
    Rock allRocks[], int reach, int energyCosts[], struct path (*f)(int, Edge*, int, Rock[], int, int, int, int[])) {

    // get all possible start rocks
    Rock startRocks[w->width*reach];
    int nPossibleStarts = 0;
    for (int i = 0; i < numRocks; i++) {
        if (allRocks[i].row <= reach) {
            startRocks[nPossibleStarts] = allRocks[i];
            nPossibleStarts++;
        }
    }
    // get all possible end rocks
    Rock finishRocks[w->width*reach];
    int nPossibleFinishes = 0;
    for (int i = 0; i < numRocks; i++) {
        if (allRocks[i].row >= w->height - reach) {
            finishRocks[nPossibleFinishes] = allRocks[i];
            nPossibleFinishes++;
        }
    }

    // loop through all possible starts and finishes, return one of the ones with the smallest number of rocks
    struct path p = (*f)(numRocks, EdgeList, numEdges, allRocks, 
        getRockIndex(allRocks, startRocks[0], numRocks), getRockIndex(allRocks, finishRocks[0], numRocks), reach, energyCosts);
    struct path temp = {0, };
    for (int i = 0; i < nPossibleFinishes; i++) {
        for (int j = 0; j < nPossibleStarts; j++) {
            temp = (*f)(numRocks, EdgeList, numEdges, allRocks, getRockIndex(allRocks, startRocks[j], 
                numRocks), getRockIndex(allRocks, finishRocks[i], numRocks), reach, energyCosts);
            if (temp.numRocks < p.numRocks && temp.numRocks !=0) {
                p.rocks = temp.rocks;
                p.numRocks = temp.numRocks;
            }
        }
    }
    return p;
}

// Given an array of rocks, returns the index of the target rock
static int getRockIndex(Rock allRocks[], Rock target, int numRocks) {
    int index = 0;
    for (int i = 0; i < numRocks; i++) {
        if (allRocks[i].col == target.col && allRocks[i].row == target.row) {
            return index;
        }
        index++;
    }
    return -1;
}

// returns the distance between two points, or 100 if the points are not within a reachable radius
static double CalcRange(int p1col, int p1row, int p2col, int p2row, double radius) {
    double distance = sqrt((p1col - p2col) * (p1col - p2col) + (p1row - p2row) * (p1row - p2row));
    if (distance <= radius) {
        return distance;
    }
    return (double)100.0;
}

// Given an array of bools, returns true if they are all visited
static bool allVisited(bool visited[], int numRocks) {
    for (int i = 0; i < numRocks; i++) {
        if (visited[i] == false) return false;
    }
    return true;
}



// Implementing Dijkstra's shortest path algorithm
static struct path DijkstraSolve(int numRocks, Edge* edgeList, int numEdges, 
    Rock allRocks[], int startRockIndex, int finishRockIndex, int reach, int energyCosts[]) {

    // Create an array to hold the distances from the starting vertex to each other vertex in the graph. 
    Edge* startToVDistance = malloc(numRocks * sizeof(Edge));
    // Create empty predeccessor array
    Rock* pred = malloc(numRocks * sizeof(Rock));
    // Create array of bools, see if element is visited
    bool* vVisted = malloc(numRocks * sizeof(bool));
    // Initialize the distance to the starting vertex as 0, and all other distances as 10000.
    Rock currentVertex = allRocks[startRockIndex];
    for (int i = 0; i < numRocks; i++) {
        startToVDistance[i].from = allRocks[startRockIndex];
        startToVDistance[i].to = allRocks[i];
        startToVDistance[i].distance = 10000;
        vVisted[i] = false;
        pred[i].col = 0;
        pred[i].row = 0;
        pred[i].colour = 0;
    }
    startToVDistance[startRockIndex].distance = 0;

 
    // While the set of unvisited vertices is not empty:
    while (!allVisited(vVisted, numRocks)) {

        //a. Find the vertex with the smallest distance from the starting vertex. This will be the current vertex considered
        int indexCurrentReplacement = -1;
        int currentMinDistance = 100;
        for (int i = 0; i < numRocks; i++) {
            if (startToVDistance[i].distance < currentMinDistance && vVisted[i] == false) {
                currentMinDistance = startToVDistance[i].distance;
                indexCurrentReplacement = i;
            }
        }
        if (indexCurrentReplacement == -1) break;
        currentVertex = allRocks[indexCurrentReplacement];

        //b. For each neighbor of the current vertex:
        for (int i = 0; i < numEdges; i++) {
            if (edgeList[i].from.col == currentVertex.col && edgeList[i].from.row == currentVertex.row) {
                int targetNeighbourIndex = getRockIndex(allRocks, edgeList[i].to, numRocks);

                //i. Calculate the distance from the starting vertex to the neighbor through the current vertex.
                double distance = startToVDistance[indexCurrentReplacement].distance + CalcRange(currentVertex.col, 
                    currentVertex.row, allRocks[targetNeighbourIndex].col, allRocks[targetNeighbourIndex].row, reach);

                //ii. If this distance is less than the current distance stored in the distance array for the neighbor,
                // update the distance array with the new distance. Also set neighbour's predecessor to this vertex.
                if (distance < startToVDistance[targetNeighbourIndex].distance) {
                    startToVDistance[targetNeighbourIndex].distance = distance;

                    pred[targetNeighbourIndex].col = currentVertex.col;
                    pred[targetNeighbourIndex].row = currentVertex.row;
                    pred[targetNeighbourIndex].colour = currentVertex.colour;
                }
            }
        }

        //c. Mark the current vertex as visited and remove it from the set of unvisited vertices.
        vVisted[indexCurrentReplacement] = true;
    }
    
    struct path p;
    p.rocks = malloc(numRocks * sizeof(Rock));
    int countPathNum = 0;
    int prevRockIndex = finishRockIndex;

    // from the finish rock, trace through the predecessors along the shortest path, until we find the beginning
    while (true) {
        p.rocks[countPathNum] = allRocks[prevRockIndex];
        countPathNum++;
        // if reach start, break.
        if (( allRocks[prevRockIndex].col == allRocks[startRockIndex].col 
            && allRocks[prevRockIndex].row == allRocks[startRockIndex].row )) {
            break;
        }
        // if finish rock has no predecessor, it must not be reachable. return {0,0}
        if (pred[prevRockIndex].col == 0 && pred[prevRockIndex].row == 0) {
            struct path badPath = {0, };
            return badPath;
        }
        // trace backwards
        prevRockIndex = getRockIndex(allRocks, pred[prevRockIndex], numRocks);
    }
    p.numRocks = countPathNum;
    free(startToVDistance);
    free(vVisted);
    return p;
}



// Dijkstra's algorithm to solve for the shortest path in terms of energy, NOT number of rocks
static struct path DijkstraSolveEnergy(int numRocks, Edge* edgeList, int numEdges, Rock allRocks[], 
    int startRockIndex, int finishRockIndex, int reach, int energyCosts[]) {

    // Create an array to hold the energy cost from the starting vertex to each other vertex in the graph. 
    Edge* startToVEnergy = malloc(numRocks * sizeof(Edge));
    Rock* pred = malloc(numRocks * sizeof(Rock));
    bool* vVisted = malloc(numRocks * sizeof(bool));
    
    // Initialize the distance to the starting vertex to energy cost to get there, and all other distances as 10000.
    Rock currentVertex = allRocks[startRockIndex];
    for (int i = 0; i < numRocks; i++) {
        startToVEnergy[i].from = allRocks[startRockIndex];
        startToVEnergy[i].to = allRocks[i];
        startToVEnergy[i].distance = 10000;
        vVisted[i] = false;
        pred[i].col = 0;
        pred[i].row = 0;
        pred[i].colour = 0;
    }
    startToVEnergy[startRockIndex].distance = energyCosts[allRocks[startRockIndex].colour];

    // While the set of unvisited vertices is not empty:
    while (!allVisited(vVisted, numRocks)) {

        //a. Find the vertex with the smallest distance from the starting vertex. This will be the current vertex.
        int indexCurrentReplacement = -1;
        int currentMinEnergy = 1000;
        for (int i = 0; i < numRocks; i++) {
            if (vVisted[i] == false) {
                // if edge has not been considered from the list and it is still not visited, it may not be reachable
                if (startToVEnergy[i].distance == 10000) {
                    continue;
                }
                if ((int)startToVEnergy[i].distance < currentMinEnergy) {
                    currentMinEnergy = startToVEnergy[i].distance;
                    indexCurrentReplacement = i;
                }
            }
        }
        // if no suitable replacements can be found, then not all vertices can be visited
        if (indexCurrentReplacement == -1) break;
        currentVertex = allRocks[indexCurrentReplacement];

        //b. For each neighbor of the current vertex:
        for (int i = 0; i < numEdges; i++) {
            if (edgeList[i].from.col == currentVertex.col && edgeList[i].from.row == currentVertex.row) {
                int targetNeighbourIndex = getRockIndex(allRocks, edgeList[i].to, numRocks);

                //i. Calculate the distance from the starting vertex to the neighbor through the current vertex.
                int distance = startToVEnergy[indexCurrentReplacement].distance + energyCosts[allRocks[targetNeighbourIndex].colour];

                //ii. If this distance is less than the current distance stored in the distance array for the neighbor,
                // update the distance array with the new distance.
                if (distance < startToVEnergy[targetNeighbourIndex].distance) {
                    startToVEnergy[targetNeighbourIndex].distance = distance;

                    pred[targetNeighbourIndex].col = currentVertex.col;
                    pred[targetNeighbourIndex].row = currentVertex.row;
                    pred[targetNeighbourIndex].colour = currentVertex.colour;
                }
            }
        }

        //c. Mark the current vertex as visited and remove it from the set of unvisited vertices.
        vVisted[indexCurrentReplacement] = true;
    }
    
    struct path p;
    p.rocks = malloc(numRocks * sizeof(Rock));
    int countPathNum = 0;
    int prevRockIndex = finishRockIndex;

    // starting from the finish, move backwards through predecessors to the final node along the shortest path
    while (true) {
        p.rocks[countPathNum] = allRocks[prevRockIndex];
        countPathNum++;
        // if we reach the start, stop
        if (( allRocks[prevRockIndex].col == allRocks[startRockIndex].col && allRocks[prevRockIndex].row == allRocks[startRockIndex].row )) {
            break;
        }
        // if we reach a dead node, stop
        if (pred[prevRockIndex].col == 0 && pred[prevRockIndex].row == 0) {
            break;
        }
        prevRockIndex = getRockIndex(allRocks, pred[prevRockIndex], numRocks);
    }

    int energy = startToVEnergy[finishRockIndex].distance;
    p.numRocks = energy;

    free(startToVEnergy);
    free(vVisted);
    return p;
}