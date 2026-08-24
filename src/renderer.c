#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "Renderer.h"

static RendererSettings rSettings;

void initRenderer(const RendererSettings *renderSettings) {
    rSettings = *renderSettings;
    InitWindow(rSettings.screenWidth, rSettings.screenHeight, "Fluid Simulation");
    SetTargetFPS(rSettings.targetFPS);
}

void deinitRenderer() {
    CloseWindow();
}

bool shouldEndSimulation() {
    return WindowShouldClose();
}

void startRender() {
    BeginDrawing();
    ClearBackground((Color){17, 18, 46, 255});
}
void endRender() {
    EndDrawing();
}

int getGridRenderWidth(const Simulation *sim) {
    return sim->sizeX * rSettings.cellRenderWidth;
}

int getGridRenderHeight(const Simulation *sim) {
    return sim->sizeY * rSettings.cellRenderWidth;
}

Vector2 getCellRenderPos(const Simulation *sim, int cellX, int cellY) { // Bottom left pos
    Vector2 drawPos = {cellX * rSettings.cellRenderWidth, GetScreenHeight() - cellY * rSettings.cellRenderWidth};

    // Ensure the grid is centered
    drawPos.x += (GetScreenWidth() - getGridRenderWidth(sim)) / 2;
    drawPos.y -= (GetScreenHeight() - getGridRenderHeight(sim)) / 2;

    return drawPos;
}

bool screenPosOnGrid(const Simulation *sim, int posX, int posY) {
    Vector2 gridBL = getCellRenderPos(sim, 0, 0);
    return posX >= gridBL.x && posX < gridBL.x + getGridRenderWidth(sim) && posY <= gridBL.y && posY > gridBL.y - getGridRenderHeight(sim);
}

void screenToGridPos(const Simulation *sim, int gridPos[2], int posX, int posY) {
    Vector2 gridBL = getCellRenderPos(sim, 0, 0);
    gridPos[0] = floorf(Normalize(posX, gridBL.x, gridBL.x + getGridRenderWidth(sim)) * sim->sizeX);
    gridPos[1] = floorf(Normalize(posY, gridBL.y, gridBL.y - getGridRenderHeight(sim)) * sim->sizeY);
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



void renderSolidCells(const Simulation *sim) {
    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {
            if (isSolidCell(sim, cellX, cellY)) {
                Vector2 drawPos = getCellRenderPos(sim, cellX, cellY);
                DrawRectangle(drawPos.x, drawPos.y - rSettings.cellRenderWidth, rSettings.cellRenderWidth, rSettings.cellRenderWidth, (Color){13, 14, 36, 255});
            }
        }
    }
}

void renderGridLines(const Simulation *sim) {
    const Vector2 gridTR = getCellRenderPos(sim, sim->sizeX, sim->sizeY);
    const Vector2 gridBL = getCellRenderPos(sim, 0, 0);

    // Draw vertical lines
    for (int x = 0; x < sim->sizeX + 1; x++) {
        int drawX = getCellRenderPos(sim, x, 0).x;
        DrawLineEx((Vector2){drawX, gridTR.y}, (Vector2){drawX, gridBL.y}, rSettings.gridLineThickness, GRAY);
    }

    // Draw horizontal lines
    for (int y = 0; y < sim->sizeY + 1; y++) {
        int drawY = getCellRenderPos(sim, 0, y).y;
        DrawLineEx((Vector2){gridBL.x, drawY}, (Vector2){gridTR.x, drawY}, rSettings.gridLineThickness, GRAY);
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
        startPos.y -= rSettings.cellRenderWidth / 2;
        Vector2 endPos = startPos;
        endPos.x += *curVelH * rSettings.unitVelocityEdgeArrowLength;
        drawArrow(startPos, endPos, rSettings.velocityEdgeArrowThickness, WHITE);
    }

    // Render vertical arrows
    for (int i = 0; i < getVelocitiesCountV(sim); i++, curVelV++) {
        int cellX = i % sim->sizeX;
        int cellY = i / sim->sizeX;
        Vector2 startPos = getCellRenderPos(sim, cellX, cellY);
        startPos.x += rSettings.cellRenderWidth / 2;
        Vector2 endPos = startPos;
        endPos.y += *curVelV * -rSettings.unitVelocityEdgeArrowLength;
        drawArrow(startPos, endPos, rSettings.velocityEdgeArrowThickness, WHITE);
    }
}

void renderCellLabel(const Simulation *sim, float (*labelFunc)(const Simulation*, int, int), int labelRow) {
    for (int cellY = 0; cellY < sim->sizeY; cellY++) {
        for (int cellX = 0; cellX < sim->sizeX; cellX++) {

            Vector2 drawPos = getCellRenderPos(sim, cellX, cellY);
            drawPos.x += 2;
            drawPos.y += 2 - rSettings.cellRenderWidth + labelRow * rSettings.lableFontSize;

            char buf[8];
            float f = labelFunc(sim, cellX, cellY);
            snprintf(buf, sizeof(buf), "%.2f", f);

            DrawTextEx(GetFontDefault(), buf, drawPos, rSettings.lableFontSize, 1.0f, WHITE);
        }
    }
}

void renderPressureLabels(const Simulation *sim) {
    renderCellLabel(sim, pressureAt, 0);
}

void renderDivergenceLabels(const Simulation *sim) {
    renderCellLabel(sim, divergenceAt, 1);
}