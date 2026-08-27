#include <raylib.h>

#include "InputHandler.h"
#include "Renderer.h"

void getMouseGridPos(const Simulation *sim, int gridPos[2]) {
    screenToGridPos(sim, gridPos, GetMouseX(), GetMouseY());
}

void getMouseVelocity(float mouseVel[2]) {
    mouseVel[0] = GetMouseDelta().x;
    mouseVel[1] = GetMouseDelta().y;
}

bool isMouseDown() {
    return IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

bool resetButtonPressed() {
    return IsKeyPressed(KEY_R);
}

bool particleButtonDown() {
    return IsKeyDown(KEY_SPACE);
}