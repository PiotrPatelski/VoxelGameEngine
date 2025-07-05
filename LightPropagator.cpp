#include "LightPropagator.hpp"

namespace {
constexpr int PADDING{2};
VoxelTypes::VoxelGrid3D createEmptyVoxelGrid(size_t size) {
    return {size, std::vector<std::vector<CubeType>>(
                      size, std::vector<CubeType>(size, CubeType::NONE))};
}
VoxelTypes::LightGrid3D createEmptyLightGrid(size_t size) {
    return {size, std::vector<std::vector<float>>(
                      size, std::vector<float>(size, 0.0f))};
}
} // namespace

LightPropagator::LightPropagator(int chunkSize, float attenuationFactor)
    : originalSize(chunkSize),
      paddedSize(chunkSize + PADDING),
      attenuation(attenuationFactor) {
    paddedVoxels = createEmptyVoxelGrid(paddedSize);
    paddedLight = createEmptyLightGrid(paddedSize);
}

std::vector<float> LightPropagator::computeLightMask(
    const VoxelTypes::VoxelGrid3D& originalVoxelGrid,
    const std::vector<glm::ivec3>& torchPositions,
    const VoxelTypes::NeighborVoxelsMap& neighborsSurroundingCubes) {
    clearPaddedGrids();
    emplaceChunkIntoPaddedGrid(originalVoxelGrid);
    insertNeighborsSurroundingCubes(neighborsSurroundingCubes);
    seedChunkInternalTorches(torchPositions);
    runPropagation();
    return extractFinalChunkWithSeededLight();
}

void LightPropagator::clearPaddedGrids() {
    for (int x = 0; x < paddedSize; ++x) {
        for (int z = 0; z < paddedSize; ++z) {
            for (int y = 0; y < paddedSize; ++y) {
                paddedVoxels[x][z][y] = CubeType::NONE;
                paddedLight[x][z][y] = 0.0f;
            }
        }
    }
    std::queue<glm::ivec3> empty;
    std::swap(bfsQueue, empty);
}

void LightPropagator::emplaceChunkIntoPaddedGrid(
    const VoxelTypes::VoxelGrid3D& original) {
    for (int x = 0; x < originalSize; ++x) {
        for (int z = 0; z < originalSize; ++z) {
            for (int y = 0; y < originalSize; ++y) {
                paddedVoxels[x + 1][z + 1][y + 1] = original[x][z][y];
            }
        }
    }
}

void LightPropagator::insertNeighborsSurroundingCubes(
    const VoxelTypes::NeighborVoxelsMap& neighborCubes) {
    for (const auto& [position, cubeType] : neighborCubes) {
        if (!isPositionWithinBounds(position, paddedSize)) {
            continue;
        }
        paddedVoxels[position.x][position.z][position.y] = cubeType;
        if (cubeType == CubeType::TORCH) {
            paddedLight[position.x][position.z][position.y] = 1.0f;
            bfsQueue.push(position);
        }
    }
}

void LightPropagator::seedChunkInternalTorches(
    const std::vector<glm::ivec3>& torches) {
    for (const auto& local : torches) {
        const glm::ivec3 paddedGridOffset{1, 1, 1};
        const glm::ivec3 paddedPos = local + paddedGridOffset;
        if (!isPositionWithinBounds(paddedPos, paddedSize)) {
            continue;
        }
        paddedLight[paddedPos.x][paddedPos.z][paddedPos.y] = 1.0f;
        bfsQueue.push(paddedPos);
    }
}

void LightPropagator::runPropagation() {
    while (!bfsQueue.empty()) {
        const glm::ivec3 currentPos = bfsQueue.front();
        bfsQueue.pop();

        const auto currentLightValue =
            paddedLight[currentPos.x][currentPos.z][currentPos.y];
        if (currentLightValue < 0.01f) {
            continue;
        }
        const auto nextLightValue = currentLightValue * attenuation;

        propagateToNeighbors(currentPos, nextLightValue);
    }
}

void LightPropagator::propagateToNeighbors(const glm::ivec3& position,
                                           float nextLightValue) {
    for (const auto& offset : NEIGHBOR_OFFSETS) {
        glm::ivec3 neighborPos = position + offset;
        if (!isPositionWithinBounds(neighborPos, paddedSize)) {
            continue;
        }

        auto& currentNeighborLightValue =
            paddedLight[neighborPos.x][neighborPos.z][neighborPos.y];
        if (nextLightValue <= currentNeighborLightValue) {
            continue;
        }

        currentNeighborLightValue = nextLightValue;
        if (paddedVoxels[neighborPos.x][neighborPos.z][neighborPos.y] ==
            CubeType::NONE) {
            bfsQueue.push(neighborPos);
        }
    }
}

std::vector<float> LightPropagator::extractFinalChunkWithSeededLight() {
    std::vector<float> out;
    out.reserve(originalSize * originalSize * originalSize);

    for (int z = 1; z <= originalSize; ++z) {
        for (int y = 1; y <= originalSize; ++y) {
            for (int x = 1; x <= originalSize; ++x) {
                out.push_back(paddedLight[x][z][y]);
            }
        }
    }
    return out;
}
