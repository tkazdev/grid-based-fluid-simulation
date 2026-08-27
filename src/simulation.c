#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Simulation.h"
#include "MathHelpers.h"

const int offsets[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

bool isInGrid(const Simulation *sim, int cellX, int cellY) {
    return (cellX >= 0 && cellX < sim->sizeX && cellY >= 0 && cellY < sim->sizeY);
}

bool isSolidCell(const Simulation *sim, int cellX, int cellY) {
    if (!isInGrid(sim, cellX, cellY)) return false;
    return sim->solidCells[cellX + cellY * sim->sizeX];
}

float horizontalVelocityAt(const Simulation *sim, int cellX, int cellY) {
    cellX = clampInt(cellX, 0, sim->sizeX);
    cellY = clampInt(cellY, 0, sim->sizeY - 1);
    return sim->velocitiesH[cellY + cellX * sim->sizeY];
}

float verticalVelocityAt(const Simulation *sim, int cellX, int cellY) {
    cellX = clampInt(cellX, 0, sim->sizeX - 1);
    cellY = clampInt(cellY, 0, sim->sizeY);
    return sim->velocitiesV[cellX + cellY * sim->sizeX];
}

float pressureAt(const Simulation *sim, int cellX, int cellY) {
    cellX = clampInt(cellX, 0, sim->sizeX - 1);
    cellY = clampInt(cellY, 0, sim->sizeY - 1);
    return sim->pressures[cellX + cellY * sim->sizeX];
}

float particleDensityAt(const Simulation *sim, int cellX, int cellY) {
    if (!isInGrid(sim, cellX, cellY)) return 0.0f;
    return sim->densityField[cellX + cellY * sim->sizeX];
}

float getInterpolatedParticleDensityAt(const Simulation *sim, float posX, float posY) {
    posX = clampFloat(posX, 0, sim->sizeX);
    posY = clampFloat(posY, 0, sim->sizeY);

    int rightEdge = roundf(posX);
    int leftEdge = maxInt(rightEdge - 1, 0);
    int topEdge = roundf(posY);
    int bottomEdge = maxInt(topEdge - 1, 0);

    return lerpQuad(
        particleDensityAt(sim, leftEdge, topEdge), particleDensityAt(sim, rightEdge, topEdge),
        particleDensityAt(sim, leftEdge, bottomEdge), particleDensityAt(sim, rightEdge, bottomEdge),
        posX - rightEdge + 0.5f, posY - topEdge + 0.5f
    );
}

float divergenceAt(const Simulation *sim, int cellX, int cellY) {
    return horizontalVelocityAt(sim, cellX + 1, cellY) - horizontalVelocityAt(sim, cellX, cellY)
        + verticalVelocityAt(sim, cellX, cellY + 1) - verticalVelocityAt(sim, cellX, cellY);
}

void updateHorizontalVelocityArrayAt(const Simulation *sim, float velocitiesH[], int cellX, int cellY, float newVelocity) {
    int idx = cellY + cellX * sim->sizeY;
    if (cellX < 0 || cellX > sim->sizeX || cellY < 0 || cellY > sim->sizeY - 1 || idx >= sim->velocityCountH) return;
    velocitiesH[idx] = newVelocity;
}

void updateVerticalVelocityArrayAt(const Simulation *sim, float velocitiesV[], int cellX, int cellY, float newVelocity) {
    int idx = cellX + cellY * sim->sizeX;
    if (cellX < 0 || cellX > sim->sizeX - 1 || cellY < 0 || cellY > sim->sizeY || idx >= sim->velocityCountV) return;
    velocitiesV[idx] = newVelocity;
}

void updateHorizontalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity) {
    updateHorizontalVelocityArrayAt(sim, sim->velocitiesH, cellX, cellY, newVelocity);
}

void updateVerticalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity) {
    updateVerticalVelocityArrayAt(sim, sim->velocitiesV, cellX, cellY, newVelocity);
}

void updatePressureAt(Simulation *sim, int cellX, int cellY, float newPressure) {
    if (!isInGrid(sim, cellX, cellY)) return;
    sim->pressures[cellX + cellY * sim->sizeX] = newPressure;
}

void updateParticleDensityArrayAt(Simulation *sim, float densityField[], int cellX, int cellY, float newDensity) {
    if (!isInGrid(sim, cellX, cellY)) return;
    densityField[cellX + cellY * sim->sizeX] = newDensity;
}

void updateParticleDensityAt(Simulation *sim, int cellX, int cellY, float newDensity) {
    updateParticleDensityArrayAt(sim, sim->densityField, cellX, cellY, newDensity);
}

float getHorizontalInterpolatedVelocity(const Simulation *sim, float posX, float posY) {
    posX = clampFloat(posX, 0, sim->sizeX);
    posY = clampFloat(posY, 0, sim->sizeY);

    int leftX = floorf(posX);
    int rightX = leftX + 1;
    int topY = roundf(posY);
    int bottomY = topY - 1;
    
    return lerpQuad(
        horizontalVelocityAt(sim, leftX, topY), horizontalVelocityAt(sim, rightX, topY),
        horizontalVelocityAt(sim, leftX, bottomY), horizontalVelocityAt(sim, rightX, bottomY),
        posX - leftX, posY - topY + 0.5f
    );
}

float getVerticalInterpolatedVelocity(const Simulation *sim, float posX, float posY) {
    posX = clampFloat(posX, 0, sim->sizeX);
    posY = clampFloat(posY, 0, sim->sizeY);

    float leftX = roundf(posX) - 1;
    float rightX = leftX + 1;
    float bottomY = floorf(posY);
    float topY = bottomY + 1;

    return lerpQuad(
        verticalVelocityAt(sim, leftX, topY), verticalVelocityAt(sim, rightX, topY),
        verticalVelocityAt(sim, leftX, bottomY), verticalVelocityAt(sim, rightX, bottomY),
        posX - rightX + 0.5f, posY - bottomY
    );
}

void getInterpolatedVelocity(const Simulation *sim, float velocity[2], float posX, float posY) {
    velocity[0] = getHorizontalInterpolatedVelocity(sim, posX, posY);
    velocity[1] = getVerticalInterpolatedVelocity(sim, posX, posY);
}

void setSolidCell(Simulation *sim, int cellX, int cellY) {
    if (!isInGrid(sim, cellX, cellY)) return;
    sim->solidCells[cellX + cellY * sim->sizeX] = true;
}

void addSolidCircle(Simulation *sim, int centerX, int centerY, int radius) {
    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {
            if (squareInt(centerX - cellX) + squareInt(centerY - cellY) < squareInt(radius)) {
                setSolidCell(sim, cellX, cellY);
            }
        }
    }
}

void addSolidRectangle(Simulation *sim, int posX, int posY, int width, int height) {
    for (int cellY = posY; cellY < posY + height; cellY++) {
        for (int cellX = posX; cellX < posX + width; cellX++) {
            setSolidCell(sim, cellX, cellY);
        }
    }
}

void addSolidBorder(Simulation *sim, bool leftEdge, bool rightEdge, bool bottomEdge, bool topEdge) {
    if (leftEdge) {
        for (int posY = 0; posY < sim->sizeY; posY++) setSolidCell(sim, 0, posY);
    }

    if (rightEdge) {
        for (int posY = 0; posY < sim->sizeY; posY++) setSolidCell(sim, sim->sizeX - 1, posY);
    }

    if (bottomEdge) {
        for (int posX = 0; posX < sim->sizeX; posX++) setSolidCell(sim, posX, 0);
    }

    if (topEdge) {
        for (int posX = 0; posX < sim->sizeX; posX++) setSolidCell(sim, posX, sim->sizeY - 1);
    }
}

void applyExternalWindForce(Simulation *sim, float force, bool leftEdge, bool rightEdge, bool bottomEdge, bool topEdge) {
    if (leftEdge) {
        for (int cellY = 0; cellY < sim->sizeY; cellY++) {
            float curVel = horizontalVelocityAt(sim, 1, cellY);
            updateHorizontalVelocityAt(sim, 1, cellY, curVel + force);
        }
    }
    
    if (rightEdge) {
        for (int cellY = 0; cellY < sim->sizeY; cellY++) {
            float curVel = horizontalVelocityAt(sim, sim->sizeX - 1, cellY);
            updateHorizontalVelocityAt(sim, sim->sizeX - 1, cellY, curVel - force);
        }
    }

    if (bottomEdge) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {
            float curVel = horizontalVelocityAt(sim, cellX, 1);
            updateVerticalVelocityAt(sim, cellX, 1, curVel + force);
        }
    }

    if (topEdge) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {
            float curVel = horizontalVelocityAt(sim, cellX, sim->sizeY - 1);
            updateVerticalVelocityAt(sim, cellX, sim->sizeY - 1, curVel - force);
        }
    }

}

void applyExternalForce(Simulation *sim, int posX, int posY, int forceX, int forceY, int cellRadius) {
    // Find the square that all the updated cells will fall into
    int minX = maxInt(posX - cellRadius, 0);
    int maxX = minInt(posX + cellRadius + 1, sim->sizeX);
    int minY = maxInt(posY - cellRadius, 0);
    int maxY = minInt(posY + cellRadius + 1, sim->sizeY);

    for (int cellY = minY; cellY < maxY; cellY++) {
        for (int cellX = minX; cellX < maxX; cellX++) {
            if (!isSolidCell(sim, cellX, cellY) && squareInt(posX - cellX) + squareInt(posY - cellY) < squareInt(cellRadius)) {
                float velH = horizontalVelocityAt(sim, cellX, cellY);
                float velV = verticalVelocityAt(sim, cellX, cellY);
                const float c = sim->frameTimestep / sim->fluidDensity;
                updateHorizontalVelocityAt(sim, cellX, cellY, velH + forceX * c);
                updateVerticalVelocityAt(sim, cellX, cellY, velV + forceY * c);
            }
        }
    }
}

void increaseParticleDensity(Simulation *sim, int posX, int posY, float densityIncrease, int cellRadius) {
    int minX = maxInt(posX - cellRadius, 0);
    int maxX = minInt(posX + cellRadius + 1, sim->sizeX);
    int minY = maxInt(posY - cellRadius, 0);
    int maxY = minInt(posY + cellRadius + 1, sim->sizeY);

    for (int cellY = minY; cellY < maxY; cellY++) {
        for (int cellX = minX; cellX < maxX; cellX++) {
            if (!isSolidCell(sim, cellX, cellY) && squareInt(posX - cellX) + squareInt(posY - cellY) < squareInt(cellRadius)) {
                float curDensity = particleDensityAt(sim, cellX, cellY);
                updateParticleDensityAt(sim,cellX, cellY, curDensity + densityIncrease);
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

    sim.velocityCountH = sim.cellCount + sim.sizeY;
    sim.velocityCountV = sim.cellCount + sim.sizeX;
    
    sim.velocitiesH = calloc(sim.velocityCountH, sizeof(float));
    sim.velocitiesV = calloc(sim.velocityCountV, sizeof(float));
    sim.pressures = calloc((sim.cellCount), sizeof(float));
    sim.densityField = calloc((sim.cellCount), sizeof(float));
    sim.solidCells = calloc((sim.cellCount), sizeof(bool));

    sim.frameTimestep = settings->frameTimestep;
    sim.projectionRepeats = settings->projectionRepeats;
    sim.projectionSOR = settings->projectionSOR;

    return sim;
}

void deleteSimulation(Simulation *sim) {
    free(sim->velocitiesH);
    free(sim->velocitiesV);
    free(sim->pressures);
    free(sim->densityField);
    free(sim->solidCells);
}

void resetSimulation(Simulation *sim) {
    memset(sim->velocitiesH, 0.0f, sim->velocityCountH * sizeof(float));
    memset(sim->velocitiesV, 0.0f, sim->velocityCountV * sizeof(float));
    memset(sim->pressures, 0.0f, sim->cellCount * sizeof(float));
    memset(sim->densityField, 0.0f, sim->cellCount * sizeof(float));
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
                    float curCellPressure = pressureAt(sim, cellX, cellY);
                    curCellPressure += (newCellPressure - curCellPressure) * sim->projectionSOR;
                    updatePressureAt(sim, cellX, cellY, curCellPressure);
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
                float velocityLeft = horizontalVelocityAt(sim, cellX, cellY);
                float velocityBottom = verticalVelocityAt(sim, cellX, cellY);

                // Horizontal
                if (!isSolidCell(sim, cellX - 1, cellY)) {
                    float leftCellPressure = pressureAt(sim, cellX - 1, cellY);
                    velocityLeft += (leftCellPressure - curCellPressure) * gradientConstants;
                    updateHorizontalVelocityAt(sim, cellX, cellY, velocityLeft);
                }

                // Vertical
                if (!isSolidCell(sim, cellX, cellY - 1)) {
                    float bottomCellPressure = pressureAt(sim, cellX, cellY - 1);
                    velocityBottom += (bottomCellPressure - curCellPressure) * gradientConstants;
                    updateVerticalVelocityAt(sim, cellX, cellY, velocityBottom);
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

void applyVelocityAdvection(Simulation *sim) {
    // Semi-Lagrangian advection

    float *newVelocitiesH = malloc(sim->velocityCountH * sizeof(float));
    float *newVelocitiesV = malloc(sim->velocityCountV * sizeof(float));

    for (int cellY = 0; cellY < sim->sizeY + 1; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX + 1; cellX++) {
            float leftFaceVel[2];
            float bottomFaceVel[2];
            float posX = cellX;
            float posY = cellY;

            getInterpolatedVelocity(sim, leftFaceVel, posX, posY + 0.5f);
            getInterpolatedVelocity(sim, bottomFaceVel, posX + 0.5f, posY);

            float newLeftFaceVel[2];
            float newBottomFaceVel[2];
            getInterpolatedVelocity(sim, newLeftFaceVel, posX - leftFaceVel[0] * sim->frameTimestep, posY - leftFaceVel[1] * sim->frameTimestep + 0.5f);
            getInterpolatedVelocity(sim, newBottomFaceVel, posX - bottomFaceVel[0] * sim->frameTimestep + 0.5f, posY - bottomFaceVel[1] * sim->frameTimestep);

            updateHorizontalVelocityArrayAt(sim, newVelocitiesH, cellX, cellY, newLeftFaceVel[0]);
            updateVerticalVelocityArrayAt(sim, newVelocitiesV, cellX, cellY, newBottomFaceVel[1]);
        }
    }

    memcpy(sim->velocitiesH, newVelocitiesH, sim->velocityCountH * sizeof(float));
    memcpy(sim->velocitiesV, newVelocitiesV, sim->velocityCountV * sizeof(float));
    free(newVelocitiesH);
    free(newVelocitiesV);
}

void applyParticleDensityAdvection(Simulation *sim) {
    float *newDensityField = malloc(sim->cellCount * sizeof(float));

    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {
            float velocity[2];
            getInterpolatedVelocity(sim, velocity, cellX + 0.5f, cellY + 0.5f);
            float newDensity = getInterpolatedParticleDensityAt(sim, cellX + 0.5f - velocity[0] * sim->frameTimestep, cellY + 0.5f - velocity[1] * sim->frameTimestep);
            updateParticleDensityArrayAt(sim, newDensityField, cellX, cellY, newDensity);
        }
    }

    memcpy(sim->densityField, newDensityField, sim->cellCount * sizeof(float));
    free(newDensityField);
}


void updateSimulation(Simulation *sim) {
    applyFluidProjection(sim);
    applyVelocityAdvection(sim);
    applyParticleDensityAdvection(sim);
}