#include "Chunk.hpp"
#include <cstdlib>
#include <functional>
#include "FastNoiseLite.h"
#include "VertexData.hpp"

static constexpr int matrixAttributeCount{4};

namespace {

constexpr std::array<glm::ivec3, 6> neighborOffsets = {
    glm::ivec3(1, 0, 0),  // +X
    glm::ivec3(-1, 0, 0), // -X
    glm::ivec3(0, 0, 1),  // +Z
    glm::ivec3(0, 0, -1), // -Z
    glm::ivec3(0, 1, 0),  // +Y
    glm::ivec3(0, -1, 0)  // -Y
};

bool isPositionWithinBounds(const glm::ivec3& pos, int chunkSize) {
    return (pos.x >= 0 and pos.x < chunkSize and pos.z >= 0 and
            pos.z < chunkSize and pos.y >= 0 and pos.y < chunkSize);
}

bool isCubeExposed(const GridGenerator::VoxelGrid& grid,
                   const glm::ivec3& pos) {
    const auto chunkSize = static_cast<int>(grid.size());
    for (const auto& offset : neighborOffsets) {
        glm::ivec3 neighbor = pos + offset;
        // If neighbor is out of bounds, we consider the cube exposed.
        if (not isPositionWithinBounds(neighbor, chunkSize)) {
            return true;
        }
        // If neighbor cell is false (i.e. no cube present), then current cube
        // is exposed.
        if (grid[neighbor.x][neighbor.z][neighbor.y] == CubeType::NONE) {
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
      treeManager{chunkSize},
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

std::vector<std::vector<std::vector<CubeType>>>
Chunk::generateInitialVoxelGrid() {
    GridGenerator generator(size, chunkWorldXPosition, chunkWorldZPosition);
    return generator.generateGrid();
}

void Chunk::createCube(const glm::vec3& pos, CubeType type) {
    cubes.push_back(std::make_unique<Cube>(pos, type));
    instanceModelMatrices[type].push_back(cubes.back()->getModel());
}

void Chunk::processVoxelGrid(float chunkOriginBlockPositionX,
                             float chunkOriginBlockPositionZ,
                             const CubeCreator& applyCube) {
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            for (int y = 0; y < size; y++) {
                glm::vec3 cubePos(
                    chunkOriginBlockPositionX + static_cast<float>(x),
                    static_cast<float>(y),
                    chunkOriginBlockPositionZ + static_cast<float>(z));
                if (voxelGrid[x][z][y] != CubeType::NONE &&
                    isCubeExposed(voxelGrid, glm::ivec3{x, y, z})) {
                    applyCube(cubePos, voxelGrid[x][z][y]);
                } else if (y == waterHeight) {
                    applyCube(cubePos, CubeType::WATER);
                }
            }
        }
    }
}

void Chunk::regenerateChunk(const CubeCreator& applier) {
    const float chunkOriginBlockPositionX =
        static_cast<float>(chunkWorldXPosition * size);
    const float chunkOriginBlockPositionZ =
        static_cast<float>(chunkWorldZPosition * size);

    processVoxelGrid(chunkOriginBlockPositionX, chunkOriginBlockPositionZ,
                     applier);

    treeManager.generateTrees(voxelGrid);
    treeManager.reapplyTrunks(chunkOriginBlockPositionX,
                              chunkOriginBlockPositionZ, applier);
    treeManager.reapplyCrowns(chunkOriginBlockPositionX,
                              chunkOriginBlockPositionZ, applier);
}

void Chunk::rebuildCubesFromGrid() {
    cubes.clear();
    instanceModelMatrices.clear();
    auto cubeCreator = [this](const glm::vec3& worldPos, CubeType type) {
        createCube(worldPos, type);
    };
    regenerateChunk(cubeCreator);
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
    const std::array<CubeType, 6> cubeTypes = {
        CubeType::SAND,  CubeType::DIRT, CubeType::GRASS,
        CubeType::WATER, CubeType::LOG,  CubeType::LEAVES};
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

CubeData Chunk::computeCubeData() {
    CubeData data;
    auto cubeDataApplier = [&data](const glm::vec3& pos, CubeType type) {
        auto cube = std::make_unique<Cube>(pos, type);
        data.instanceModelMatrices[type].push_back(cube->getModel());
        data.cubes.push_back(std::move(cube));
    };
    regenerateChunk(cubeDataApplier);
    return data;
}

void Chunk::applyCubeData(CubeData&& data) {
    cubes = std::move(data.cubes);
    instanceModelMatrices = std::move(data.instanceModelMatrices);
    uploadInstanceBuffer();
    modified = false;
}

bool Chunk::addCube(const glm::ivec3& localPos, CubeType type) {
    if (not isPositionWithinBounds(localPos, size) or
        voxelGrid[localPos.x][localPos.z][localPos.y] != CubeType::NONE) {
        return false;
    }
    voxelGrid[localPos.x][localPos.z][localPos.y] = type;
    modified = true;
    return true;
}

bool Chunk::removeCube(const glm::ivec3& localPos) {
    if (not isPositionWithinBounds(localPos, size) or
        voxelGrid[localPos.x][localPos.z][localPos.y] == CubeType::NONE) {
        return false;
    }
    voxelGrid[localPos.x][localPos.z][localPos.y] = CubeType::NONE;
    treeManager.removeTreeCubeAt(localPos);
    modified = true;
    return true;
}

Chunk::~Chunk() {
    for (auto& [cubeType, instanceBufferID] : instanceVBOs) {
        glDeleteBuffers(1, &instanceBufferID);
    }
    glDeleteVertexArrays(1, &vao);
}