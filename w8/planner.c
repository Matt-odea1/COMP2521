// Algorithms to design electrical grids

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "Place.h"
#include "PQ.h"

////////////////////////////////////////////////////////////////////////
// Your task

double calcWeight(Place p1, Place p2) {
    double xdiff = (abs(p1.x - p2.x)^2);
    double ydiff = (abs(p1.y - p2.y)^2);
    double final = sqrt((xdiff+ydiff));
    if (final > 0.0) {
        return final;
    }
    return 0.0;
}


/**
 * Designs  a minimal cost electrical grid that will deliver electricity
 * from a power plant to all the given cities. Power lines must be built
 * between cities or between a city and a power plant.  Cost is directly
 * proportional to the total length of power lines used.
 * Assumes  that  numCities  is at least 1 (numCities is the size of the
 * cities array).
 * Stores the power lines that need to be built in the given  powerLines
 * array, and returns the number of power lines stored. Assumes that the
 * powerLines array is large enough to store all required power lines.
 */
int planGrid1(Place cities[], int numCities, Place powerPlant, PowerLine powerLines[]) {
    Graph g = GraphNew(numCities + 1);
    Edge e = {0,0,0};
    PowerLine p;
    for (int i = 0; i < numCities; i++) {
        for (int j = i+1; j < numCities; j++) {
            p.p1 = cities[i];
            p.p2 = cities[j];

            e.v = i;
            e.w = j;
            e.weight = calcWeight(p.p1, p.p2);
            if (e.weight != 0) {
                GraphInsertEdge(g, e);
            }
        }
        p.p1 = cities[i];
        p.p2 = powerPlant;
        e.v = i;
        e.w = numCities;
        e.weight = calcWeight(p.p1, p.p2);
        if (e.weight != 0) {
            GraphInsertEdge(g, e);
        }
    }

    // calculate edge size between each city
    Graph MST = GraphMST(g);
    GraphShow(MST);
    return numCities;
}



////////////////////////////////////////////////////////////////////////
// Optional task

/**
 * Designs  a minimal cost electrical grid that will deliver electricity
 * to all the given cities.  Power lines must be built between cities or
 * between a city and a power plant.  Cost is directly  proportional  to
 * the  total  length of power lines used.  Assume that each power plant
 * generates enough electricity to supply all cities, so not  all  power
 * plants need to be used.
 * Assumes  that  numCities and numPowerPlants are at least 1 (numCities
 * and numPowerPlants are the sizes of the cities and powerPlants arrays
 * respectively).
 * Stores the power lines that need to be built in the given  powerLines
 * array, and returns the number of power lines stored. Assumes that the
 * powerLines array is large enough to store all required power lines.
 */
int planGrid2(Place cities[], int numCities,
              Place powerPlants[], int numPowerPlants,
              PowerLine powerLines[]) {
    // TODO: Complete this function
    return 0;
}
