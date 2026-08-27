#include "Simulation.h"
#include "Renderer.h"
#include "InputHandler.h"

int main(void) {
    RendererSettings rSettings;

    rSettings.targetFPS = 60;
    rSettings.screenWidth = 1400;
    rSettings.screenHeight = 800;
    rSettings.cellRenderWidth = 40;
    rSettings.gridLineThickness = 1;
    rSettings.velocityEdgeArrowThickness = 2;
    rSettings.unitVelocityEdgeArrowLength = 1.5f;
    rSettings.velocityFieldArrowThickness = 2;
    rSettings.unitVelocityFieldArrowLength = 0.3f;
    rSettings.lableFontSize = 10;
    
    
    SimulationSettings simSettings;
    simSettings.gridWidth = 32;
    simSettings.gridHeight = 16;
    simSettings.cellWidth = 1;
    simSettings.fluidDensity = 1;
    simSettings.frameTimestep = 1.f / rSettings.targetFPS;
    simSettings.projectionRepeats = 10;
    simSettings.projectionSOR = 1.8;

    initRenderer(&rSettings);
    Simulation sim = createSimulation(&simSettings);
    
    addSolidBorder(&sim, true, true, true, true);

    while (!shouldEndSimulation()) {
        // Handle inupts

        int mouseGridPos[2];
        float mouseVelocity[2];
        getMouseGridPos(&sim, mouseGridPos);
        getMouseVelocity(mouseVelocity);

        if (resetButtonPressed()) {
            resetSimulation(&sim);
        }

        if (isMouseDown()) {
            applyExternalForce(&sim, mouseGridPos[0], mouseGridPos[1], mouseVelocity[0] * 50.0f, mouseVelocity[1] * -50.0f, 2);
        }

        if (particleButtonDown()) {
            increaseParticleDensity(&sim, mouseGridPos[0], mouseGridPos[1], 0.15f, 2);
        }

        // Update
        updateSimulation(&sim);

        // Render
        startRender();

        renderSolidCells(&sim);
        // renderFluidSpeed(&sim, 1, 100.0f);
        renderFluidDensity(&sim);
        // renderGridLines(&sim);

        // renderVelocityField(&sim, 0.125f);
        renderVelocityArrows(&sim);

        // renderPressureLabels(&sim);
        // renderDivergenceLabels(&sim);

        endRender();
    }

    deleteSimulation(&sim);
    deinitRenderer();

    return 0;
}