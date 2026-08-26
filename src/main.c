#include "Simulation.h"
#include "Renderer.h"
#include "InputHandler.h"

int main(void) {
    RendererSettings rSettings;
    rSettings.screenWidth = 800;
    rSettings.screenHeight = 800;

    rSettings.targetFPS = 60;
    rSettings.screenWidth = 800;
    rSettings.screenHeight = 800;
    rSettings.cellRenderWidth = 20;
    rSettings.gridLineThickness = 1;
    rSettings.velocityEdgeArrowThickness = 2;
    rSettings.unitVelocityEdgeArrowLength = 1.5f;
    rSettings.velocityFieldArrowThickness = 2;
    rSettings.unitVelocityFieldArrowLength = 0.08f;
    rSettings.lableFontSize = 10;
    
    
    SimulationSettings simSettings;
    simSettings.gridWidth = 32;
    simSettings.gridHeight = 32;
    simSettings.cellWidth = 1;
    simSettings.fluidDensity = 1;
    simSettings.frameTimestep = 1.f / rSettings.targetFPS;
    simSettings.projectionRepeats = 20;
    simSettings.projectionSOR = 1.5;

    initRenderer(&rSettings);
    Simulation sim = createSimulation(&simSettings);
    
    while (!shouldEndSimulation()) {
        // Handle inupts

        if (resetButtonPressed()) {
            resetSimulation(&sim);
        }

        if (isMouseDown()) {
            int mouseGridPos[2];
            float mouseVelocity[2];
            getMouseGridPos(&sim, mouseGridPos);
            getMouseVelocity(mouseVelocity);
            applyExternalForce(&sim, mouseGridPos[0], mouseGridPos[1], mouseVelocity[0] * 50.0f, mouseVelocity[1] * -50.0f, 5);
        }

        // Update
        updateSimulation(&sim);

        // Render
        startRender();

        renderSolidCells(&sim);
        renderFluidSpeed(&sim, 1, 100.0f);
        renderGridLines(&sim);

        renderVelocityField(&sim, 0.5f);
        // renderVelocityArrows(&sim);

        // renderPressureLabels(&sim);
        // renderDivergenceLabels(&sim);

        endRender();
    }

    deleteSimulation(&sim);
    deinitRenderer();

    return 0;
}