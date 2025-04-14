#pragma once

#include <unordered_map>
#include <memory>
#include <future>
#include <unordered_set>
#include "Chunk.hpp"
#include "ChunkCoord.hpp"

class ChunkLoader {
   public:
    explicit ChunkLoader(int renderingDistance, int sizeOfChunk);
    ~ChunkLoader();
    ChunkLoader(const ChunkLoader&) = delete;
    ChunkLoader(ChunkLoader&&) = delete;
    ChunkLoader& operator=(const ChunkLoader&) = delete;
    ChunkLoader& operator=(ChunkLoader&&) = delete;
    void launchTask(int camChunkX, int camChunkZ,
                    const std::unordered_set<ChunkCoord>& existingKeys);
    bool isTaskRunning() const;
    bool isFinished() const;
    std::unique_ptr<Chunk> createChunk(int x, int z);
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>> retrieveNewChunks();

   private:
    void setupVertexBuffers();
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>
    generateMissingChunks(int camChunkX, int camChunkZ,
                          const std::unordered_set<ChunkCoord>& existingKeys);

    int renderDistance{};
    int chunkSize{};
    unsigned int vertexBufferObjects{0};
    unsigned int cubeElementBufferObjects{0};
    unsigned int waterElementBufferObjects{0};

    std::future<std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>>
        newChunkGroup{};
    bool isRunning{false};
};