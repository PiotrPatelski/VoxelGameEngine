#include "Renderer.hpp"
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
// settings

// LIGHTING
std::vector<glm::vec3> pointLightPositions = {
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

} // namespace

void Renderer::setupDirectionalLightConfig() {
    cubeShader->setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
    cubeShader->setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
    cubeShader->setVec3("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
}

void Renderer::setupPointLightsConfig() {
    for (unsigned int lightPosIndex = 0;
         lightPosIndex < pointLightPositions.size(); lightPosIndex++) {
        const std::string uniformName =
            "pointLights[" + std::to_string(lightPosIndex) + "]";
        cubeShader->setVec3(uniformName + ".position",
                            pointLightPositions[lightPosIndex]);
        cubeShader->setVec3(uniformName + ".ambient", 0.05f, 0.05f, 0.05f);
        cubeShader->setVec3(uniformName + ".diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader->setFloat(uniformName + ".constant", 1.0f);
        cubeShader->setFloat(uniformName + ".linear", 0.09f);
        cubeShader->setFloat(uniformName + ".quadratic", 0.032f);
    }
}

void Renderer::setupSpotlightConfig() {
    cubeShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    cubeShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    cubeShader->setFloat("spotLight.constant", 1.0f);
    cubeShader->setFloat("spotLight.linear", 0.09f);
    cubeShader->setFloat("spotLight.quadratic", 0.032f);
    cubeShader->setFloat("spotLight.innerCutOff",
                         glm::cos(glm::radians(12.5f)));
    cubeShader->setFloat("spotLight.outerCutOff",
                         glm::cos(glm::radians(15.0f)));
}
void Renderer::setupWaterTintConfig() {
    cubeShader->setVec3("underwaterTint", glm::vec3(0.0f, 0.3f, 0.5f));
    cubeShader->setFloat("underwaterMix", 0.0f);
}

void Renderer::applyCubeShaderInitialConfig() {
    // lightCubeShader = std::make_unique<Shader>("shaders/light_source.vs",
    //                                            "shaders/light_source.fs");
    // lightCubeShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // lightCubeShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    cubeShader->use();
    cubeShader->setFloat("fadeValue", 0.2f);
    setupDirectionalLightConfig();
    // setupPointLightsConfig();
    // setupSpotlightConfig();
    setupWaterTintConfig();
}

Renderer::Renderer(unsigned int width, unsigned int height)
    : screenWidth{static_cast<float>(width)},
      screenHeight{static_cast<float>(height)} {
    std::cout << "Renderer::Init!" << std::endl;

    statusTextRenderer =
        std::make_unique<StatusTextRenderer>(screenWidth, screenHeight);
    cubeShader = std::make_unique<Shader>("shaders/cube_shader.vs",
                                          "shaders/cube_shader.fs");
    applyCubeShaderInitialConfig();
}

Renderer::~Renderer() {
    std::cout << "Renderer::Shutdown!" << std::endl;
    glfwTerminate();
}

void Renderer::updateWaterShaderParams(const Camera& camera) {
    const float underwaterEffectHeight = 14.5f;
    const bool underwater = (camera.getPosition().y < underwaterEffectHeight);
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
                         screenWidth / screenHeight, 0.1f, 100.0f);
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
    // updateSpotlightShaderParams(camera);
    updateProjectionViewShaderParams(camera);
    lastCameraPosition = camera.getPosition();
}

void Renderer::renderOpaqueCubes(World& world) {
    for (const auto& [cubeType, cubeMaterial] : materials.get()) {
        if (cubeType == CubeType::WATER) continue;
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

void Renderer::render(unsigned int fps, World& world) {
    glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world.performFrustumCulling(frustum);
    cubeShader->use();
    renderOpaqueCubes(world);
    renderWater(world);
    statusTextRenderer->renderStatus(fps, lastCameraPosition);
}