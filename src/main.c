#include "Simulation.h"
#include "Renderer.h"
#include "InputHandler.h"

const float speedScale = 3.0f;

int main(void) {
    RendererSettings rSettings;

    rSettings.targetFPS = 60;
    rSettings.screenWidth = 1400;
    rSettings.screenHeight = 800;
    rSettings.cellRenderWidth = 2;
    rSettings.gridLineThickness = 1;
    rSettings.velocityEdgeArrowThickness = 2;
    rSettings.unitVelocityEdgeArrowLength = 1.5f;
    rSettings.velocityFieldArrowThickness = 2;
    rSettings.unitVelocityFieldArrowLength = 0.3f;
    rSettings.lableFontSize = 10;
    rSettings.uiFontSize = 20;
    
    
    SimulationSettings simSettings;
    simSettings.gridWidth = 320;
    simSettings.gridHeight = 180;
    simSettings.cellWidth = 1;
    simSettings.fluidDensity = 1;
    simSettings.frameTimestep = 1.f / rSettings.targetFPS * speedScale;
    simSettings.projectionRepeats = 8 * speedScale;
    simSettings.projectionSOR = 1.8;

    initRenderer(&rSettings);
    Simulation sim = createSimulation(&simSettings);
    
    addSolidBorder(&sim, true, false, true, true);
    addSolidCircle(&sim, 50, simSettings.gridHeight / 2, simSettings.gridHeight / 8);

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
            increaseParticleDensity(&sim, mouseGridPos[0], mouseGridPos[1], 0.15f, 5);
        }

        // Update
        applyExternalWindForce(&sim, 20.0f, true, false, false, false);
        increaseParticleDensity(&sim, 2, simSettings.gridHeight / 2, 0.15f, 7);

        updateSimulation(&sim);

        // Render
        startRender();

        // renderFluidSpeed(&sim, 1, 100.0f);
        // renderFluidPressure(&sim, 2000);
        renderFluidDensity(&sim);
        // renderGridLines(&sim);
        renderSolidCells(&sim);

        // renderVelocityField(&sim, 0.2f);
        // renderVelocityArrows(&sim);

        // renderPressureLabels(&sim);
        // renderDivergenceLabels(&sim);

        renderFPS(10, 10);

        endRender();
    }

    deleteSimulation(&sim);
    deinitRenderer();

    return 0;
}