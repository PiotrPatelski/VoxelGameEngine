#include "ChunkVoxels.hpp"
#include "VertexData.hpp" // only for CubeType enum
#include <array>
#include <queue>

namespace {
constexpr std::array<glm::ivec3, 6> neighborOffsets{
    glm::ivec3(1, 0, 0),  glm::ivec3(-1, 0, 0), glm::ivec3(0, 0, 1),
    glm::ivec3(0, 0, -1), glm::ivec3(0, 1, 0),  glm::ivec3(0, -1, 0)};

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

using LightGrid3D = std::vector<std::vector<std::vector<float>>>;

void propagateLight(std::queue<glm::ivec3>& outQueue, LightGrid3D& outLightGrid,
                    const glm::ivec3& currentTorchPos, float nextLevel,
                    int chunkSize, const ChunkVoxels::VoxelGrid& voxelGrid) {
    for (const auto& offset : neighborOffsets) {
        glm::ivec3 neighbor = currentTorchPos + offset;
        if (not isPositionWithinBounds(neighbor, chunkSize)) {
            continue;
        }
        auto& neighborLight = outLightGrid[neighbor.x][neighbor.z][neighbor.y];
        if (nextLevel <= neighborLight) {
            continue;
        }
        // Light the neighbor cell:
        neighborLight = nextLevel;
        // Only propagate further if that cell is air:
        if (voxelGrid[neighbor.x][neighbor.z][neighbor.y] == CubeType::NONE) {
            outQueue.push(neighbor);
        }
    }
}

LightGrid3D computeLightGrid(const ChunkVoxels::VoxelGrid& voxelGrid,
                             const std::vector<glm::ivec3>& torchPositions,
                             int chunkSize, float attenuationFactor) {
    LightGrid3D lightGrid(chunkSize,
                          std::vector<std::vector<float>>(
                              chunkSize, std::vector<float>(chunkSize, 0.0f)));
    std::queue<glm::ivec3> bfsQueue;
    for (const auto& torchPos : torchPositions) {
        lightGrid[torchPos.x][torchPos.z][torchPos.y] = 1.0f;
        bfsQueue.push(torchPos);
    }
    while (!bfsQueue.empty()) {
        const auto currentTorchPos = bfsQueue.front();
        bfsQueue.pop();
        const auto currentLevel =
            lightGrid[currentTorchPos.x][currentTorchPos.z][currentTorchPos.y];
        if (currentLevel < 0.01f) {
            continue;
        }
        const auto nextLevel = currentLevel * attenuationFactor;

        propagateLight(bfsQueue, lightGrid, currentTorchPos, nextLevel,
                       chunkSize, voxelGrid);
    }
    return lightGrid;
}

// 2) Flatten 3D grid into a linear volume
std::vector<float> toSingleVector(const LightGrid3D& grid) {
    const auto gridDimensionSize = grid.size();
    std::vector<float> volume;
    volume.reserve(gridDimensionSize * gridDimensionSize * gridDimensionSize);
    for (size_t z = 0; z < gridDimensionSize; ++z) {
        for (size_t y = 0; y < gridDimensionSize; ++y) {
            for (size_t x = 0; x < gridDimensionSize; ++x) {
                volume.push_back(grid[x][z][y]);
            }
        }
    }
    return volume;
}

} // namespace

ChunkVoxels::ChunkVoxels(int chunkSize, int worldXIndex, int worldZIndex)
    : size{chunkSize},
      chunkWorldXIndex{worldXIndex},
      chunkWorldZIndex{worldZIndex},
      treeGenerator{chunkSize},
      voxelGrid(generateInitialVoxelGrid()) {
    rebuildCubesFromGrid();
}

ChunkVoxels::ChunkVoxels(ChunkVoxels&& other) noexcept
    : size(other.size),
      chunkWorldXIndex(other.chunkWorldXIndex),
      chunkWorldZIndex(other.chunkWorldZIndex),
      treeGenerator(std::move(other.treeGenerator)),
      voxelGrid(std::move(other.voxelGrid)),
      cubes(std::move(other.cubes)),
      instanceModelMatrices(std::move(other.instanceModelMatrices)),
      torchPositions(std::move(other.torchPositions)),
      modified(other.modified) {}

ChunkVoxels& ChunkVoxels::operator=(ChunkVoxels&& other) noexcept {
    if (this != &other) {
        std::lock_guard lock(other.voxelMutex);
        size = other.size;
        chunkWorldXIndex = other.chunkWorldXIndex;
        chunkWorldZIndex = other.chunkWorldZIndex;
        treeGenerator = std::move(other.treeGenerator);
        voxelGrid = std::move(other.voxelGrid);
        cubes = std::move(other.cubes);
        instanceModelMatrices = std::move(other.instanceModelMatrices);
        torchPositions = std::move(other.torchPositions);
        modified = other.modified;
    }
    return *this;
}

bool ChunkVoxels::addCube(const glm::ivec3& localPos, CubeType cubeType) {
    std::lock_guard lock(voxelMutex);
    if (not isPositionWithinBounds(localPos, size) or isCubeInGrid(localPos)) {
        return false;
    }
    voxelGrid[localPos.x][localPos.z][localPos.y] = cubeType;
    if (cubeType == CubeType::TORCH) {
        torchPositions.push_back(localPos);
    }
    modified = true;
    return true;
}

bool ChunkVoxels::removeCube(const glm::ivec3& localPos) {
    std::lock_guard lock(voxelMutex);
    if (not isPositionWithinBounds(localPos, size) or
        not isCubeInGrid(localPos)) {
        return false;
    }
    if (voxelGrid[localPos.x][localPos.z][localPos.y] == CubeType::TORCH) {
        auto it =
            std::find(torchPositions.begin(), torchPositions.end(), localPos);
        if (it != torchPositions.end()) {
            torchPositions.erase(it);
        }
    }
    voxelGrid[localPos.x][localPos.z][localPos.y] = CubeType::NONE;
    treeGenerator.removeTreeCubeAt(localPos);
    modified = true;
    return true;
}

bool ChunkVoxels::isCubeInGrid(const glm::ivec3& localPos) const {
    return voxelGrid[localPos.x][localPos.z][localPos.y] != CubeType::NONE;
}

CubeData ChunkVoxels::computeCubeData() {
    std::lock_guard lock(voxelMutex);
    const auto lightGrid =
        computeLightGrid(voxelGrid, torchPositions, size, 0.8f);
    CubeData data;
    data.lightVolume = toSingleVector(lightGrid);

    auto instanceBuilder = [&](const glm::ivec3& worldPos, CubeType type) {
        auto cube = std::make_unique<Cube>(worldPos, type);
        data.instanceModelMatrices[type].push_back(cube->getModel());
        data.cubes.push_back(std::move(cube));
    };
    regenerateChunk(instanceBuilder);

    return data;
}

void ChunkVoxels::storeCubes(std::vector<std::unique_ptr<Cube>>&& newCubes) {
    std::lock_guard lock(voxelMutex);
    cubes = std::move(newCubes);
}

std::pair<glm::vec3, glm::vec3> ChunkVoxels::computeChunkAABB() const {
    const auto firstCubeXWorldPosition = chunkWorldXIndex * size;
    const auto chunkOriginBlockPositionY = 0.0f;
    const auto firstCubeZWorldPosition = chunkWorldZIndex * size;
    const glm::vec3 chunkBoundsMin{firstCubeXWorldPosition,
                                   chunkOriginBlockPositionY,
                                   firstCubeZWorldPosition};
    const auto chunkBoundsMax = chunkBoundsMin + glm::vec3(size);
    return {chunkBoundsMin, chunkBoundsMax};
}

ChunkVoxels::VoxelGrid ChunkVoxels::generateInitialVoxelGrid() {
    return GridGenerator(size, chunkWorldXIndex, chunkWorldZIndex)
        .generateGrid();
}

void ChunkVoxels::createCube(const glm::ivec3& worldCubePos,
                             CubeType cubeType) {
    cubes.push_back(std::make_unique<Cube>(worldCubePos, cubeType));
    instanceModelMatrices[cubeType].push_back(cubes.back()->getModel());
}

void ChunkVoxels::processVoxelGrid(float firstCubeXWorldPosition,
                                   float firstCubeZWorldPosition,
                                   const CubeCreator& createCube) {
    for (int x = 0; x < size; ++x)
        for (int z = 0; z < size; ++z)
            for (int y = 0; y < size; ++y) {
                glm::ivec3 worldCubePos(firstCubeXWorldPosition + x, y,
                                        firstCubeZWorldPosition + z);
                auto cubeType = voxelGrid[x][z][y];
                if (cubeType != CubeType::NONE and
                    isCubeExposed(voxelGrid, {x, y, z})) {
                    createCube(worldCubePos, cubeType);
                } else if (y == waterHeight) {
                    createCube(worldCubePos, CubeType::WATER);
                }
            }
}

void ChunkVoxels::regenerateChunk(const CubeCreator& createCube) {
    float firstCubeXWorldPosition = static_cast<float>(chunkWorldXIndex * size);
    float firstCubeZWorldPosition = static_cast<float>(chunkWorldZIndex * size);
    processVoxelGrid(firstCubeXWorldPosition, firstCubeZWorldPosition,
                     createCube);

    treeGenerator.generateTrees(voxelGrid);
    treeGenerator.reapplyTrunks(firstCubeXWorldPosition,
                                firstCubeZWorldPosition, createCube);
    treeGenerator.reapplyCrowns(firstCubeXWorldPosition,
                                firstCubeZWorldPosition, createCube);
}

void ChunkVoxels::rebuildCubesFromGrid() {
    cubes.clear();
    instanceModelMatrices.clear();
    regenerateChunk([this](const glm::ivec3& worldCubePos, CubeType cubeType) {
        createCube(worldCubePos, cubeType);
    });
}
