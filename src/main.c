#include "Simulation.h"
#include "Renderer.h"

const int targetFPS = 60;
const int screenWidth = 400;
const int screenHeight = 400;

int main(void) {
    initRenderer(screenWidth, screenHeight, targetFPS);
    Simulation sim = createSimulation();
    
    while (!shouldEndSimulation()) {
        updateSimulation(&sim);
        renderSimulation(&sim);
    }

    deinitRenderer();

    return 0;
}