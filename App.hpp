#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Camera.hpp"
#include "Shader.hpp"

class App {
   public:
    App();
    ~App();
    void run();
    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

   private:
    void processInput(GLFWwindow* window);

    GLFWwindow* window{nullptr};
    Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
    unsigned int texture1{0};
    unsigned int texture2{0};
    unsigned int specularMapContainer{0};
    unsigned int emissionMap{0};
    std::unique_ptr<Shader> cubeShader{nullptr};
    std::unique_ptr<Shader> lightCubeShader{nullptr};
};