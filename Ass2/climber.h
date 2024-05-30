// Interface to boulder climbing algorithms

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef CLIMBER_H
#define CLIMBER_H

#include "Wall.h"

struct path {
    int numRocks;
    struct rock *rocks;
};

struct path findShortestPath(Wall w, int reach, Colour colour);

struct path findMinEnergyPath(Wall w, int reach, int energyCosts[NUM_COLOURS]);

struct path findMinTurnsPath(Wall w, int reach, int energyCosts[NUM_COLOURS],
                             int maxEnergy);

#endif

