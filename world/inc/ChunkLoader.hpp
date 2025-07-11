#pragma once

#include <unordered_map>
#include <memory>
#include <future>
#include <unordered_set>
#include "CpuChunk.hpp"
#include "ChunkCoord.hpp"
#include "VertexData.hpp"

class ChunkLoader {
   public:
    ChunkLoader(int renderingDistance, int sizeOfChunk);
    ~ChunkLoader();
    ChunkLoader(const ChunkLoader&) = delete;
    ChunkLoader(ChunkLoader&&) = delete;
    ChunkLoader& operator=(const ChunkLoader&) = delete;
    ChunkLoader& operator=(ChunkLoader&&) = delete;
    void launchTask(
        int camChunkX, int camChunkZ,
        const std::unordered_set<ChunkCoord, PositionXYHash>& existingKeys);
    bool isTaskRunning() const;
    bool isFinished() const;
    std::unique_ptr<RenderableChunk> createChunk(int x, int z);
    Coord::CpuChunksMap retrieveNewChunks();
    unsigned int getSharedVBO() const { return vertexBufferObjects; }
    unsigned int getSharedEBO() const { return cubeElementBufferObjects; }
    unsigned int getSharedWaterEBO() const { return waterElementBufferObjects; }

   private:
    void setupVertexBuffers();
    Coord::CpuChunksMap generateMissingChunks(
        int camChunkX, int camChunkZ,
        const std::unordered_set<ChunkCoord, PositionXYHash>& existingKeys);
    std::vector<Vertex> vertices{};
    int renderDistance{};
    int chunkSize{};
    unsigned int vertexBufferObjects{0};
    unsigned int cubeElementBufferObjects{0};
    unsigned int waterElementBufferObjects{0};

    std::future<Coord::CpuChunksMap> newChunkGroup{};
    bool isRunning{false};
};