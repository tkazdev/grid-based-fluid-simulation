#pragma once

typedef struct {
    int sizeX;
    int sizeY;
} Simulation;

Simulation createSimulation();
void updateSimulation(Simulation *sim);