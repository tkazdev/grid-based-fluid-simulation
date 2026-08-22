#pragma once

#include <raylib.h>

#include "Simulation.h"

void initRenderer(int width, int height, int fps);
void deinitRenderer();
bool shouldEndSimulation();
void renderSimulation(Simulation *sim);