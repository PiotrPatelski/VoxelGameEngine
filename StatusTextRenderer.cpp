#include "StatusTextRenderer.hpp"
#include <glm/vec3.hpp>
#include <string>

StatusTextRenderer::StatusTextRenderer(float screenWidth, float screenHeight)
    : cameraTextStartX(25.0f),
      cameraTextStartY(230.0f),
      cameraTextLineSpacing(40.0f),
      cameraTextScale(0.7f),
      fpsTextPosX(25.0f),
      fpsTextPosY(25.0f),
      fpsTextScale(1.0f) {
    fontManager = std::make_unique<FontManager>(screenWidth, screenHeight);
}

void StatusTextRenderer::renderCameraPosition(const glm::vec3& cameraPos) {
    int camX = static_cast<int>(cameraPos.x);
    int camY = static_cast<int>(cameraPos.y);
    int camZ = static_cast<int>(cameraPos.z);

    glm::vec3 textColor(1.0f, 1.0f, 1.0f);
    fontManager->renderText("Position", cameraTextStartX, cameraTextStartY,
                            cameraTextScale, textColor);

    float currentY = cameraTextStartY - cameraTextLineSpacing;
    fontManager->renderText("X = " + std::to_string(camX), cameraTextStartX,
                            currentY, cameraTextScale, textColor);

    currentY -= cameraTextLineSpacing;
    fontManager->renderText("Y = " + std::to_string(camY), cameraTextStartX,
                            currentY, cameraTextScale, textColor);

    currentY -= cameraTextLineSpacing;
    fontManager->renderText("Z = " + std::to_string(camZ), cameraTextStartX,
                            currentY, cameraTextScale, textColor);
}

void StatusTextRenderer::renderFpsCount(unsigned int fps) {
    std::string fpsText = "FPS count: " + std::to_string(fps);
    glm::vec3 fpsColor(0.5f, 0.8f, 0.2f);
    fontManager->renderText(fpsText, fpsTextPosX, fpsTextPosY, fpsTextScale,
                            fpsColor);

    fontManager->renderText("Pioter Craft Project", 1640.0f, 1010.0f, 0.5f,
                            glm::vec3(0.3f, 0.7f, 0.9f));
}

void StatusTextRenderer::renderStatus(unsigned int fps,
                                      const glm::vec3& cameraPos) {
    renderCameraPosition(cameraPos);
    renderFpsCount(fps);
}
