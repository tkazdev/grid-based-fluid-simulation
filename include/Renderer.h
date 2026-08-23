#pragma once

#include "Simulation.h"

void initRenderer(int width, int height, int fps);
void deinitRenderer();
bool shouldEndSimulation();
void startRender();
void endRender();

void renderGridLines(const Simulation *sim);
void renderVelocityArrows(const Simulation *sim);
void renderPressureLabels(const Simulation *sim);