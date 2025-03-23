#include "Chunk.hpp"
#include "FastNoiseLite.h"

static constexpr int chunkSize{64};
static constexpr int mat4Length{4};

// clang-format off
//Pos: {X, Y, Z} Color:{R, G, B} TexCord:{X, Y} Normal{X, Y, Z}
std::vector<Vertex> Chunk::vertices = {
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f,  0.5f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f,  -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

    {{-0.5f, -0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f},{ 0.0f, 0.0f, 1.0f}},
    {{0.5f,  -0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f},{ 0.0f, 0.0f, 1.0f}},
    {{0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f}}, 
    {{-0.5f, 0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f},{ 0.0f, 0.0f, 1.0f}}, 

    {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f},{-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f},{-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f,-0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f},{-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f},{-1.0f, 0.0f, 0.0f}},

    {{0.5f,  0.5f,  0.5f},  {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f},{ 1.0f, 0.0f, 0.0f}},
    {{0.5f,  -0.5f,-0.5f},  {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f},{ 1.0f, 0.0f, 0.0f}},
    {{0.5f,  0.5f, -0.5f},  {0.5f, 0.5f, 0.0f}, {1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f}},
    {{0.5f,  -0.5f, 0.5f},  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f},{ 1.0f, 0.0f, 0.0f}},

    {{-0.5f, -0.5f,-0.5f},  {0.5f, 0.0f, 0.5f}, {0.0f, 1.0f},{ 0.0f, -1.0f, 0.0f}},
    {{0.5f,  -0.5f,-0.5f},  {0.5f, 0.0f, 0.5f}, {1.0f, 1.0f},{ 0.0f, -1.0f, 0.0f}},
    {{0.5f,  -0.5f, 0.5f},  {0.5f, 0.0f, 0.5f}, {1.0f, 0.0f},{ 0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.5f, 0.0f, 0.5f}, {0.0f, 0.0f},{ 0.0f, -1.0f, 0.0f}},

    {{-0.5f, 0.5f, -0.5f},  {0.0f, 0.5f, 0.5f}, {0.0f, 1.0f},{ 0.0f, 1.0f, 0.0f}},
    {{0.5f,  0.5f,  0.5f},  {0.0f, 0.5f, 0.5f}, {1.0f, 0.0f},{ 0.0f, 1.0f, 0.0f}},
    {{0.5f,  0.5f, -0.5f},  {0.0f, 0.5f, 0.5f}, {1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.5f, 0.5f}, {0.0f, 0.0f},{ 0.0f, 1.0f, 0.0f}}};

std::vector<unsigned int> Chunk::indices = {
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

static bool isCubeExposed(
    const std::vector<std::vector<std::vector<bool>>>& cubePresent,
    const glm::vec3& gridPosition) {
    const std::array<std::array<int, 3>, 6> neighborOffsets{{
        {{1, 0, 0}},  // +X
        {{-1, 0, 0}}, // -X
        {{0, 1, 0}},  // +Z
        {{0, -1, 0}}, // -Z
        {{0, 0, 1}},  // +Y
        {{0, 0, -1}}  // -Y
    }};

    for (const auto& offset : neighborOffsets) {
        int neighborX = gridPosition.x + offset[0];
        int neighborZ = gridPosition.z + offset[1];
        int neighborY = gridPosition.y + offset[2];

        // If neighbor is out-of-bounds, cube is exposed.
        if (neighborX < 0 || neighborX >= chunkSize || neighborZ < 0 ||
            neighborZ >= chunkSize || neighborY < 0 || neighborY >= chunkSize) {
            return true;
        }
        // If neighbor cell is empty, cube is exposed.
        if (!cubePresent[neighborX][neighborZ][neighborY]) {
            return true;
        }
    }
    return false;
}

Chunk::Chunk(int worldXindex, int worldZindex, unsigned int sharedVBO,
             unsigned int sharedEBO)
    : chunkWorldXPosition{worldXindex}, chunkWorldZPosition{worldZindex} {
    // 1. Set up the VAO.
    setupVAO(sharedVBO, sharedEBO);

    // 2. Generate cubes from noise with hidden-face removal.
    generateCubeData();

    // 3. Upload instance data.
    uploadInstanceData();
}

void Chunk::setupVAO(unsigned int sharedVBO, unsigned int sharedEBO) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);

    const unsigned int stride = sizeof(Vertex);
    // POSITION attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // COLOR attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    // TEXCOORD attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    // NORMAL attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate the instance VBO.
    glGenBuffers(1, &buffer);
    glBindVertexArray(0);
}

void Chunk::generateCubeData() {
    // Prepare 3D boolean grid and column heights.
    std::vector<std::vector<std::vector<bool>>> cubePresent(
        chunkSize, std::vector<std::vector<bool>>(
                       chunkSize, std::vector<bool>(chunkSize, false)));
    std::vector<std::vector<int>> columnHeights(chunkSize,
                                                std::vector<int>(chunkSize, 0));

    generateCubePresence(cubePresent, columnHeights);

    // Calculate initial world-space base positions.
    float initialCubeX = static_cast<float>(chunkWorldXPosition * chunkSize);
    float initialCubeZ = static_cast<float>(chunkWorldZPosition * chunkSize);

    generateVisibleCubes(cubePresent, initialCubeX, initialCubeZ);
}

void Chunk::generateCubePresence(
    std::vector<std::vector<std::vector<bool>>>& cubePresent,
    std::vector<std::vector<int>>& columnHeights) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int worldCubeX = chunkWorldXPosition * chunkSize + x;
            int worldCubeZ = chunkWorldZPosition * chunkSize + z;
            float heightValue = noise.GetNoise(static_cast<float>(worldCubeX),
                                               static_cast<float>(worldCubeZ));
            int height =
                static_cast<int>((heightValue + 1.0f) * 0.5f * chunkSize / 2);
            columnHeights[x][z] = height;
            for (int y = 0; y < chunkSize; y++) {
                if (y <= height) cubePresent[x][z][y] = true;
            }
        }
    }
}

void Chunk::generateVisibleCubes(
    const std::vector<std::vector<std::vector<bool>>>& cubePresent,
    float initialCubeX, float initialCubeZ) {
    std::vector<glm::mat4> initialModels;

    for (int gridX = 0; gridX < chunkSize; gridX++) {
        for (int gridZ = 0; gridZ < chunkSize; gridZ++) {
            for (int gridY = 0; gridY < chunkSize; gridY++) {
                if (!cubePresent[gridX][gridZ][gridY]) {
                    continue;
                }

                if (isCubeExposed(cubePresent,
                                  glm::vec3{gridX, gridY, gridZ})) {
                    glm::vec3 cubePos{initialCubeX + static_cast<float>(gridX),
                                      static_cast<float>(gridY),
                                      initialCubeZ + static_cast<float>(gridZ)};

                    cubes.push_back(std::make_unique<Cube>(cubePos));
                    initialModels.push_back(cubes.back()->getModel());
                }
            }
        }
    }
    modelMatrices = initialModels;
}

void Chunk::uploadInstanceData() {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
                 modelMatrices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // A mat4 is represented as 4 vec4 attributes (locations 4-7).
    for (unsigned int matIdx = 0; matIdx < mat4Length; matIdx++) {
        glVertexAttribPointer(mat4Length + matIdx, mat4Length, GL_FLOAT,
                              GL_FALSE, sizeof(glm::mat4),
                              (void*)(matIdx * sizeof(glm::vec4)));
        glEnableVertexAttribArray(mat4Length + matIdx);
        glVertexAttribDivisor(mat4Length + matIdx, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chunk::render(Shader& shader) {
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,
                            cubes.size());
    glBindVertexArray(0);
}

void Chunk::performFrustumCulling(const Frustum& frustum) {
    modelMatrices.clear();
    for (const auto& cube : cubes) {
        if (cube) {
            glm::mat4 model = cube->getModel();
            glm::vec3 pos = glm::vec3(model[3]); // Extract translation.
            glm::vec3 aabbMin = pos - glm::vec3(0.5f);
            glm::vec3 aabbMax = pos + glm::vec3(0.5f);
            if (isAABBInsideFrustum(aabbMin, aabbMax, frustum)) {
                modelMatrices.push_back(model);
            }
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
                 modelMatrices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::updateInstanceBuffer() {
    modelMatrices.clear();
    for (const auto& cube : cubes) {
        if (cube) {
            modelMatrices.push_back(cube->getModel());
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
                 modelMatrices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Chunk::~Chunk() {
    glDeleteBuffers(1, &buffer);
    glDeleteVertexArrays(1, &vao);
}
