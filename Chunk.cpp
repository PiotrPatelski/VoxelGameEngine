#include "Chunk.hpp"
#include "FastNoiseLite.h"
#include "VertexData.hpp"
#include "GridGenerator.hpp"

static constexpr int matrixAttributeCount{4};

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
        if (not grid[neighbor.x][neighbor.z][neighbor.y]) {
            return true;
        }
    }
    return false;
}
} // namespace

Chunk::Chunk(int chunkSize, int worldXindex, int worldZindex,
             unsigned int sharedVBO, unsigned int sharedEBO,
             unsigned int sharedWaterEBO)
    : size{chunkSize},
      chunkWorldXPosition{worldXindex},
      chunkWorldZPosition{worldZindex},
      waterHeight{14},
      modified{true},
      regularCubeEBO{sharedEBO},
      waterEBO{sharedWaterEBO} {
    setupVAO(sharedVBO, sharedEBO);
    voxelGrid = generateInitialVoxelGrid();
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

std::vector<std::vector<std::vector<bool>>> Chunk::generateInitialVoxelGrid() {
    GridGenerator generator(size, chunkWorldXPosition, chunkWorldZPosition);
    return generator.generateGrid();
}

void Chunk::createCube(const glm::vec3& pos, CubeType type) {
    cubes.push_back(std::make_unique<Cube>(pos, type));
    instanceModelMatrices[type].push_back(cubes.back()->getModel());
}

void Chunk::rebuildCubesFromGrid() {
    cubes.clear();
    instanceModelMatrices.clear();
    const auto initialCubeX = static_cast<float>(chunkWorldXPosition * size);
    const auto initialCubeZ = static_cast<float>(chunkWorldZPosition * size);
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            for (int y = 0; y < size; y++) {
                glm::vec3 cubePos{initialCubeX + static_cast<float>(x),
                                  static_cast<float>(y),
                                  initialCubeZ + static_cast<float>(z)};
                if (voxelGrid[x][z][y] and
                    isCubeExposed(voxelGrid, {x, y, z})) {
                    createCube(cubePos, getCubeTypeBasedOnHeight(y));
                } else if ((not voxelGrid[x][z][y]) and (y == waterHeight)) {
                    createCube(cubePos, CubeType::WATER);
                }
            }
        }
    }
}

void Chunk::rebuildVisibleInstances(const Frustum& frustum) {
    instanceModelMatrices.clear();
    for (const auto& cube : cubes) {
        if (cube) {
            glm::mat4 model = cube->getModel();
            glm::vec3 pos = glm::vec3(model[3]);
            glm::vec3 voxelMin = pos - glm::vec3(0.5f);
            glm::vec3 voxelMax = pos + glm::vec3(0.5f);
            if (frustum.isAABBInside(voxelMin, voxelMax)) {
                instanceModelMatrices[cube->getType()].push_back(model);
            }
        }
    }
}

void Chunk::generateInstanceBuffersForCubeTypes() {
    const std::array<CubeType, 4> cubeTypes = {
        CubeType::SAND, CubeType::DIRT, CubeType::GRASS, CubeType::WATER};
    for (const auto& type : cubeTypes) {
        instanceVBOs[type] = 0;
        glGenBuffers(1, &instanceVBOs[type]);
    }
}

void Chunk::uploadInstanceBuffer() {
    for (const auto& [cubeType, matrices] : instanceModelMatrices) {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[cubeType]);
        glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4),
                     matrices.data(), GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::clearInstanceBuffer() {
    for (auto& [cubeType, matrices] : instanceModelMatrices) {
        matrices.clear();
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[cubeType]);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::bindInstanceAttributesForType(CubeType cubeType) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[cubeType]);
    for (unsigned int i = 0; i < matrixAttributeCount; i++) {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(3 + i);
        glVertexAttribDivisor(3 + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::drawElements(CubeType type, unsigned int amount) {
    if (type == CubeType::WATER) {
        glDisable(GL_CULL_FACE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(waterIndicesCount),
                                GL_UNSIGNED_INT, 0, amount);
        glEnable(GL_CULL_FACE);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, regularCubeEBO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(indices.size()),
                                GL_UNSIGNED_INT, 0, amount);
    }
}

void Chunk::renderByType(Shader& shader, CubeType type) {
    // If no instances for this type => skip
    auto it = instanceModelMatrices.find(type);
    if (it == instanceModelMatrices.end() || it->second.empty()) {
        return;
    }
    unsigned int instanceCount = it->second.size();

    glBindVertexArray(vao);
    bindInstanceAttributesForType(type);
    drawElements(type, instanceCount);
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
        voxelGrid[localPos.x][localPos.z][localPos.y]) {
        return false;
    }
    voxelGrid[localPos.x][localPos.z][localPos.y] = true;
    modified = true;
    updateInstanceData();
    return true;
}

bool Chunk::removeCube(const glm::ivec3& localPos) {
    if (not isPositionWithinBounds(localPos) or
        not voxelGrid[localPos.x][localPos.z][localPos.y]) {
        return false;
    }
    voxelGrid[localPos.x][localPos.z][localPos.y] = false;
    modified = true;
    updateInstanceData();
    return true;
}

Chunk::~Chunk() {
    for (auto& [cubeType, instanceBufferID] : instanceVBOs) {
        glDeleteBuffers(1, &instanceBufferID);
    }
    glDeleteVertexArrays(1, &vao);
}