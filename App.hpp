#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Clock.hpp"
#include "Cube.hpp"

class App {
   public:
    App();
    ~App();
    App(const App&) = delete;
    App(App&&) = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&) = delete;
    void run();
    void mouse_callback(GLFWwindow* targetWindow, double xposIn, double yposIn);
    void scroll_callback(GLFWwindow* targetWindow, double xoffset,
                         double yoffset);
    void mouse_button_callback(GLFWwindow* window, int button, int action,
                               int mods);

   private:
    void processInput();

    GLFWwindow* window{nullptr};
    Camera camera{};
    std::unique_ptr<Renderer> renderer{nullptr};
    std::unique_ptr<World> gameWorld{nullptr};
    Clock frameTimeClock{};

    // settings
    // SCREEN SIZE
    const unsigned int SCR_WIDTH{1920};
    const unsigned int SCR_HEIGHT{1080};

    // MOUSE
    float lastX{0};
    float lastY{0};
    bool firstMouse{true};
    CubeType selectedCubeType{CubeType::SAND};
};