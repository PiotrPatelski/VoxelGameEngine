#include "Renderer.hpp"
#include "Entity.hpp"
#include "WaterSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "TextureManager.hpp"

namespace {
constexpr float WATER_ANIMATION_SPEED{0.05f};
constexpr glm::vec3 DIRECTIONAL_LIGHT_DIR{-0.2f, -1.0f, -0.3f};
constexpr glm::vec3 LIGHT_AMBIENT{0.2f, 0.2f, 0.2f};
constexpr glm::vec3 LIGHT_DIFFUSE{0.5f, 0.5f, 0.5f};
constexpr glm::vec3 UNDERWATER_TINT{0.0f, 0.3f, 0.5f};
constexpr float UNDERWATER_MIX_FACTOR{0.5f};
constexpr float FADE_VALUE{0.2f};
constexpr int LIGHT_VOLUME_TEXTURE_UNIT{15};
} // namespace

void Renderer::setupDirectionalLightConfig() {
    cubeShader->setVec3("directionalLight.direction", DIRECTIONAL_LIGHT_DIR);
    cubeShader->setVec3("directionalLight.ambient", LIGHT_AMBIENT);
    cubeShader->setVec3("directionalLight.diffuse", LIGHT_DIFFUSE);
}

void Renderer::setupWaterTintConfig() {
    cubeShader->setVec3("underwaterTint", UNDERWATER_TINT);
    cubeShader->setFloat("underwaterMix", 0.0f);
}

void Renderer::applyCubeShaderInitialConfig() {
    cubeShader->use();
    cubeShader->setFloat("fadeValue", FADE_VALUE);
    setupDirectionalLightConfig();
    setupWaterTintConfig();
    cubeShader->setInt("lightVolume", LIGHT_VOLUME_TEXTURE_UNIT);
}

void Renderer::applyWaterShaderInitialConfig() {
    waterShader->use();
    waterShader->setFloat("fadeValue", FADE_VALUE);
    waterShader->setVec3("directionalLight.direction", DIRECTIONAL_LIGHT_DIR);
    waterShader->setVec3("directionalLight.ambient", LIGHT_AMBIENT);
    waterShader->setVec3("directionalLight.diffuse", LIGHT_DIFFUSE);
    waterShader->setInt("lightVolume", LIGHT_VOLUME_TEXTURE_UNIT);
}

Renderer::Renderer(unsigned int width, unsigned int height)
    : screenWidth{static_cast<float>(width)},
      screenHeight{static_cast<float>(height)} {
    cubeShader = std::make_unique<Shader>("shaders/cube_shader.vs",
                                          "shaders/cube_shader.fs");
    waterShader = std::make_unique<Shader>("shaders/water_shader.vs",
                                           "shaders/water_shader.fs");
    crosshairShader = std::make_unique<Shader>("shaders/crosshair_shader.vs",
                                               "shaders/crosshair_shader.fs");
    crosshair = std::make_unique<Crosshair>();
    statusTextRenderer =
        std::make_unique<StatusTextRenderer>(screenWidth, screenHeight);
    applyCubeShaderInitialConfig();
    applyWaterShaderInitialConfig();
}

Renderer::~Renderer() { std::cout << "Renderer::Shutdown!" << std::endl; }

void Renderer::updateWaterShaderParams(const Camera& camera) {
    const bool underwater = camera.isUnderwater();

    cubeShader->use();
    cubeShader->setBool("isUnderwater", underwater);
    cubeShader->setVec3("underwaterTint", UNDERWATER_TINT);
    cubeShader->setFloat("underwaterMix",
                         underwater ? UNDERWATER_MIX_FACTOR : 0.0f);

    waterShader->use();
    waterShader->setBool("shouldAnimateWater", 0);
}

void Renderer::updateSpotlightShaderParams(const Camera& camera) {
    cubeShader->setVec3("spotLight.position", camera.getPosition());
    cubeShader->setVec3("spotLight.direction", camera.getFront());
}

void Renderer::updateProjectionViewShaderParams(const Camera& camera) {
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 cameraView = camera.getViewMatrix();

    cubeShader->use();
    cubeShader->setVec3("viewPosition", camera.getPosition());
    cubeShader->setMat4("projection", projection);
    cubeShader->setMat4("view", cameraView);

    waterShader->use();
    waterShader->setVec3("viewPosition", camera.getPosition());
    waterShader->setMat4("projection", projection);
    waterShader->setMat4("view", cameraView);

    glm::mat4 projView = projection * cameraView;
    frustum.update(projView);
}

void Renderer::updateShaders(const Camera& camera) {
    waterShader->use();
    waterShader->setFloat("time", glfwGetTime());

    updateWaterShaderParams(camera);
    updateProjectionViewShaderParams(camera);

    cubeShader->use();
    lastCameraPosition = camera.getPosition();
}

void Renderer::renderOpaqueCubes(World& world) {
    for (const auto& [cubeType, cubeMaterial] : materials.get()) {
        if (WaterSystem::isWater(cubeType)) {
            continue;
        }
        TextureManager::BindTextureToUnit(cubeMaterial.mainDiffuseTexturePath,
                                          cubeMaterial.mainDiffuseUnit);
        cubeShader->setInt("material.diffuseMain",
                           cubeMaterial.mainDiffuseUnit);
        cubeShader->setBool("material.useSecondaryTexture",
                            cubeMaterial.useSecondaryTexture);
        if (cubeType == CubeType::LOG) {
            TextureManager::BindTextureToUnit(
                cubeMaterial.secondaryDiffuseTexturePath,
                cubeMaterial.secondaryDiffuseUnit);
            cubeShader->setInt("material.diffuseSecondary",
                               cubeMaterial.secondaryDiffuseUnit);
        }
        cubeShader->setFloat("material.shininess", cubeMaterial.shininess);
        cubeShader->setFloat("material.alpha", cubeMaterial.alpha);
        world.renderByType(*cubeShader, cubeType);
    }
}

void Renderer::renderWater(World& world) {
    // Save current OpenGL state
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    GLint srcBlend, dstBlend;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcBlend);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &dstBlend);

    // Set water rendering state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    const Material& waterMat = materials.get(CubeType::WATER_SOURCE);

    TextureManager::BindTextureToUnit(waterMat.mainDiffuseTexturePath,
                                      waterMat.mainDiffuseUnit);

    waterShader->use();
    waterShader->setInt("material.diffuseMain", waterMat.mainDiffuseUnit);
    waterShader->setFloat("material.shininess", waterMat.shininess);
    waterShader->setFloat("material.alpha", waterMat.alpha);
    waterShader->setBool("shouldAnimateWater", true);

    world.renderWaterMeshes(*waterShader);

    waterShader->setBool("shouldAnimateWater", false);

    // Restore OpenGL state
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (!blendEnabled) {
        glDisable(GL_BLEND);
    }
    glBlendFunc(srcBlend, dstBlend);
}

void Renderer::renderCurrentWorldView(World& world) {
    world.performFrustumCulling(frustum);
    cubeShader->use();
    renderOpaqueCubes(world);
    renderWater(world);
}

void Renderer::renderCrosshair() {
    crosshairShader->use();
    crosshairShader->setVec3("color", 1.0f, 1.0f, 1.0f);
    crosshair->render();
}

void Renderer::render(unsigned int fps, World& world) {
    glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderCurrentWorldView(world);
    renderCrosshair();
    statusTextRenderer->renderStatus(fps, lastCameraPosition);
}