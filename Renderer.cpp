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

void Renderer::setupShaders() {
    cubeShader = std::make_unique<Shader>("shaders/cube_shader.vs",
                                          "shaders/cube_shader.fs");
    // lightCubeShader = std::make_unique<Shader>("shaders/light_source.vs",
    //                                            "shaders/light_source.fs");
    // lightCubeShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // lightCubeShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    cubeShader->use();
    cubeShader->setFloat("fadeValue", 0.2f);

    // directional light
    cubeShader->setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
    cubeShader->setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
    cubeShader->setVec3("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
    // point lights
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

void Renderer::setupMaterials() {
    materials[CubeType::SAND] = Material{
        "textures/sand.jpg", "./textures/matrix.jpg", 1, 99, 32.0f, 1.f};
    materials[CubeType::DIRT] = Material{
        "textures/dirt.jpg", "./textures/matrix.jpg", 2, 99, 16.0f, 1.f};
    materials[CubeType::GRASS] = Material{
        "textures/grass.jpg", "./textures/matrix.jpg", 3, 99, 8.0f, 1.f};
    materials[CubeType::WATER] = Material{
        "textures/water.jpg", "./textures/matrix.jpg", 4, 99, 64.0f, 0.5f};
}

Renderer::Renderer(unsigned int width, unsigned int height)
    : screenWidth{static_cast<float>(width)},
      screenHeight{static_cast<float>(height)} {
    std::cout << "Renderer::Init!" << std::endl;

    fontManager = std::make_unique<FontManager>(screenWidth, screenHeight);

    setupShaders();
    setupMaterials();
}

Renderer::~Renderer() {
    std::cout << "Renderer::Shutdown!" << std::endl;
    glfwTerminate();
}

void Renderer::updateShaders(const Camera& camera) {
    cubeShader->use();
    cubeShader->setFloat("time", glfwGetTime());
    cubeShader->setVec3("viewPosition", camera.getPosition());
    // spotLight
    cubeShader->setVec3("spotLight.position", camera.getPosition());
    cubeShader->setVec3("spotLight.direction", camera.getFront());

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.getZoom()),
                         screenWidth / screenHeight, 0.1f, 100.0f);
    glm::mat4 cameraView = camera.getViewMatrix();
    cubeShader->setMat4("projection", projection);
    cubeShader->setMat4("view", cameraView);
    glm::mat4 projView = projection * cameraView;
    frustum.update(projView);
}

void Renderer::render(unsigned int fps, World& world) {
    glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cubeShader->use();
    world.performFrustumCulling(frustum);

    for (const auto& [cubeType, cubeMaterial] : materials) {
        if (cubeType == CubeType::WATER) {
            continue;
        }
        TextureManager::BindTextureToUnit(cubeMaterial.diffuseTexturePath,
                                          cubeMaterial.diffuseUnit);
        cubeShader->setInt("material.diffuse", cubeMaterial.diffuseUnit);
        cubeShader->setFloat("material.shininess", cubeMaterial.shininess);
        cubeShader->setFloat("material.alpha", cubeMaterial.alpha);
        world.renderByType(*cubeShader, cubeType);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const Material& waterMat = materials[CubeType::WATER];
    TextureManager::BindTextureToUnit(waterMat.diffuseTexturePath,
                                      waterMat.diffuseUnit);
    cubeShader->setInt("material.diffuse", waterMat.diffuseUnit);
    cubeShader->setFloat("material.shininess", waterMat.shininess);
    cubeShader->setFloat("material.alpha", waterMat.alpha);
    world.renderByType(*cubeShader, CubeType::WATER);

    const std::string fpsCount{"FPS count: " + std::to_string(fps)};
    fontManager->renderText(fpsCount, 25.0f, 25.0f, 1.0f,
                            glm::vec3(0.5, 0.8f, 0.2f));
    fontManager->renderText("Pioter Craft Project", 1640.0f, 1010.0f, 0.5f,
                            glm::vec3(0.3, 0.7f, 0.9f));
}