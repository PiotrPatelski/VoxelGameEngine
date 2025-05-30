#include "Crosshair.hpp"
#include <array>

Crosshair::~Crosshair() {
    if (vertexBufferObjects) {
        glDeleteBuffers(1, &vertexBufferObjects);
    }
    if (vertexArrayObjects) {
        glDeleteVertexArrays(1, &vertexArrayObjects);
    }
}

Crosshair::Crosshair() {
    const std::array<float, 8> vertices{
        -size, 0.0f, // horizontal line start
        size,  0.0f, // horizontal line end

        0.0f,  -size, // vertical line start
        0.0f,  size   // vertical line end
    };

    glGenVertexArrays(1, &vertexArrayObjects);
    glGenBuffers(1, &vertexBufferObjects);

    glBindVertexArray(vertexArrayObjects);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void*)0);
    glBindVertexArray(0);
}

void Crosshair::render() {
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(vertexArrayObjects);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}
