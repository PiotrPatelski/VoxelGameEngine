#pragma once

#include <glad/glad.h>

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
    float size{0.02f};
    unsigned vertexArrayObjects{0};
    unsigned vertexBufferObjects{0};
};