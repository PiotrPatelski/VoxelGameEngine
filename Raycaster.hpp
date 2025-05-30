#pragma once
#include <optional>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "ChunkCoord.hpp"
#include "RenderableChunk.hpp"
#include "Camera.hpp"

int floorDivide(int a, int b);
int negativeSafeModulo(int a, int b);

struct HitResult {
    glm::ivec3 position{};
    ChunkCoord chunkCoord{};
    glm::ivec3 normal{};
    bool hit{false};
};

class Raycaster {
   public:
    Raycaster(const Camera& camera, int chunkSize);
    Raycaster(const Raycaster&) = delete;
    Raycaster(Raycaster&&) = delete;
    Raycaster& operator=(const Raycaster&) = delete;
    Raycaster& operator=(Raycaster&&) = delete;
    std::optional<HitResult> raycastDDA(
        const std::unordered_map<ChunkCoord, std::unique_ptr<RenderableChunk>>&
            loadedChunks,
        float maxDistance);

   private:
    void incrementRayStep();
    int size{0};
    float distanceTraveled{0.0f};
    glm::ivec3 lastStep{0};
    glm::vec3 rayDirection{};
    glm::vec3 rayDir{};
    glm::ivec3 blockPos{};
    glm::ivec3 step{};
    glm::vec3 nextVoxelBoundary{};
    glm::vec3 boundaryStepIncrement{};
};