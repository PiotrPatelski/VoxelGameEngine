#include "Chunk.hpp"
#include "FastNoiseLite.h"
#include "VertexData.hpp"
#include "GridGenerator.hpp"

static constexpr int mat4Length{4};

namespace {
constexpr CubeType getCubeTypeBasedOnHeight(int height) {
    if (height < 11)
        return CubeType::SAND;
    else if (height < 14)
        return CubeType::DIRT;
    else
        return CubeType::GRASS;
}

constexpr std::array<glm::ivec3, 6> neighborOffsets = {
    glm::ivec3(1, 0, 0),  // +X
    glm::ivec3(-1, 0, 0), // -X
    glm::ivec3(0, 0, 1),  // +Z
    glm::ivec3(0, 0, -1), // -Z
    glm::ivec3(0, 1, 0),  // +Y
    glm::ivec3(0, -1, 0)  // -Y
};

bool isCubeExposed(const std::vector<std::vector<std::vector<bool>>>& grid,
                   const glm::ivec3& pos) {
    const auto chunkSize = static_cast<int>(grid.size());
    for (const auto& offset : neighborOffsets) {
        glm::ivec3 neighbor = pos + offset;
        // If neighbor is out of bounds, we consider the cube exposed.
        if (neighbor.x < 0 or neighbor.x >= chunkSize or neighbor.z < 0 or
            neighbor.z >= chunkSize or neighbor.y < 0 or
            neighbor.y >= chunkSize) {
            return true;
        }
        // If neighbor cell is false (i.e. no cube present), then current cube
        // is exposed.
        if (!grid[neighbor.x][neighbor.z][neighbor.y]) {
            return true;
        }
    }
    return false;
}
} // namespace

Chunk::Chunk(int chunkSize, int worldXindex, int worldZindex,
             unsigned int sharedVBO, unsigned int sharedEBO)
    : size{chunkSize},
      chunkWorldXPosition{worldXindex},
      chunkWorldZPosition{worldZindex},
      waterHeight{14},
      modified{true} {
    setupVAO(sharedVBO, sharedEBO);
    cubeGrid = generateInitialCubeGrid();
    generateInstanceBuffersForCubeTypes();
    rebuildCubesFromGrid();
    uploadInstanceBuffer();
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
    // TEXCOORD attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);
    // NORMAL attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

std::vector<std::vector<std::vector<bool>>> Chunk::generateInitialCubeGrid() {
    GridGenerator generator(size, chunkWorldXPosition, chunkWorldZPosition);
    return generator.generateGrid();
}

void Chunk::createCube(const glm::vec3& pos, CubeType type) {
    cubes.push_back(std::make_unique<Cube>(pos, type));
    instanceMatrices[type].push_back(cubes.back()->getModel());
}

void Chunk::rebuildCubesFromGrid() {
    cubes.clear();
    instanceMatrices.clear();
    const auto initialCubeX = static_cast<float>(chunkWorldXPosition * size);
    const auto initialCubeZ = static_cast<float>(chunkWorldZPosition * size);
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            for (int y = 0; y < size; y++) {
                glm::vec3 cubePos{initialCubeX + static_cast<float>(x),
                                  static_cast<float>(y),
                                  initialCubeZ + static_cast<float>(z)};
                if (cubeGrid[x][z][y] and isCubeExposed(cubeGrid, {x, y, z})) {
                    createCube(cubePos, getCubeTypeBasedOnHeight(y));
                } else if (not cubeGrid[x][z][y] and y <= waterHeight) {
                    cubeGrid[x][z][y] = true;
                    createCube(cubePos, CubeType::WATER);
                }
            }
        }
    }
}

void Chunk::rebuildVisibleInstances(const Frustum& frustum) {
    instanceMatrices.clear();
    for (const auto& cube : cubes) {
        if (cube) {
            glm::mat4 model = cube->getModel();
            glm::vec3 pos = glm::vec3(model[3]);
            glm::vec3 voxelMin = pos - glm::vec3(0.5f);
            glm::vec3 voxelMax = pos + glm::vec3(0.5f);
            if (frustum.isAABBInside(voxelMin, voxelMax)) {
                instanceMatrices[cube->getType()].push_back(model);
            }
        }
    }
}

void Chunk::generateInstanceBuffersForCubeTypes() {
    const std::array<CubeType, 4> cubeTypes = {
        CubeType::SAND, CubeType::DIRT, CubeType::GRASS, CubeType::WATER};
    for (const auto& type : cubeTypes) {
        instanceBuffers[type] = 0;
        glGenBuffers(1, &instanceBuffers[type]);
    }
}

void Chunk::uploadInstanceBuffer() {
    for (const auto& [cubeType, matrices] : instanceMatrices) {
        glBindBuffer(GL_ARRAY_BUFFER, instanceBuffers[cubeType]);
        glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4),
                     matrices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Chunk::clearInstanceBuffer() {
    for (auto& [cubeType, matrices] : instanceMatrices) {
        matrices.clear();
        glBindBuffer(GL_ARRAY_BUFFER, instanceBuffers[cubeType]);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Chunk::renderByType(Shader& shader, CubeType type, size_t indicesSize) {
    glBindVertexArray(vao);
    auto it = instanceMatrices.find(type);
    if (it == instanceMatrices.end() or it->second.empty()) {
        glBindVertexArray(0);
        return;
    }
    unsigned int instanceCount = it->second.size();
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffers[type]);
    for (unsigned int i = 0; i < mat4Length; i++) {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(3 + i);
        glVertexAttribDivisor(3 + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indicesSize),
                            GL_UNSIGNED_INT, 0,
                            static_cast<GLsizei>(instanceCount));
    glBindVertexArray(0);
}

std::pair<glm::vec3, glm::vec3> Chunk::computeChunkAABB() const {
    const auto chunkOriginBlockPositionX = chunkWorldXPosition * size;
    const auto chunkOriginBlockPositionY = 0.0f;
    const auto chunkOriginBlockPositionZ = chunkWorldZPosition * size;
    const glm::vec3 chunkBoundsMin{chunkOriginBlockPositionX,
                                   chunkOriginBlockPositionY,
                                   chunkOriginBlockPositionZ};
    const auto chunkBoundsMax = chunkBoundsMin + glm::vec3(size);
    return {chunkBoundsMin, chunkBoundsMax};
}

void Chunk::performFrustumCulling(const Frustum& frustum) {
    const auto [chunkMin, chunkMax] = computeChunkAABB();
    if (not frustum.isAABBInside(chunkMin, chunkMax)) {
        clearInstanceBuffer();
        return;
    }
    rebuildVisibleInstances(frustum);
    uploadInstanceBuffer();
}

void Chunk::updateInstanceData() {
    if (modified) {
        rebuildCubesFromGrid();
        uploadInstanceBuffer();
        modified = false;
    }
}

bool Chunk::isPositionWithinBounds(const glm::ivec3& pos) const {
    return (pos.x >= 0 and pos.x < size and pos.z >= 0 and pos.z < size and
            pos.y >= 0 and pos.y < size);
}

bool Chunk::addCube(const glm::ivec3& localPos) {
    if (not isPositionWithinBounds(localPos) or
        cubeGrid[localPos.x][localPos.z][localPos.y]) {
        return false;
    }
    cubeGrid[localPos.x][localPos.z][localPos.y] = true;
    modified = true;
    updateInstanceData();
    return true;
}

bool Chunk::removeCube(const glm::ivec3& localPos) {
    if (not isPositionWithinBounds(localPos) or
        not cubeGrid[localPos.x][localPos.z][localPos.y]) {
        return false;
    }
    cubeGrid[localPos.x][localPos.z][localPos.y] = false;
    modified = true;
    updateInstanceData();
    return true;
}

Chunk::~Chunk() {
    for (auto& pair : instanceBuffers) {
        glDeleteBuffers(1, &pair.second);
    }
    glDeleteVertexArrays(1, &vao);
}