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

glm::vec3 computeTMax(const glm::vec3& origin, const glm::ivec3& blockPos,
                      const glm::vec3& rayDir, const glm::ivec3& step) {
    // Compute the distance to the next voxel boundary along each axis.
    return (glm::vec3(blockPos) + glm::vec3(step) * 0.5f - origin) / rayDir;
}

glm::vec3 computeTDelta(const glm::vec3& rayDir) {
    return glm::abs(1.0f / rayDir);
}

Chunk* findChunkAtCurrentRayPos(
    const std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>& loadedChunks,
    const ChunkCoord& coords) {
    auto loadedChunk = loadedChunks.find(coords);
    return (loadedChunk != loadedChunks.end()) ? loadedChunk->second.get()
                                               : nullptr;
}
} // namespace

Raycaster::Raycaster(const Camera& camera, int chunkSize)
    : size{chunkSize},
      rayDir{glm::normalize(glm::normalize(camera.getFront()))},
      blockPos{computeInitialBlockPos(camera.getPosition())} {
    step = computeStep(rayDir);
    tMax = computeTMax(camera.getPosition(), blockPos, rayDir, step);
    tDelta = computeTDelta(rayDir);
}

std::optional<HitResult> Raycaster::raycast(
    const std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>& loadedChunks,
    float maxDistance) {
    distanceTraveled = 0.0f;

    while (distanceTraveled < maxDistance) {
        int chunkX = blockPos.x / size;
        int chunkZ = blockPos.z / size;
        ChunkCoord coord{chunkX, chunkZ};
        const Chunk* chunk = findChunkAtCurrentRayPos(loadedChunks, coord);

        if (chunk) {
            // Use bitwise & if chunkSize is a power of 2.
            int localX = blockPos.x & (size - 1);
            int localZ = blockPos.z & (size - 1);
            int localY = blockPos.y;
            if (localY >= 0 && localY < size &&
                chunk->isCubeInGrid({localX, localY, localZ})) {
                return HitResult{blockPos, coord, true};
            }
        }
        incrementRayStep();
    }
    return std::nullopt;
}

void Raycaster::incrementRayStep() {
    // Advance the ray: choose the smallest tMax value.
    if (tMax.x < tMax.y && tMax.x < tMax.z) {
        blockPos.x += step.x;
        distanceTraveled = tMax.x;
        tMax.x += tDelta.x;
    } else if (tMax.y < tMax.z) {
        blockPos.y += step.y;
        distanceTraveled = tMax.y;
        tMax.y += tDelta.y;
    } else {
        blockPos.z += step.z;
        distanceTraveled = tMax.z;
        tMax.z += tDelta.z;
    }
}