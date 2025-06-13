#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 position{};
    glm::vec2 texCoord{};
    glm::vec3 normal{};
};

struct EntityVertex {
    glm::vec3 position{};
    glm::vec2 texCoord{};
};

// clang-format off
//Pos: {X, Y, Z}
const std::vector<glm::vec3> cubeVertices = {
    {-0.5f, -0.5f, -0.5f},
    {0.5f,  0.5f,  -0.5f},
    {0.5f,  -0.5f, -0.5f},
    {-0.5f, 0.5f,  -0.5f},

    {-0.5f, -0.5f, 0.5f},
    {0.5f,  -0.5f, 0.5f},
    {0.5f,  0.5f,  0.5f},
    {-0.5f, 0.5f,  0.5f},

    {-0.5f, 0.5f,  0.5f},
    {-0.5f, 0.5f, -0.5f},
    {-0.5f, -0.5f,-0.5f},
    {-0.5f, -0.5f, 0.5f},

    {0.5f,  0.5f,  0.5f},
    {0.5f,  -0.5f,-0.5f},
    {0.5f,  0.5f, -0.5f},
    {0.5f,  -0.5f, 0.5f},

    {-0.5f, -0.5f,-0.5f},
    {0.5f,  -0.5f,-0.5f},
    {0.5f,  -0.5f, 0.5f},
    {-0.5f, -0.5f, 0.5f},
    
    {-0.5f, 0.5f, -0.5f},
    {0.5f,  0.5f,  0.5f},
    {0.5f,  0.5f, -0.5f},
    {-0.5f, 0.5f,  0.5f}
};

//TexCord:{X, Y}
const std::vector<glm::vec2> wholeFileTexCoords = {
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},

    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 0.0f},
    
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},

    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 0.0f},

    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f}};

//Normal{X, Y, Z}
const std::vector<glm::vec3> cubeNormals = {
    {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},

    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},

    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},

    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},

    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},

    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};

const std::vector<unsigned int> indices = {
    0, 1, 2, 1, 0, 3,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 13, 12, 15,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 21, 20, 23
};
// clang-format on

const std::vector<unsigned int> waterIndices = {20, 21, 22, 21, 20, 23};