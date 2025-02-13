#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Clock.hpp"

class App {
   public:
    App();
    ~App();
    void run();
    void mouse_callback(GLFWwindow* targetWindow, double xposIn, double yposIn);
    void scroll_callback(GLFWwindow* targetWindow, double xoffset,
                         double yoffset);

   private:
    void processInput();

    GLFWwindow* window{nullptr};
    Camera camera{};
    std::unique_ptr<Renderer> renderer{nullptr};
    Clock frameTimeClock{};

    // settings
    // SCREEN SIZE
    const unsigned int SCR_WIDTH{1920};
    const unsigned int SCR_HEIGHT{1080};

    // MOUSE
    float lastX{0};
    float lastY{0};
    bool firstMouse{true};
};