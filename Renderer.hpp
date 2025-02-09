#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "FontManager.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

class Renderer {
   public:
    Renderer(unsigned int screenWidth, unsigned int screenHeight);
    ~Renderer();
    void updateShaders(const Camera& camera);
    void render(unsigned int fps);

   private:
    float screenWidth{0};
    float screenHeight{0};
    unsigned int texture1{0};
    unsigned int texture2{0};
    unsigned int specularMapContainer{0};
    unsigned int emissionMap{0};
    std::unique_ptr<Shader> cubeShader{nullptr};
    std::unique_ptr<Shader> lightCubeShader{nullptr};
    std::unique_ptr<FontManager> fontManager{nullptr};
};