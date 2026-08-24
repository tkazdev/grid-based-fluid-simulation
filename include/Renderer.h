#pragma once

#include <stdbool.h>

#include "Simulation.h"

typedef struct {
    int targetFPS;
    int screenWidth;
    int screenHeight;

    int cellRenderWidth;
    int gridLineThickness;
    int velocityEdgeArrowThickness;
    float unitVelocityEdgeArrowLength;
    int velocityFieldArrowThickness;
    float unitVelocityFieldArrowLength;
    int lableFontSize;
} RendererSettings;

void initRenderer(const RendererSettings *renderSettings);
void deinitRenderer();
bool shouldEndSimulation();
void startRender();
void endRender();

bool screenPosOnGrid(const Simulation *sim, int posX, int posY);
void screenToGridPos(const Simulation *sim, int gridPos[2], int posX, int posY);

void renderSolidCells(const Simulation *sim);
void renderGridLines(const Simulation *sim);
void renderVelocityArrows(const Simulation *sim);
void renderVelocityField(const Simulation *sim, int lineDensity);
void renderPressureLabels(const Simulation *sim);
void renderDivergenceLabels(const Simulation *sim);