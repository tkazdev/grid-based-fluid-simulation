#include "Renderer.h"

void initRenderer(int width, int height, int fps) {
    InitWindow(width, height, "Fluid Simulation");
    SetTargetFPS(fps);
}

void deinitRenderer() {
    CloseWindow();
}

bool shouldEndSimulation() {
    return WindowShouldClose();
}

void renderSimulation(Simulation *sim) {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    EndDrawing();
}