#pragma once

#include <stdbool.h>

typedef struct {
    const int sizeX;
    const int sizeY;
    const int cellCount;
    const float cellWidth;
    float fluidDensity;
    int velocityCountH;
    int velocityCountV;
    bool *solidCells;
    float *velocitiesH;
    float *velocitiesV;
    float *pressures;
    float *densityField;
    
    float frameTimestep;
    int projectionRepeats;
    float projectionSOR;

} Simulation;

typedef struct {
    int gridWidth;
    int gridHeight;
    float cellWidth;
    float fluidDensity;
    float frameTimestep;
    int projectionRepeats;
    float projectionSOR;
} SimulationSettings;

Simulation createSimulation(const SimulationSettings *settings);
void deleteSimulation(Simulation *sim);
void updateSimulation(Simulation *sim);
void resetSimulation(Simulation *sim);

bool isSolidCell(const Simulation *sim, int cellX, int cellY);
float horizontalVelocityAt(const Simulation *sim, int cellX, int cellY); // Returns velocity from the left
float verticalVelocityAt(const Simulation *sim, int cellX, int cellY); // Returns velocity from the bottom
float pressureAt(const Simulation *sim, int cellX, int cellY);
float particleDensityAt(const Simulation *sim, int cellX, int cellY);
float getInterpolatedParticleDensityAt(const Simulation *sim, float posX, float posY);
float divergenceAt(const Simulation *sim, int cellX, int cellY);
void updateHorizontalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity);
void updateVerticalVelocityAt(Simulation *sim, int cellX, int cellY, float newVelocity);
void updatePressureAt(Simulation *sim, int cellX, int cellY, float newPressure);
void updateParticleDensityAt(Simulation *sim, int cellX, int cellY, float newDensity);
void getInterpolatedVelocity(const Simulation *sim, float velocity[2], float posX, float posY);

void setSolidCell(Simulation *sim, int cellX, int cellY);
void addSolidCircle(Simulation *sim, int centerX, int centerY, int radius);
void addSolidRectangle(Simulation *sim, int posX, int posY, int width, int height);
void addSolidBorder(Simulation *sim, bool leftEdge, bool rightEdge, bool bottomEdge, bool topEdge);

void applyExternalWindForce(Simulation *sim, float force, bool leftEdge, bool rightEdge, bool bottomEdge, bool topEdge);
void applyExternalForce(Simulation *sim, int posX, int posY, int forceX, int forceY, int cellRadius);
void increaseParticleDensity(Simulation *sim, int posX, int posY, float densityIncrease, int cellRadius);
void applyFluidProjection(Simulation *sim);
void applyVelocityAdvection(Simulation *sim);
void applyParticleDensityAdvection(Simulation *sim);