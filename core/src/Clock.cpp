#include "Clock.hpp"
#include <GLFW/glfw3.h>

unsigned int Clock::calculateFps() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // FPS CALCULATION
    fpsTimer += deltaTime;
    frameCount++;

    if (fpsTimer >= fpsUpdateInterval) {
        currentFps = static_cast<unsigned int>(static_cast<float>(frameCount) /
                                               fpsTimer);
        frameCount = 0;
        fpsTimer = 0.0f;
    }
    return currentFps;
}