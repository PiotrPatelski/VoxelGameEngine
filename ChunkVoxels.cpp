#include "ChunkVoxels.hpp"
#include "VertexData.hpp"
#include "LightPropagator.hpp"
#include <array>
#include <queue>
#include "VoxelTypes.hpp"

namespace {
constexpr int WATER_HEIGHT{14};
bool isCubeExposed(const VoxelTypes::VoxelGrid3D& grid, const glm::ivec3& pos) {
    const auto chunkSize = static_cast<int>(grid.size());
    for (const auto& offset : NEIGHBOR_OFFSETS) {
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

const std::unordered_map<CubeType, std::vector<glm::mat4>>&
ChunkVoxels::getInstanceModelMatrices() const {
    return instanceModelMatrices;
}

glm::vec3 ChunkVoxels::getChunkOrigin() const {
    return glm::vec3(chunkWorldXIndex * size, 0.0f, chunkWorldZIndex * size);
}

void ChunkVoxels::setModified(bool value) {
    std::lock_guard lock(voxelMutex);
    modified = value;
}

void ChunkVoxels::setNeighborsSurroundingCubes(
    VoxelTypes::NeighborVoxelsMap&& data) {
    std::lock_guard lock(voxelMutex);
    neighborsSurroundingCubes = std::move(data);
}
void ChunkVoxels::clearNeighborsSurroundingCubes() {
    std::lock_guard lock(voxelMutex);
    neighborsSurroundingCubes.clear();
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
    const float attenuation{0.8f};
    LightPropagator lp(size, attenuation);

    CubeData data;
    data.lightVolume = lp.computeLightMask(voxelGrid, torchPositions,
                                           neighborsSurroundingCubes);
    neighborsSurroundingCubes.clear();

    auto instanceBuilder = [&](const glm::ivec3& worldPos, CubeType type) {
        auto cube = std::make_unique<Cube>(worldPos, type);
        data.instanceModelMatrices[type].push_back(cube->getModel());
        data.cubes.push_back(std::move(cube));
    };
    regenerateChunk(instanceBuilder);
    return data;
}

CubeType ChunkVoxels::getCubeTypeAt(const glm::ivec3& position) const {
    return voxelGrid[position.x][position.z][position.y];
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

VoxelTypes::VoxelGrid3D ChunkVoxels::generateInitialVoxelGrid() {
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
                } else if (y == WATER_HEIGHT) {
                    createCube(worldCubePos, CubeType::WATER);
                }
            }
}

void ChunkVoxels::regenerateChunk(const CubeCreator& createCube) {
    const auto firstCubeXWorldPosition =
        static_cast<float>(chunkWorldXIndex * size);
    const auto firstCubeZWorldPosition =
        static_cast<float>(chunkWorldZIndex * size);
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
