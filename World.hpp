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
#include "VoxelTypes.hpp"

class World {
   public:
    World();
    ~World();
    World(const World&) = delete;
    World(World&&) = delete;
    World& operator=(const World&) = delete;
    World& operator=(World&&) = delete;
    void setCameraPosition(const glm::vec3& camPos);
    bool addCubeFromRaycast(const Camera& camera, float maxDistance,
                            CubeType type);
    bool removeCubeFromRaycast(const Camera& camera, float maxDistance);
    void updateLoadedChunks();
    void performFrustumCulling(const Frustum& frustum);
    void renderByType(Shader& shader, CubeType type);
    RenderableChunk* getChunk(const ChunkCoord& coord);

   private:
    VoxelTypes::NeighborVoxelsMap gatherEdgeVoxelsFor(
        const ChunkCoord& centerCoord);
    void notifyNeighborChunks(const ChunkCoord& centerCoord);
    void loadInitialChunks();
    bool updateCameraChunk(const ChunkCoord& currentCamCoord);
    std::unordered_set<ChunkCoord, PositionXYHash> getLoadedChunkKeys();
    void mergeNewChunks(Coord::CpuChunksMap& newChunks);
    void reloadCurrentlyRelevantChunkGroup(const ChunkCoord& currentCamCoord);
    void runUpdatePerChunk();
    void restoreSavedChunks(const ChunkWindow& window);
    void evictOutOfRangeChunks(const ChunkWindow& window);
    void adjustLoadedChunks(const ChunkCoord& currentCamCoord);

    void restoreSavedChunk(const ChunkCoord& coord,
                           Coord::CpuChunksMap::iterator& it);
    bool shouldEvictLoadedChunk(const ChunkCoord& coord,
                                const ChunkWindow& window) const;
    void evictLoadedChunk(const ChunkCoord& coord,
                          Coord::RenderableChunksMap::iterator& it);
    void injectNeighborsToModifiedChunks();

    std::mutex loadedChunksMutex{};
    Coord::RenderableChunksMap loadedChunks{};
    Coord::CpuChunksMap savedChunks{};
    Coord::ChunkUpdatersMap chunkUpdaters{};

    ChunkCoord lastCameraChunk{-1000, -1000};
    int renderDistance{8};
    glm::vec3 cameraPosition{};
    static constexpr int chunkSize{64};
    std::unique_ptr<ChunkLoader> chunkLoader{};
};