#include "ChunkVoxels.hpp"
#include "VertexData.hpp" // only for CubeType enum
#include <array>

namespace {
constexpr std::array<glm::ivec3, 6> neighbourOffsets{
    glm::ivec3(1, 0, 0),  glm::ivec3(-1, 0, 0), glm::ivec3(0, 0, 1),
    glm::ivec3(0, 0, -1), glm::ivec3(0, 1, 0),  glm::ivec3(0, -1, 0)};

bool isPositionWithinBounds(const glm::ivec3& pos, int chunkSize) {
    return (pos.x >= 0 and pos.x < chunkSize and pos.z >= 0 and
            pos.z < chunkSize and pos.y >= 0 and pos.y < chunkSize);
}

bool isCubeExposed(const GridGenerator::VoxelGrid& grid,
                   const glm::ivec3& pos) {
    const auto chunkSize = static_cast<int>(grid.size());
    for (const auto& offset : neighbourOffsets) {
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

ChunkVoxels::ChunkVoxels(int chunkSize, int worldXIndex, int worldZIndex)
    : size{chunkSize},
      chunkWorldXIndex{worldXIndex},
      chunkWorldZIndex{worldZIndex},
      treeGenerator{chunkSize} {
    voxelGrid = generateInitialVoxelGrid();
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
    modified = true;
    return true;
}

bool ChunkVoxels::removeCube(const glm::ivec3& localPos) {
    std::lock_guard lock(voxelMutex);
    if (not isPositionWithinBounds(localPos, size) or
        not isCubeInGrid(localPos)) {
        return false;
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
    CubeData cubeData;
    auto cubeDataApplier = [&cubeData](const glm::ivec3& worldCubePos,
                                       CubeType cubeType) {
        auto cube = std::make_unique<Cube>(worldCubePos, cubeType);
        cubeData.instanceModelMatrices[cubeType].push_back(cube->getModel());
        cubeData.cubes.push_back(std::move(cube));
    };
    regenerateChunk(cubeDataApplier);
    return cubeData;
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
