#include "Raycaster.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace {
glm::ivec3 computeInitialBlockPos(const glm::vec3& origin) {
    return glm::floor(origin);
}

glm::ivec3 computeStep(const glm::vec3& rayDir) { return glm::sign(rayDir); }

glm::vec3 computeDistanceToNextVoxel(const glm::vec3& origin,
                                     const glm::ivec3& blockPos,
                                     const glm::vec3& rayDir,
                                     const glm::ivec3& step) {
    // Compute the distance to the next voxel boundary along each axis.
    return (glm::vec3(blockPos) + glm::vec3(step) * 0.5f - origin) / rayDir;
}

glm::vec3 computeboundaryStepIncrement(const glm::vec3& rayDir) {
    return glm::abs(1.0f / rayDir);
}

RenderableChunk* findChunkAtCurrentRayPos(
    const std::unordered_map<ChunkCoord, std::unique_ptr<RenderableChunk>>&
        loadedChunks,
    const ChunkCoord& coords) {
    auto loadedChunk = loadedChunks.find(coords);
    return (loadedChunk != loadedChunks.end()) ? loadedChunk->second.get()
                                               : nullptr;
}
} // namespace

int floorDivide(int a, int b) { return (a >= 0) ? (a / b) : ((a - b + 1) / b); }

int negativeSafeModulo(int a, int b) {
    const int remainder{a % b};
    return (remainder < 0) ? remainder + b : remainder;
}

Raycaster::Raycaster(const Camera& camera, int chunkSize)
    : size{chunkSize},
      rayDir{glm::normalize(glm::normalize(camera.getFront()))},
      blockPos{computeInitialBlockPos(camera.getPosition())} {
    step = computeStep(rayDir);
    nextVoxelBoundary = computeDistanceToNextVoxel(camera.getPosition(),
                                                   blockPos, rayDir, step);
    boundaryStepIncrement = computeboundaryStepIncrement(rayDir);
}

std::optional<HitResult> Raycaster::raycastDDA(
    const std::unordered_map<ChunkCoord, std::unique_ptr<RenderableChunk>>&
        loadedChunks,
    float maxDistance) {
    distanceTraveled = 0.0f;

    while (distanceTraveled < maxDistance) {
        const auto chunkX = floorDivide(blockPos.x, size);
        const auto chunkZ = floorDivide(blockPos.z, size);
        const ChunkCoord coord{chunkX, chunkZ};
        const RenderableChunk* chunk =
            findChunkAtCurrentRayPos(loadedChunks, coord);
        if (chunk) {
            const auto localX = negativeSafeModulo(blockPos.x, size);
            const auto localZ = negativeSafeModulo(blockPos.z, size);
            const auto localY = blockPos.y;
            if (localY >= 0 and localY < size and
                chunk->isCubeInGrid({localX, localY, localZ})) {
                const bool isHit{true};
                const auto normal = -lastStep;
                return HitResult{blockPos, coord, normal, isHit};
            }
        }
        incrementRayStep();
    }
    return std::nullopt;
}

void Raycaster::incrementRayStep() {
    // Advance the ray: choose the smallest nextVoxelBoundary value.
    if (nextVoxelBoundary.x < nextVoxelBoundary.y and
        nextVoxelBoundary.x < nextVoxelBoundary.z) {
        blockPos.x += step.x;
        distanceTraveled = nextVoxelBoundary.x;
        nextVoxelBoundary.x += boundaryStepIncrement.x;
        lastStep = glm::ivec3(step.x, 0, 0);
    } else if (nextVoxelBoundary.y < nextVoxelBoundary.z) {
        blockPos.y += step.y;
        distanceTraveled = nextVoxelBoundary.y;
        nextVoxelBoundary.y += boundaryStepIncrement.y;
        lastStep = glm::ivec3(0, step.y, 0);
    } else {
        blockPos.z += step.z;
        distanceTraveled = nextVoxelBoundary.z;
        nextVoxelBoundary.z += boundaryStepIncrement.z;
        lastStep = glm::ivec3(0, 0, step.z);
    }
}