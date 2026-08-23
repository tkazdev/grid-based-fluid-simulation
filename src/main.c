#include "Simulation.h"
#include "Renderer.h"

const int targetFPS = 60;
const int screenWidth = 800;
const int screenHeight = 600;

int main(void) {
    initRenderer(screenWidth, screenHeight, targetFPS);
    
    SimulationSettings simSettings;
    simSettings.gridWidth = 6;
    simSettings.gridHeight = 5;
    simSettings.cellWidth = 1;
    simSettings.fluidDensity = 1;
    simSettings.frameTimestep = 1.f / targetFPS;
    simSettings.projectionRepeats = 1;

    Simulation sim = createSimulation(&simSettings);

    updateHorizontalVelocityAt(&sim, 2, 3, 1.5f);
    updateVerticalVelocityAt(&sim, 2, 2, 1.0f);
    
    while (!shouldEndSimulation()) {
        updateSimulation(&sim);

        // Render
        startRender();
        renderGridLines(&sim);
        renderPressureLabels(&sim);
        renderVelocityArrows(&sim);
        endRender();
    }

    deleteSimulation(&sim);
    deinitRenderer();

    return 0;
}