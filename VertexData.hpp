#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 position{};
    glm::vec2 texCoord{};
    glm::vec3 normal{};
};

// clang-format off
//Pos: {X, Y, Z} TexCord:{X, Y} Normal{X, Y, Z}
const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f,  -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f,  -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

    {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f,  -0.5f, 0.5f},  {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, 
    {{-0.5f, 0.5f,  0.5f},  {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, 

    {{-0.5f, 0.5f,  0.5f},  {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f},  {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f,-0.5f},  {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

    {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f,  -0.5f,-0.5f},  {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f,  -0.5f, 0.5f},  {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

    {{-0.5f, -0.5f,-0.5f},  {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f,  -0.5f,-0.5f},  {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f,  -0.5f, 0.5f},  {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

    {{-0.5f, 0.5f, -0.5f},  {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}};

const std::vector<unsigned int> indices = {
    // Front face (z = -0.5)
    0, 1, 2, 1, 0, 3,
    // Back face (z = 0.5)
    4, 5, 6, 6, 7, 4,
    // Left face (x = -0.5)
    8, 9, 10, 10, 11, 8,
    // Right face (x = 0.5)
    12, 13, 14, 13, 12, 15,
    // Top face (y = -0.5)
    16, 17, 18, 18, 19, 16,
    // Bottom face (y = 0.5)
    20, 21, 22, 21, 20, 23
};
// clang-format on

const std::vector<unsigned int> waterIndices = {
    // Tri 1
    20, 21, 22,
    // Tri 2
    21, 20, 23};
const size_t waterIndicesCount = waterIndices.size();