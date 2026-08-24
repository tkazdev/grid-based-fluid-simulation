#pragma once

#include <stdbool.h>

#include "Simulation.h"

void initRenderer(int width, int height, int fps);
void deinitRenderer();
bool shouldEndSimulation();
void startRender();
void endRender();

bool screenPosOnGrid(const Simulation *sim, int posX, int posY);
void screenToGridPos(const Simulation *sim, int gridPos[2], int posX, int posY);

void renderGridLines(const Simulation *sim);
void renderVelocityArrows(const Simulation *sim);
void renderPressureLabels(const Simulation *sim);
void renderDivergenceLabels(const Simulation *sim);