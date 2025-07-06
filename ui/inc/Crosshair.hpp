#pragma once

#include <glad/glad.h>
#include <array>

class Crosshair {
   public:
    Crosshair();
    ~Crosshair();
    Crosshair(const Crosshair&) = delete;
    Crosshair(Crosshair&&) = delete;
    Crosshair& operator=(const Crosshair&) = delete;
    Crosshair& operator=(Crosshair&&) = delete;
    void render();

   private:
    void setupVertexAttributes();
    float size{0.02f};
    std::array<float, 8> vertices{};
    unsigned vertexArrayAmount{1};
    unsigned vertexBufferAmount{1};
    unsigned vertexArrayObjects{0};
    unsigned vertexBufferObjects{0};
};