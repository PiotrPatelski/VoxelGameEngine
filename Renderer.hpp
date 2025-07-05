#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "FontManager.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Crosshair.hpp"
#include "World.hpp"
#include "Frustum.hpp"
#include "Material.hpp"
#include "StatusTextRenderer.hpp"
#include "Entity.hpp"

class Renderer {
   public:
    Renderer(unsigned int screenWidth, unsigned int screenHeight);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    void updateShaders(const Camera& camera);
    void render(unsigned int fps, World& world);

   private:
    void applyCubeShaderInitialConfig();
    void setupDirectionalLightConfig();
    void setupWaterTintConfig();
    void updateWaterShaderParams(const Camera& camera);
    void updateSpotlightShaderParams(const Camera& camera);
    void updateProjectionViewShaderParams(const Camera& camera);
    void renderOpaqueCubes(World& world);
    void renderWater(World& world);
    void renderCurrentWorldView(World& world);
    void renderCrosshair();

    float screenWidth{0};
    float screenHeight{0};

    std::unique_ptr<Shader> cubeShader{nullptr};
    std::unique_ptr<Shader> lightCubeShader{nullptr};
    std::unique_ptr<Shader> crosshairShader{nullptr};
    std::unique_ptr<Crosshair> crosshair{nullptr};
    std::unique_ptr<StatusTextRenderer> statusTextRenderer{nullptr};
    Materials materials{};
    Frustum frustum{};
    glm::vec3 lastCameraPosition{0.0f, 0.0f, 0.0f};
};