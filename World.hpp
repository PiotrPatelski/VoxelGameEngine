#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <memory>
#include <future>
#include <mutex>
#include "RenderableChunk.hpp"
#include "CpuChunk.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"
#include "ChunkCoord.hpp"
#include "Camera.hpp"
#include "ChunkLoader.hpp"
#include "ChunkUpdater.hpp"

struct ChunkWindow {
    int minX{};
    int maxX{};
    int minZ{};
    int maxZ{};
};

class World {
   public:
    World();
    ~World();
    World(const World&) = delete;
    World(World&&) = delete;
    World& operator=(const World&) = delete;
    World& operator=(World&&) = delete;
    bool addCubeFromRaycast(const Camera& camera, float maxDistance,
                            CubeType type);
    bool removeCubeFromRaycast(const Camera& camera, float maxDistance);
    void updateLoadedChunks(const glm::vec3& camPos);
    void performFrustumCulling(const Frustum& frustum);
    void renderByType(Shader& shader, CubeType type);
    RenderableChunk* getChunk(const ChunkCoord& coord);

   private:
    void loadInitialChunks();
    bool updateCameraChunk(const ChunkCoord& currentCamCoord);
    std::unordered_set<ChunkCoord> getLoadedChunkKeys();
    void mergeNewChunks(
        std::unordered_map<ChunkCoord, std::unique_ptr<CpuChunk>>& newChunks);
    void reloadCurrentlyRelevantChunkGroup(const ChunkCoord& currentCamCoord);
    void runUpdatePerChunk();
    void restoreSavedChunks(const ChunkWindow& window);
    void evictOutOfRangeChunks(const ChunkWindow& window);
    void adjustLoadedChunks(const ChunkCoord& currentCamCoord);

    std::mutex loadedChunksMutex{};
    std::unordered_map<ChunkCoord, std::unique_ptr<RenderableChunk>>
        loadedChunks{};
    std::unordered_map<ChunkCoord, std::unique_ptr<CpuChunk>> savedChunks{};
    std::unordered_map<ChunkCoord, std::unique_ptr<ChunkUpdater>>
        chunkUpdaters{};

    ChunkCoord lastCameraChunk{-1000, -1000};
    int renderDistance{8};
    static constexpr int chunkSize{64};

    std::unique_ptr<ChunkLoader> chunkLoader{};
};