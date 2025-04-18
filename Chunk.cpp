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

Chunk::Chunk(int chunkSize, int worldXindex, int worldZindex, bool cpuOnly)
    : size{chunkSize},
      chunkWorldXIndex{worldXindex},
      chunkWorldZIndex{worldZindex},
      treeManager{chunkSize},
      modified{true} {
    // Build everything on CPU: voxel grid and instanceModelMatrices
    voxelGrid = generateInitialVoxelGrid();
    rebuildCubesFromGrid();
}

// —— Full constructor: calls CPU build, then all GL setup on main thread ——
Chunk::Chunk(int chunkSize, int worldXindex, int worldZindex,
             unsigned sharedVBO, unsigned sharedEBO, unsigned sharedWaterEBO)
    : Chunk(chunkSize, worldXindex, worldZindex, /*cpuOnly=*/true) {
    // Record which EBOs to use
    regularCubeEBO = sharedEBO;
    waterEBO = sharedWaterEBO;

    // Set up VAO with cube indices
    setupVAO(sharedVBO, sharedEBO);
    // Now generate instance buffers and upload all instance data
    generateInstanceBuffersForCubeTypes();
    uploadInstanceBuffer();
}

// Called on main thread for CPU-only chunks
void Chunk::initializeGL(unsigned int sharedVBO, unsigned int sharedEBO,
                         unsigned int sharedWaterEBO) {
    regularCubeEBO = sharedEBO;
    waterEBO = sharedWaterEBO;

    setupVAO(sharedVBO, sharedEBO);
    generateInstanceBuffersForCubeTypes();
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
    GridGenerator generator(size, chunkWorldXIndex, chunkWorldZIndex);
    return generator.generateGrid();
}

void Chunk::createCube(const glm::vec3& worldCubePos, CubeType type) {
    cubes.push_back(std::make_unique<Cube>(worldCubePos, type));
    instanceModelMatrices[type].push_back(cubes.back()->getModel());
}

void Chunk::processVoxelGrid(float firstCubeXWorldPosition,
                             float firstCubeZWorldPosition,
                             const CubeCreator& applyCube) {
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            for (int y = 0; y < size; y++) {
                glm::vec3 worldCubePos(
                    firstCubeXWorldPosition + static_cast<float>(x),
                    static_cast<float>(y),
                    firstCubeZWorldPosition + static_cast<float>(z));
                const auto cubeType = voxelGrid[x][z][y];
                if (cubeType != CubeType::NONE &&
                    isCubeExposed(voxelGrid, glm::ivec3{x, y, z})) {
                    applyCube(worldCubePos, cubeType);
                } else if (y == waterHeight) {
                    applyCube(worldCubePos, CubeType::WATER);
                }
            }
        }
    }
}

void Chunk::regenerateChunk(const CubeCreator& applier) {
    const auto firstCubeXWorldPosition =
        static_cast<float>(chunkWorldXIndex * size);
    const auto firstCubeZWorldPosition =
        static_cast<float>(chunkWorldZIndex * size);

    processVoxelGrid(firstCubeXWorldPosition, firstCubeZWorldPosition, applier);

    treeManager.generateTrees(voxelGrid);
    treeManager.reapplyTrunks(firstCubeXWorldPosition, firstCubeZWorldPosition,
                              applier);
    treeManager.reapplyCrowns(firstCubeXWorldPosition, firstCubeZWorldPosition,
                              applier);
}

void Chunk::rebuildCubesFromGrid() {
    cubes.clear();
    instanceModelMatrices.clear();
    auto cubeCreator = [this](const glm::vec3& worldPos, CubeType type) {
        createCube(worldPos, type);
    };
    regenerateChunk(cubeCreator);
}

// FIX TO INCREASE FPS
std::unordered_map<CubeType, std::vector<glm::mat4>>
Chunk::rebuildVisibleInstances(const Frustum& frustum) {
    std::lock_guard<std::mutex> lock(voxelMutex);
    std::unordered_map<CubeType, std::vector<glm::mat4>> visible;
    for (const auto& cube : cubes) {
        if (cube) {
            glm::mat4 model = cube->getModel();
            if (frustum.isModelIncluded(model)) {
                visible[cube->getType()].push_back(model);
            }
        }
    }
    return visible;
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
    const auto it = instanceModelMatrices.find(type);
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
    const auto firstCubeXWorldPosition = chunkWorldXIndex * size;
    const auto chunkOriginBlockPositionY = 0.0f;
    const auto firstCubeZWorldPosition = chunkWorldZIndex * size;
    const glm::vec3 chunkBoundsMin{firstCubeXWorldPosition,
                                   chunkOriginBlockPositionY,
                                   firstCubeZWorldPosition};
    const auto chunkBoundsMax = chunkBoundsMin + glm::vec3(size);
    return {chunkBoundsMin, chunkBoundsMax};
}

void Chunk::performFrustumCulling(const Frustum& frustum) {
    const auto [chunkMin, chunkMax] = computeChunkAABB();
    if (not frustum.isAABBInside(chunkMin, chunkMax)) {
        clearInstanceBuffer();
        return;
    }
    if (!visibleUpdateRunning) {
        visibleUpdateFuture =
            std::async(std::launch::async, &Chunk::rebuildVisibleInstances,
                       this, std::ref(frustum));
        visibleUpdateRunning = true;
    } else {
        auto status =
            visibleUpdateFuture.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            instanceModelMatrices = visibleUpdateFuture.get();
            uploadInstanceBuffer();
            visibleUpdateRunning = false;
        }
    }
}

CubeData Chunk::computeCubeData() {
    std::lock_guard<std::mutex> lock(voxelMutex);
    CubeData data;
    auto cubeDataApplier = [&data](const glm::vec3& worldCubePos,
                                   CubeType type) {
        auto cube = std::make_unique<Cube>(worldCubePos, type);
        data.instanceModelMatrices[type].push_back(cube->getModel());
        data.cubes.push_back(std::move(cube));
    };
    regenerateChunk(cubeDataApplier);
    return data;
}

void Chunk::applyCubeData(CubeData&& data) {
    if (visibleUpdateRunning) {
        visibleUpdateFuture.wait();
        visibleUpdateRunning = false;
    }
    cubes = std::move(data.cubes);
    instanceModelMatrices = std::move(data.instanceModelMatrices);
    uploadInstanceBuffer();
    modified = false;
}

bool Chunk::addCube(const glm::ivec3& localPos, CubeType type) {
    std::lock_guard<std::mutex> lock(voxelMutex);
    if (not isPositionWithinBounds(localPos, size) or
        voxelGrid[localPos.x][localPos.z][localPos.y] != CubeType::NONE) {
        return false;
    }
    voxelGrid[localPos.x][localPos.z][localPos.y] = type;
    modified = true;
    return true;
}

bool Chunk::removeCube(const glm::ivec3& localPos) {
    std::lock_guard<std::mutex> lock(voxelMutex);
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