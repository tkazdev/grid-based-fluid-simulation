#pragma once

#include <stdbool.h>

#include "Simulation.h"

void getMouseGridPos(const Simulation *sim, int gridPos[2]);
void getMouseVelocity(float mouseVel[2]);
bool isMouseDown();
bool resetButtonPressed();