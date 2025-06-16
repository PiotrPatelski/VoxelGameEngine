#include "Renderer.hpp"
#include "Entity.hpp"
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

void Renderer::setupDirectionalLightConfig() {
    cubeShader->setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
    cubeShader->setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
    cubeShader->setVec3("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
}

void Renderer::setupWaterTintConfig() {
    cubeShader->setVec3("underwaterTint", glm::vec3(0.0f, 0.3f, 0.5f));
    cubeShader->setFloat("underwaterMix", 0.0f);
}

void Renderer::applyCubeShaderInitialConfig() {
    cubeShader->use();
    cubeShader->setFloat("fadeValue", 0.2f);
    setupDirectionalLightConfig();
    setupWaterTintConfig();
    cubeShader->setInt("lightVolume", 15);
}

Renderer::Renderer(unsigned int width, unsigned int height)
    : screenWidth{static_cast<float>(width)},
      screenHeight{static_cast<float>(height)} {
    cubeShader = std::make_unique<Shader>("shaders/cube_shader.vs",
                                          "shaders/cube_shader.fs");
    crosshairShader = std::make_unique<Shader>("shaders/crosshair_shader.vs",
                                               "shaders/crosshair_shader.fs");
    crosshair = std::make_unique<Crosshair>();
    statusTextRenderer =
        std::make_unique<StatusTextRenderer>(screenWidth, screenHeight);
    applyCubeShaderInitialConfig();
}

Renderer::~Renderer() {
    std::cout << "Renderer::Shutdown!" << std::endl;
    glfwTerminate();
}

void Renderer::updateWaterShaderParams(const Camera& camera) {
    const float underwaterEffectHeight{14.5f};
    const bool underwater{(camera.getPosition().y < underwaterEffectHeight)};
    cubeShader->setBool("isUnderwater", underwater);
    cubeShader->setVec3("underwaterTint", glm::vec3(0.0f, 0.3f, 0.5f));
    cubeShader->setFloat("underwaterMix", underwater ? 0.5f : 0.0f);
    cubeShader->setBool("shouldAnimateWater", 0);
}

void Renderer::updateSpotlightShaderParams(const Camera& camera) {
    cubeShader->setVec3("spotLight.position", camera.getPosition());
    cubeShader->setVec3("spotLight.direction", camera.getFront());
}

void Renderer::updateProjectionViewShaderParams(const Camera& camera) {
    cubeShader->setVec3("viewPosition", camera.getPosition());
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.getZoom()),
                         screenWidth / screenHeight, 0.1f, 200.0f);
    glm::mat4 cameraView = camera.getViewMatrix();
    cubeShader->setMat4("projection", projection);
    cubeShader->setMat4("view", cameraView);
    glm::mat4 projView = projection * cameraView;
    frustum.update(projView);
}

void Renderer::updateShaders(const Camera& camera) {
    cubeShader->use();
    cubeShader->setFloat("time", glfwGetTime());

    updateWaterShaderParams(camera);
    updateProjectionViewShaderParams(camera);
    lastCameraPosition = camera.getPosition();
}

void Renderer::renderOpaqueCubes(World& world) {
    for (const auto& [cubeType, cubeMaterial] : materials.get()) {
        if (cubeType == CubeType::WATER) {
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const Material& waterMat = materials.get(CubeType::WATER);
    TextureManager::BindTextureToUnit(waterMat.mainDiffuseTexturePath,
                                      waterMat.mainDiffuseUnit);
    cubeShader->setInt("material.diffuseMain", waterMat.mainDiffuseUnit);
    cubeShader->setFloat("material.shininess", waterMat.shininess);
    cubeShader->setFloat("material.alpha", waterMat.alpha);
    cubeShader->setInt("shouldAnimateWater", 1);
    world.renderByType(*cubeShader, CubeType::WATER);
    cubeShader->setInt("shouldAnimateWater", 0);
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