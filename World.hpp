#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <memory>
#include "Chunk.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"
#include "HitResult.hpp"
#include "Camera.hpp"

class World {
   public:
    World();
    ~World();
    bool addCubeFromRaycast(const Camera& camera, CubeType type,
                            float maxDistance);
    bool removeCubeFromRaycast(const Camera& camera, float maxDistance);
    void updateLoadedChunks(const glm::vec3& camPos);
    void performFrustumCulling(const Frustum& frustum);
    void renderByType(Shader& shader, CubeType type);
    Chunk* getChunk(const ChunkCoord& coord) const;

   private:
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>> loadedChunks;
    ChunkCoord lastCameraChunk{-1000, -1000};
    int renderDistance{4};
    static constexpr int chunkSize = 64;
};