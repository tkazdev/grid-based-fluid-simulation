#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#include "Renderer.h"

const int cellRenderWidth = 80;
const int gridLineThickness = 2;
const int velocityArrowThickness = 3;
const float unitVelocityArrowLength = 10.0f;

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

void startRender() {
    BeginDrawing();
    ClearBackground((Color){13, 14, 36});
}
void endRender() {
    EndDrawing();
}

int getGridRenderWidth(const Simulation *sim) {
    return sim->sizeX * cellRenderWidth;
}

int getGridRenderHeight(const Simulation *sim) {
    return sim->sizeY * cellRenderWidth;
}

Vector2 getCellRenderPos(const Simulation *sim, int cellX, int cellY) { // Bottom left pos
    Vector2 drawPos = {cellX * cellRenderWidth, GetScreenHeight() - cellY * cellRenderWidth};

    // Ensure the grid is centered
    drawPos.x += (GetScreenWidth() - getGridRenderWidth(sim)) / 2;
    drawPos.y -= (GetScreenHeight() - getGridRenderHeight(sim)) / 2;

    return drawPos;
}

void drawArrow(Vector2 startPos, Vector2 endPos, int thickness, Color color) {
    const int arrowHeadSize = thickness * 2;
    DrawLineEx(startPos, endPos, thickness, color);
    Vector2 arrowDir = Vector2Subtract(endPos, startPos);
    if (startPos.x == endPos.x && startPos.y == endPos.y)
        DrawCircleV(endPos, thickness, color);
    else
        DrawPoly(endPos, 3, arrowHeadSize, 90 + atan2f(arrowDir.x, arrowDir.y) * -RAD2DEG, color);
}



void renderGridLines(const Simulation *sim) {
    const Vector2 gridTR = getCellRenderPos(sim, sim->sizeX, sim->sizeY);
    const Vector2 gridBL = getCellRenderPos(sim, 0, 0);

    // Draw vertical lines
    for (int x = 0; x < sim->sizeX + 1; x++) {
        int drawX = getCellRenderPos(sim, x, 0).x;
        DrawLineEx((Vector2){drawX, gridTR.y}, (Vector2){drawX, gridBL.y}, gridLineThickness, GRAY);
    }

    // Draw horizontal lines
    for (int y = 0; y < sim->sizeY + 1; y++) {
        int drawY = getCellRenderPos(sim, 0, y).y;
        DrawLineEx((Vector2){gridBL.x, drawY}, (Vector2){gridTR.x, drawY}, gridLineThickness, GRAY);
    }
}

void renderVelocityArrows(const Simulation *sim) {
    float *curVelH = sim->velocitiesH;
    float *curVelV = sim->velocitiesV;
    
    // Render horizontal arrows
    for (int i = 0; i < getVelocitiesCountH(sim); i++, curVelH++) {
        int cellX = i / sim->sizeY;
        int cellY = i % sim->sizeY;
        Vector2 startPos = getCellRenderPos(sim, cellX, cellY);
        startPos.y -= cellRenderWidth / 2;
        Vector2 endPos = startPos;
        endPos.x += *curVelH * unitVelocityArrowLength;
        drawArrow(startPos, endPos, velocityArrowThickness, WHITE);
    }

    // Render vertical arrows
    for (int i = 0; i < getVelocitiesCountV(sim); i++, curVelV++) {
        int cellX = i % sim->sizeX;
        int cellY = i / sim->sizeX;
        Vector2 startPos = getCellRenderPos(sim, cellX, cellY);
        startPos.x += cellRenderWidth / 2;
        Vector2 endPos = startPos;
        endPos.y += *curVelV * -unitVelocityArrowLength;
        drawArrow(startPos, endPos, velocityArrowThickness, WHITE);
    }
}

void renderPressureLabels(const Simulation *sim) {
    const int fontSize = 10;

    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {

            Vector2 drawPos = getCellRenderPos(sim, cellX, cellY);
            drawPos.x += 2;
            drawPos.y += 2 - cellRenderWidth;

            float f = pressureAt(sim, cellX, cellY);
            char buf[8];

            snprintf(buf, sizeof(buf), "%.2f", f);

            DrawTextEx(GetFontDefault(), buf, drawPos, fontSize, 1.0f, WHITE);
        }
    }
}