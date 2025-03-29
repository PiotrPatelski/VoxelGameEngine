#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <unordered_map>
#include "FontManager.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "World.hpp"
#include "Frustum.hpp"
#include "Material.hpp"

class Renderer {
   public:
    Renderer(unsigned int screenWidth, unsigned int screenHeight,
             const World& world);
    ~Renderer();
    void updateShaders(const Camera& camera);
    void render(unsigned int fps, World& world);

   private:
    void setupShaders();
    void setupMaterials();
    float screenWidth{0};
    float screenHeight{0};

    std::unique_ptr<Shader> cubeShader{nullptr};
    std::unique_ptr<Shader> lightCubeShader{nullptr};
    std::unique_ptr<FontManager> fontManager{nullptr};
    std::unordered_map<CubeType, Material> materials;
    Frustum frustum{};
};