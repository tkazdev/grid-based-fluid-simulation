#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Simulation.h"
#include "MathHelpers.h"

const int offsets[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

bool isSolidCell(const Simulation *sim, int cellX, int cellY) {
    return cellX == 0 || cellX == sim->sizeX - 1 || cellY == 0 || cellY == sim->sizeY - 1;
}

int getVelocitiesCountH(const Simulation *sim) {
    return sim->cellCount + sim->sizeY;
}

int getVelocitiesCountV(const Simulation *sim) {
    return sim->cellCount + sim->sizeX;
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

float divergenceAt(const Simulation *sim, int cellX, int cellY) {
    return horizontalVelocityAt(sim, cellX + 1, cellY) - horizontalVelocityAt(sim, cellX, cellY)
        + verticalVelocityAt(sim, cellX, cellY + 1) - verticalVelocityAt(sim, cellX, cellY);
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

void applyExternalForce(Simulation *sim, int posX, int posY, int forceX, int forceY, int cellRadius) {
    // Find the square that all the updated cells will fall into
    int minX = maxInt(posX - cellRadius, 0);
    int maxX = minInt(posX + cellRadius + 1, sim->sizeX);
    int minY = maxInt(posY - cellRadius, 0);
    int maxY = minInt(posY + cellRadius + 1, sim->sizeY);

    for (int cellY = minY; cellY < maxY; cellY++) {
        for (int cellX = minX; cellX < maxX; cellX++) {
            if (!isSolidCell(sim, cellX, cellY) && squareInt(posX - cellX) + squareInt(posY - cellY) <= squareInt(cellRadius)) {
                float velH = horizontalVelocityAt(sim, cellX, cellY);
                float velV = verticalVelocityAt(sim, cellX, cellY);
                const float c = sim->frameTimestep / sim->fluidDensity;
                updateHorizontalVelocityAt(sim, cellX, cellY, velH + forceX * c);
                updateVerticalVelocityAt(sim, cellX, cellY, velV + forceY * c);
            }
        }
    }
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
            if (!isSolidCell(sim, cellX, cellY)) {
                float velocityLeft = horizontalVelocityAt(sim, cellX, cellY);
                float velocityRight = horizontalVelocityAt(sim, cellX + 1, cellY);
                float velocityTop = verticalVelocityAt(sim, cellX, cellY + 1);
                float velocityBottom = verticalVelocityAt(sim, cellX, cellY);

                float averagePressure = 0;
                int fluidEdgeCount = 0;
                for (int i = 0; i < 4; i++) {
                    int offsetCellX = cellX + offsets[i][0];
                    int offsetCellY = cellY + offsets[i][1];
                    if (!isSolidCell(sim, offsetCellX, offsetCellY)) {
                        averagePressure += pressureAt(sim, offsetCellX, offsetCellY);
                        fluidEdgeCount++;
                    }
                }

                if (fluidEdgeCount > 0) {
                    averagePressure /= fluidEdgeCount;
                    float velocitySum = velocityRight - velocityLeft + velocityTop - velocityBottom;
                    float newCellPressure = averagePressure -velocitySum * sim->fluidDensity * sim->cellWidth / (fluidEdgeCount * dt);
                    updatePressureAt(sim, cellX, cellY, newCellPressure);
                }
            }
        }
    }
}

void updateVelocities(Simulation *sim, float dt) {
    const float gradientConstants = dt / (sim->fluidDensity * sim->cellWidth);

    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {
            if (isSolidCell(sim, cellX, cellY)) {
                updateHorizontalVelocityAt(sim, cellX, cellY, 0);
                updateHorizontalVelocityAt(sim, cellX + 1, cellY, 0);
                updateVerticalVelocityAt(sim, cellX, cellY, 0);
                updateVerticalVelocityAt(sim, cellX, cellY + 1, 0);
            } else {
                float curCellPressure = pressureAt(sim, cellX, cellY);
                float velocityRight = horizontalVelocityAt(sim, cellX + 1, cellY);
                float velocityTop = verticalVelocityAt(sim, cellX, cellY + 1);

                // Horizontal
                if (!isSolidCell(sim, cellX + 1, cellY)) {
                    float rightCellPressure = pressureAt(sim, cellX + 1, cellY);
                    velocityRight += -(rightCellPressure - curCellPressure) * gradientConstants;
                    updateHorizontalVelocityAt(sim, cellX + 1, cellY, velocityRight);
                }

                // Vertical
                if (!isSolidCell(sim, cellX, cellY + 1)) {
                    float topCellPressure = pressureAt(sim, cellX, cellY + 1);
                    velocityTop += -(topCellPressure - curCellPressure) * gradientConstants;
                    updateVerticalVelocityAt(sim, cellX, cellY + 1, velocityTop);
                }
            }
        }
    }
}

void applyFluidProjection(Simulation *sim) {
    const float dt = sim->frameTimestep / sim->projectionRepeats;

    for (int i = 0; i < sim->projectionRepeats; i++) {
        updatePressures(sim, dt);
    }
    updateVelocities(sim, dt);
}

void applyVelocityAdvection(Simulation *sim) {}


void updateSimulation(Simulation *sim) {
    applyFluidProjection(sim);
}