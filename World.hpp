#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <memory>
#include <future>
#include <mutex>
#include "Chunk.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"
#include "ChunkCoord.hpp"
#include "Camera.hpp"
#include "ChunkLoader.hpp"
#include "ChunkUpdater.hpp"

class World {
   public:
    World();
    ~World() = default;
    World(const World&) = delete;
    World(World&&) = delete;
    World& operator=(const World&) = delete;
    World& operator=(World&&) = delete;
    bool addCubeFromRaycast(const Camera& camera, float maxDistance);
    bool removeCubeFromRaycast(const Camera& camera, float maxDistance);
    void updateLoadedChunks(const glm::vec3& camPos);
    void performFrustumCulling(const Frustum& frustum);
    void renderByType(Shader& shader, CubeType type);
    Chunk* getChunk(const ChunkCoord& coord);

   private:
    bool updateCameraChunk(const ChunkCoord& currentCamCoord);
    std::unordered_set<ChunkCoord> getLoadedChunkKeys();
    void mergeNewChunks(
        std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>& newChunks);
    void reloadCurrentlyRelevantChunkGroup(const ChunkCoord& currentCamCoord);
    void runUpdatePerChunk();

    std::mutex loadedChunksMutex;
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>> loadedChunks{};
    std::unordered_map<ChunkCoord, std::unique_ptr<ChunkUpdater>> chunkUpdaters;
    ChunkCoord lastCameraChunk{-1000, -1000};
    int renderDistance{8};
    static constexpr int chunkSize{64};

    std::unique_ptr<ChunkLoader> chunkLoader;
};