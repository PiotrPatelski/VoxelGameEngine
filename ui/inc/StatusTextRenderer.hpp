#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "FontManager.hpp"

class StatusTextRenderer {
   public:
    StatusTextRenderer(float screenWidth, float screenHeight);
    ~StatusTextRenderer() = default;
    StatusTextRenderer(const StatusTextRenderer&) = delete;
    StatusTextRenderer(StatusTextRenderer&&) = delete;
    StatusTextRenderer& operator=(const StatusTextRenderer&) = delete;
    StatusTextRenderer& operator=(StatusTextRenderer&&) = delete;
    void renderStatus(unsigned int fps, const glm::vec3& cameraPos);

   private:
    void renderCameraPosition(const glm::vec3& cameraPos);
    void renderFpsCount(unsigned int fps);

    std::unique_ptr<FontManager> fontManager;

    float cameraTextStartX{};
    float cameraTextStartY{};
    float cameraTextLineSpacing{};
    float cameraTextScale{};

    float fpsTextPosX{};
    float fpsTextPosY{};
    float fpsTextScale{};
};