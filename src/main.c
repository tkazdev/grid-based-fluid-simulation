#include "Simulation.h"
#include "Renderer.h"
#include "InputHandler.h"

const int targetFPS = 60;
const int screenWidth = 800;
const int screenHeight = 800;

int main(void) {
    initRenderer(screenWidth, screenHeight, targetFPS);
    
    SimulationSettings simSettings;
    simSettings.gridWidth = 32;
    simSettings.gridHeight = 32;
    simSettings.cellWidth = 1;
    simSettings.fluidDensity = 1;
    simSettings.frameTimestep = 1.f / targetFPS;
    simSettings.projectionRepeats = 32;

    Simulation sim = createSimulation(&simSettings);

    updateHorizontalVelocityAt(&sim, 2, 3, 10.5f);
    updateVerticalVelocityAt(&sim, 2, 2, 1.0f);
    
    while (!shouldEndSimulation()) {
        // Handle inupts
        if (isMouseDown()) {
            int mouseGridPos[2];
            float mouseVelocity[2];
            getMouseGridPos(&sim, mouseGridPos);
            getMouseVelocity(mouseVelocity);
            applyExternalForce(&sim, mouseGridPos[0], mouseGridPos[1], mouseVelocity[0] * 10.0f, mouseVelocity[1] * 10.0f, 3);
        }

        // Update
        updateSimulation(&sim);

        // Render
        startRender();
        renderGridLines(&sim);
        // renderPressureLabels(&sim);
        // renderDivergenceLabels(&sim);
        renderVelocityArrows(&sim);
        endRender();
    }

    deleteSimulation(&sim);
    deinitRenderer();

    return 0;
}