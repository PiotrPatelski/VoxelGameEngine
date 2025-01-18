#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Camera.hpp"
#include "Shader.hpp"

class Renderer {
   public:
    Renderer(unsigned int screenWidth, unsigned int screenHeight);
    ~Renderer();
    void render(const Camera& camera, GLFWwindow* window);

   private:
    float screenWidth{0};
    float screenHeight{0};
    unsigned int texture1{0};
    unsigned int texture2{0};
    unsigned int specularMapContainer{0};
    unsigned int emissionMap{0};
    std::unique_ptr<Shader> cubeShader{nullptr};
    std::unique_ptr<Shader> lightCubeShader{nullptr};
};