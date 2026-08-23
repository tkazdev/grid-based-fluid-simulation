#include <stdlib.h>
#include <stdio.h>

#include "Simulation.h"

bool isBorderCell(const Simulation *sim, int cellX, int cellY) {
    return cellX == 0 || cellX == sim->sizeX - 1 || cellY == 0 || cellY == sim->sizeY - 1;
}


float horizontalVelocityAt(const Simulation *sim, int cellX, int cellY) {
    return sim->velocitiesH[cellY + cellX * sim->sizeY];
}

float verticalVelocityAt(const Simulation *sim, int cellX, int cellY) {
    return sim->velocitiesV[cellX + cellY * sim->sizeX];
}

float pressureAt(const Simulation *sim, int cellX, int cellY) {
    return sim->pressures[cellX + cellY * sim->sizeX];
}


void updateHorizontalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity) {
    sim->velocitiesH[cellY + cellX * sim->sizeY] = newVelocity;
}

void updateVerticalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity) {
    sim->velocitiesV[cellX + cellY * sim->sizeX] = newVelocity;
}

void updatePressureAt(Simulation *sim, int cellX, int cellY, float newPressure) {
    sim->pressures[cellX + cellY * sim->sizeX] = newPressure;
}

int getVelocitiesCountH(const Simulation *sim) {
    return sim->cellCount + sim->sizeY;
}

int getVelocitiesCountV(const Simulation *sim) {
    return sim->cellCount + sim->sizeX;
}


Simulation createSimulation(const SimulationSettings *settings) {
    Simulation sim = {
        settings->gridWidth,
        settings->gridHeight,
        settings->gridWidth * settings->gridHeight,
        settings->cellWidth
    };

    sim.fluidDensity = settings->fluidDensity;
    
    sim.velocitiesH = calloc(getVelocitiesCountH(&sim), sizeof(float));
    sim.velocitiesV = calloc(getVelocitiesCountV(&sim), sizeof(float));
    sim.pressures = calloc((sim.cellCount), sizeof(float));

    sim.frameTimestep = settings->frameTimestep;
    sim.projectionRepeats = settings->projectionRepeats;

    return sim;
}

void deleteSimulation(Simulation *sim) {
    free(sim->velocitiesH);
    free(sim->velocitiesV);
    free(sim->pressures);
}



void updatePressures(Simulation *sim, float dt) {
    // Use the the general momentum form of the Navier-Stokes equation to compute new pressures
    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {
            if (!isBorderCell(sim, cellX, cellY)) {
                float velocityLeft = horizontalVelocityAt(sim, cellX, cellY);
                float velocityRight = horizontalVelocityAt(sim, cellX + 1, cellY);
                float velocityTop = horizontalVelocityAt(sim, cellX, cellY + 1);
                float velocityBottom = horizontalVelocityAt(sim, cellX, cellY);

                float averagePressure = 0;
                int pressureCount = 0;
                int offsets[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
                for (int i = 0; i < 4; i++) {
                    int offsetCellX = cellX + offsets[i][0];
                    int offsetCellY = cellY + offsets[i][1];
                    if (!isBorderCell(sim, offsetCellX, offsetCellY)) {
                        averagePressure += pressureAt(sim, offsetCellX, offsetCellY);
                        pressureCount++;
                    }
                }

                averagePressure /= pressureCount;

                float pressureLeft = horizontalVelocityAt(sim, cellX - 1, cellY);
                float pressureRight = horizontalVelocityAt(sim, cellX + 1, cellY);
                float pressureTop = horizontalVelocityAt(sim, cellX, cellY + 1);
                float pressureBottom = horizontalVelocityAt(sim, cellX, cellY - 1);
                
                float newCellPressure = averagePressure -(velocityRight - velocityLeft + velocityTop - velocityBottom) * sim->fluidDensity * sim->cellWidth / (4 * dt);
                updatePressureAt(sim, cellX, cellY, newCellPressure);
            }
        }
    }
}

void updateVelocities(Simulation *sim) {
    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {}
    }
}

void applyFluidProjection(Simulation *sim) {
    const float dt = sim->frameTimestep / sim->projectionRepeats;

    for (int i = 0; i < sim->projectionRepeats; i++) {
        updatePressures(sim, dt);
    }
}

void applyVelocityAdvection(Simulation *sim) {}


void updateSimulation(Simulation *sim) {
    applyFluidProjection(sim);
}