#include "Chunk.hpp"
#include "FastNoiseLite.h"

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

// static bool isCubeExposed(
//     const std::vector<std::vector<std::vector<bool>>>& cubePresence,
//     const glm::vec3& gridPosition) {
//     const std::array<std::array<int, 3>, 6> neighborOffsets{{
//         {{1, 0, 0}},  // +X
//         {{-1, 0, 0}}, // -X
//         {{0, 1, 0}},  // +Z
//         {{0, -1, 0}}, // -Z
//         {{0, 0, 1}},  // +Y
//         {{0, 0, -1}}  // -Y
//     }};

//     for (const auto& offset : neighborOffsets) {
//         int neighborX = gridPosition.x + offset[0];
//         int neighborZ = gridPosition.z + offset[1];
//         int neighborY = gridPosition.y + offset[2];

//         // If neighbor is out-of-bounds, cube is exposed.
//         if (neighborX < 0 || neighborX >= chunkSize || neighborZ < 0 ||
//             neighborZ >= chunkSize || neighborY < 0 || neighborY >=
//             chunkSize) { return true;
//         }
//         // If neighbor cell is empty, cube is exposed.
//         if (!cubePresence[neighborX][neighborZ][neighborY]) {
//             return true;
//         }
//     }
//     return false;
// }

Chunk::Chunk(int chunkSize, int worldXindex, int worldZindex,
             unsigned int sharedVBO, unsigned int sharedEBO)
    : size{chunkSize},
      chunkWorldXPosition{worldXindex},
      chunkWorldZPosition{worldZindex} {
    setupVAO(sharedVBO, sharedEBO);
    cubeGrid = generateInitialCubeGrid();
    generateInstanceBuffersForCubeTypes();
    rebuildCubesFromGrid();
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
    glBindVertexArray(0);
}

std::vector<std::vector<std::vector<bool>>> Chunk::generateInitialCubeGrid() {
    std::vector<std::vector<std::vector<bool>>> grid(
        size,
        std::vector<std::vector<bool>>(size, std::vector<bool>(size, false)));

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);

    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            int worldCubeX = chunkWorldXPosition * size + x;
            int worldCubeZ = chunkWorldZPosition * size + z;
            float heightValue = noise.GetNoise(static_cast<float>(worldCubeX),
                                               static_cast<float>(worldCubeZ));
            int height =
                static_cast<int>((heightValue + 1.0f) * 0.5f * size / 2);
            for (int y = 0; y < size; y++) {
                if (y <= height) grid[x][z][y] = true;
            }
        }
    }
    return grid;
}

void Chunk::rebuildCubesFromGrid() {
    cubes.clear();
    instanceMatrices.clear();

    float initialCubeX = static_cast<float>(chunkWorldXPosition * size);
    float initialCubeZ = static_cast<float>(chunkWorldZPosition * size);
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            for (int y = 0; y < size; y++) {
                if (!cubeGrid[x][z][y]) continue;
                glm::vec3 cubePos{initialCubeX + static_cast<float>(x),
                                  static_cast<float>(y),
                                  initialCubeZ + static_cast<float>(z)};
                CubeType type;
                if (y < 11)
                    type = CubeType::SAND;
                else if (y < 14)
                    type = CubeType::DIRT;
                else
                    type = CubeType::GRASS;

                cubes.push_back(std::make_unique<Cube>(cubePos, type));
                instanceMatrices[type].push_back(cubes.back()->getModel());
            }
        }
    }
}

void Chunk::generateInstanceBuffersForCubeTypes() {
    // Generate instance buffers map for each CubeType you plan to support.
    // Initially create buffers for SAND, DIRT, and GRASS.
    instanceBuffers[CubeType::SAND] = 0;
    instanceBuffers[CubeType::DIRT] = 0;
    instanceBuffers[CubeType::GRASS] = 0;
    glGenBuffers(1, &instanceBuffers[CubeType::SAND]);
    glGenBuffers(1, &instanceBuffers[CubeType::DIRT]);
    glGenBuffers(1, &instanceBuffers[CubeType::GRASS]);
}

void Chunk::uploadInstanceData() {
    for (const auto& pair : instanceMatrices) {
        CubeType type = pair.first;
        const auto& matrices = pair.second;
        unsigned int buffer = instanceBuffers[type];
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4),
                     matrices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Chunk::renderByType(Shader& shader, CubeType type) {
    glBindVertexArray(vao);
    auto it = instanceMatrices.find(type);
    if (it == instanceMatrices.end() || it->second.empty()) {
        glBindVertexArray(0);
        return;
    }
    unsigned int instanceCount = it->second.size();
    // Bind the buffer for this type.
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffers[type]);
    for (unsigned int i = 0; i < mat4Length; i++) {
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(4 + i);
        glVertexAttribDivisor(4 + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                            GL_UNSIGNED_INT, 0,
                            static_cast<GLsizei>(instanceCount));
    glBindVertexArray(0);
}

void Chunk::performFrustumCulling(const Frustum& frustum) {
    // Compute the chunk's AABB in world space.
    glm::vec3 chunkMin, chunkMax;
    computeChunkAABB(chunkMin, chunkMax);

    // If the whole chunk is outside the frustum, clear instance matrices.
    if (!isAABBInsideFrustum(chunkMin, chunkMax, frustum)) {
        for (auto& pair : instanceMatrices) {
            pair.second.clear();
        }
        for (auto& pair : instanceBuffers) {
            glBindBuffer(GL_ARRAY_BUFFER, pair.second);
            glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        return;
    }

    // Otherwise, rebuild the visible instance matrices every frame.
    std::unordered_map<CubeType, std::vector<glm::mat4>> visibleMatrices;
    for (const auto& cube : cubes) {
        if (cube) {
            glm::mat4 model = cube->getModel();
            glm::vec3 pos = glm::vec3(model[3]);
            glm::vec3 voxelMin = pos - glm::vec3(0.5f);
            glm::vec3 voxelMax = pos + glm::vec3(0.5f);
            if (isAABBInsideFrustum(voxelMin, voxelMax, frustum)) {
                visibleMatrices[cube->getType()].push_back(model);
            }
        }
    }
    instanceMatrices = std::move(visibleMatrices);

    // Update GPU instance buffers with the visible matrices.
    for (const auto& pair : instanceMatrices) {
        CubeType type = pair.first;
        unsigned int buffer = instanceBuffers[type];
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, pair.second.size() * sizeof(glm::mat4),
                     pair.second.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Chunk::updateInstanceBuffer() {
    rebuildCubesFromGrid();
    uploadInstanceData();
}

bool Chunk::addCube(const glm::ivec3& localPos, CubeType type) {
    if (localPos.x < 0 || localPos.x >= size || localPos.z < 0 ||
        localPos.z >= size || localPos.y < 0 || localPos.y >= size) {
        return false;
    }
    if (cubeGrid[localPos.x][localPos.z][localPos.y]) {
        return false;
    }
    cubeGrid[localPos.x][localPos.z][localPos.y] = true;
    updateInstanceBuffer();
    return true;
}

bool Chunk::removeCube(const glm::ivec3& localPos) {
    if (localPos.x < 0 || localPos.x >= size || localPos.z < 0 ||
        localPos.z >= size || localPos.y < 0 || localPos.y >= size) {
        return false;
    }
    if (!cubeGrid[localPos.x][localPos.z][localPos.y]) {
        return false;
    }
    cubeGrid[localPos.x][localPos.z][localPos.y] = false;
    updateInstanceBuffer();
    return true;
}

Chunk::~Chunk() {
    for (auto& pair : instanceBuffers) {
        glDeleteBuffers(1, &pair.second);
    }
    glDeleteVertexArrays(1, &vao);
}
