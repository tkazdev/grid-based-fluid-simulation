#pragma once

#include <stdbool.h>

typedef struct {
    const int sizeX;
    const int sizeY;
    const int cellCount;
    const float cellWidth;
    float fluidDensity;
    float *velocitiesH;
    float *velocitiesV;
    float *pressures;

    float frameTimestep;
    int projectionRepeats;

} Simulation;

typedef struct {
    int gridWidth;
    int gridHeight;
    float cellWidth;
    float fluidDensity;
    float frameTimestep;
    int projectionRepeats;
} SimulationSettings;

Simulation createSimulation(const SimulationSettings *settings);
void deleteSimulation(Simulation *sim);
void updateSimulation(Simulation *sim);
int getVelocitiesCountH(const Simulation *sim);
int getVelocitiesCountV(const Simulation *sim);

bool isBorderCell(const Simulation *sim, int cellX, int cellY);
float horizontalVelocityAt(const Simulation *sim, int cellX, int cellY); // Returns velocity from the left
float verticalVelocityAt(const Simulation *sim, int cellX, int cellY); // Returns velocity from the bottom
float pressureAt(const Simulation *sim, int cellX, int cellY);
float divergenceAt(const Simulation *sim, int cellX, int cellY);
void updateHorizontalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity);
void updateVerticalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity);
void updatePressureAt(Simulation *sim, int cellX, int cellY, float newPressure);