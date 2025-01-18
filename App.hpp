#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Renderer.hpp"
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
    std::unique_ptr<Renderer> renderer{nullptr};
};